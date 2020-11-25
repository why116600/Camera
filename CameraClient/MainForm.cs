using AForge.Video;
using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;

namespace CameraClient
{
    public partial class MainForm : Form
    {
        MJPEGStream m_Playing = null;
        public MainForm()
        {
            InitializeComponent();
        }

        private void ButtonConnect_Click(object sender, EventArgs e)
        {
            if(m_Playing==null)
            {
                StartPlaying(textURL.Text);
                buttonConnect.Text = "断开";
                textURL.Enabled = false;
            }
            else
            {
                StopPlaying();
                buttonConnect.Text = "连接";
                textURL.Enabled = true;
            }
        }

        public void StartPlaying(string strURL)
        {
            if(m_Playing!=null)
            {
                StopPlaying();
            }
            m_Playing = new MJPEGStream(strURL);
            videoPlayer.VideoSource = m_Playing;
            videoPlayer.Start();
        }

        public void StopPlaying()
        {
            if (m_Playing != null)
            {
                videoPlayer.SignalToStop();
                videoPlayer.WaitForStop();
                m_Playing = null;
                videoPlayer.VideoSource = null;
            }

        }
    }
}
