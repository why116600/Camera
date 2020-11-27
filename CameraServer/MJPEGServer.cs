using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Net;
using System.Net.Sockets;
using System.Text;
using System.Threading;
using System.Threading.Tasks;

namespace CameraServer
{
    class MJPEGServer
    {
        class ClientHandlerSocket
        {
            public Socket m_socket = null;
            public Thread m_thread = null;
            public ServerSocket m_server = null;
            public MJPEGServer m_parent = null;

            public const string strStreamHeader = "HTTP/1.1 200 OK\r\nContent-Type: multipart/x-mixed-replace; boundary=hehe\r\n";
            public const string strPicHeaderFormat = "\r\nhehe\r\nContent-Type: image/jpeg\r\nContent-Length: {0}\r\n\r\n";

            public ClientHandlerSocket(Socket skt, ServerSocket server, MJPEGServer parent)
            {
                m_socket = skt;
                m_server = server;
                m_parent = parent;
                m_thread = new Thread(new ThreadStart(this.ToClientThread));
                m_thread.Start();
            }
            
            public void ToClientThread()
            {
                int nRecv;
                byte[] buffer = new byte[1024];
                //string strMsg;
                MemoryStream msPic = null;
                string strPicHeader;
                //string[] strRequests;
                while(true)
                {
                    try
                    {
                        nRecv = m_socket.Receive(buffer);
                    }
                    catch (SocketException)
                    {
                        break;
                    }
                    if (nRecv <= 0)
                        break;

                    msPic = m_parent.PictureData;
                    if(msPic==null)
                    {
                        break;
                    }
                    try
                    {
                        m_socket.Send(Encoding.ASCII.GetBytes(strStreamHeader));
                        while(m_server.m_bRunning)
                        {
                            strPicHeader = string.Format(strPicHeaderFormat, msPic.Length);
                            m_socket.Send(Encoding.ASCII.GetBytes(strPicHeader));
                            m_socket.Send(msPic.ToArray());
                            Thread.Sleep(40);
                            msPic = m_parent.PictureData;
                            if (msPic == null)
                            {
                                break;
                            }

                        }
                    }
                    catch (SocketException)
                    {
                        break;
                    }
                    break;
                    //strMsg = Encoding.ASCII.GetString(buffer, 0, nRecv);
                    //strRequests = strMsg.Split(new string[] { "\r\n" }, 10, StringSplitOptions.RemoveEmptyEntries);
                    //m_server.AddMessage(strMsg);
                }
                m_socket.Close();
                m_server.DeleteClient(this);
            }

            public void Close()
            {
                m_socket.Close();
            }
        }
        class ServerSocket
        {
            public string m_strServerIP = null;
            public int m_nServerPort = 0;
            public bool m_bRunning = false;
            public Semaphore m_semStarted = new Semaphore(0, 1);
            

            public MJPEGServer m_parent = null;


            TcpListener m_server = null;

            //需要互斥访问的成员
            List<ClientHandlerSocket> m_aClients = new List<ClientHandlerSocket>();
            string m_strError = null;//错误的信息
            List<string> m_aMessages = new List<string>();//收到的字符串消息
            public void MainThread()
            {
                Socket skt;
                m_server = new TcpListener(IPAddress.Parse(m_strServerIP), m_nServerPort);
                try
                {
                    m_server.Start(5);
                }
                catch(SocketException e)
                {
                    lock(this)
                    {
                        m_strError = e.Message;
                    }
                    m_semStarted.Release();
                    return;
                }
                m_semStarted.Release();

                while (m_bRunning)
                {
                    try
                    {
                        skt = m_server.AcceptSocket();
                    }
                    catch (SocketException e)
                    {
                        lock (this)
                        {
                            m_strError = e.Message;
                        }
                        return;
                    }
                    lock(this)
                    {
                        m_aClients.Add(new ClientHandlerSocket(skt, this, m_parent));
                    }
                }
                
            }

            public void DeleteClient(ClientHandlerSocket client)
            {
                lock(this)
                {
                    for(int i= m_aClients.Count-1; i>=0;i--)
                    {
                        if(m_aClients[i]==client)
                        {
                            m_aClients.RemoveAt(i);
                        }
                    }
                }
            }

            public void AddMessage(string strMsg)
            {
                lock(this)
                {
                    m_aMessages.Add(strMsg);
                }
            }

            public void CleanAllMessages()
            {
                lock(this)
                {
                    m_aMessages.Clear();
                }
            }

            public void Close()
            {
                m_server.Stop();
                lock(this)
                {
                    foreach(ClientHandlerSocket client in m_aClients)
                    {
                        client.Close();
                    }
                }
            }

            public string LastError { get
                {
                    string ret;
                    lock(this)
                    {
                        ret = m_strError;
                    }
                    return ret;
                } }

            public string[] Messages { get
                {
                    string[] rets;
                    lock(this)
                    {
                        rets = m_aMessages.ToArray();
                    }
                    return rets;
                }
            }
        }

        ServerSocket m_server = null;
        Thread m_thServer = null;
        MemoryStream m_msPic = null;

        public MemoryStream PictureData
        {
            get
            {
                MemoryStream ms = null;
                BinaryWriter bw;
                lock(this)
                {
                    if (m_msPic == null)
                        return null;
                    ms = new MemoryStream();
                    bw = new BinaryWriter(ms);
                    bw.Write(m_msPic.ToArray());
                }
                return ms;
            }
            set
            {
                MemoryStream ms = null;
                BinaryWriter bw;
                lock(this)
                {
                    if (value == null)
                    {
                        if (m_msPic != null)
                            m_msPic.Close();
                        m_msPic = null;
                        return;
                    }
                    ms = new MemoryStream();
                    bw = new BinaryWriter(ms);
                    bw.Write(value.ToArray());
                    m_msPic = ms;
                }

            }
        }

        public bool StartServer(string strIP,int nPort)
        {
            m_server = new ServerSocket();
            m_server.m_strServerIP = strIP;
            m_server.m_nServerPort = nPort;
            m_server.m_parent = this;
            m_server.m_bRunning = true;
            m_thServer = new Thread(new ThreadStart(m_server.MainThread));
            m_thServer.Start();
            m_server.m_semStarted.WaitOne();

            return m_server.LastError == null;
        }

        public void Close()
        {
            if(m_server!=null)
            {
                m_server.Close();
                m_server = null;
            }
        }
    }
}
