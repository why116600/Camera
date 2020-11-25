using AForge.Video.DirectShow;
using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;

namespace CameraServer
{
    public partial class ServerForm : Form
    {
        private FilterInfoCollection m_CameraList = null;
        private VideoCaptureDevice m_UsingCamera = null;
        private MJPEGServer m_server = new MJPEGServer();

        public ServerForm()
        {
            InitializeComponent();
        }

        public void UpdateCameraList()
        {
            m_CameraList=new FilterInfoCollection(FilterCategory.VideoInputDevice);
            listCameras.Items.Clear();
            for(int i=0;i<m_CameraList.Count;i++)
            {
                listCameras.Items.Add(m_CameraList[i].Name);
            }
            buttonConnect.Enabled = m_UsingCamera != null;
        }
        private void ServerForm_Load(object sender, EventArgs e)
        {
            UpdateCameraList();
            m_server.StartServer("127.0.0.1", 8080);

        }

        private void ButtonRefresh_Click(object sender, EventArgs e)
        {
            UpdateCameraList();
        }

        private void ListCameras_SelectedValueChanged(object sender, EventArgs e)
        {
            if(listCameras.SelectedIndex>=0 && listCameras.SelectedIndex<m_CameraList.Count)
            {
                buttonConnect.Enabled = true;
            }
        }

        private void ButtonConnect_Click(object sender, EventArgs e)
        {
            //videoPlayer.VideoSource
            if(m_UsingCamera==null)
            {
                if(listCameras.SelectedIndex<0 || listCameras.SelectedIndex>=m_CameraList.Count)
                {
                    MessageBox.Show("请选择一个摄像头");
                    return;
                }
                m_UsingCamera = new VideoCaptureDevice(m_CameraList[listCameras.SelectedIndex].MonikerString);
                videoPlayer.VideoSource = m_UsingCamera;
                videoPlayer.Start();
                buttonConnect.Text = "断开";
            }
            else
            {
                videoPlayer.SignalToStop();
                videoPlayer.WaitForStop();
                videoPlayer.VideoSource = null;
                m_UsingCamera = null;
                buttonConnect.Text = "连接";
            }
        }
    }
}
