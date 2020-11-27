namespace CameraServer
{
    partial class ServerForm
    {
        /// <summary>
        /// 必需的设计器变量。
        /// </summary>
        private System.ComponentModel.IContainer components = null;

        /// <summary>
        /// 清理所有正在使用的资源。
        /// </summary>
        /// <param name="disposing">如果应释放托管资源，为 true；否则为 false。</param>
        protected override void Dispose(bool disposing)
        {
            if (disposing && (components != null))
            {
                components.Dispose();
            }
            base.Dispose(disposing);
        }

        #region Windows 窗体设计器生成的代码

        /// <summary>
        /// 设计器支持所需的方法 - 不要修改
        /// 使用代码编辑器修改此方法的内容。
        /// </summary>
        private void InitializeComponent()
        {
            this.videoPlayer = new AForge.Controls.VideoSourcePlayer();
            this.label1 = new System.Windows.Forms.Label();
            this.listCameras = new System.Windows.Forms.ListBox();
            this.buttonRefresh = new System.Windows.Forms.Button();
            this.buttonConnect = new System.Windows.Forms.Button();
            this.buttonTakePhoto = new System.Windows.Forms.Button();
            this.SuspendLayout();
            // 
            // videoPlayer
            // 
            this.videoPlayer.Location = new System.Drawing.Point(318, 36);
            this.videoPlayer.Margin = new System.Windows.Forms.Padding(4);
            this.videoPlayer.Name = "videoPlayer";
            this.videoPlayer.Size = new System.Drawing.Size(936, 526);
            this.videoPlayer.TabIndex = 0;
            this.videoPlayer.Text = "videoPlayer";
            this.videoPlayer.VideoSource = null;
            this.videoPlayer.NewFrame += new AForge.Controls.VideoSourcePlayer.NewFrameHandler(this.videoPlayer_NewFrame);
            // 
            // label1
            // 
            this.label1.AutoSize = true;
            this.label1.Location = new System.Drawing.Point(18, 14);
            this.label1.Margin = new System.Windows.Forms.Padding(4, 0, 4, 0);
            this.label1.Name = "label1";
            this.label1.Size = new System.Drawing.Size(98, 18);
            this.label1.TabIndex = 1;
            this.label1.Text = "摄像头列表";
            // 
            // listCameras
            // 
            this.listCameras.FormattingEnabled = true;
            this.listCameras.ItemHeight = 18;
            this.listCameras.Location = new System.Drawing.Point(21, 36);
            this.listCameras.Margin = new System.Windows.Forms.Padding(4);
            this.listCameras.Name = "listCameras";
            this.listCameras.Size = new System.Drawing.Size(240, 436);
            this.listCameras.TabIndex = 2;
            this.listCameras.SelectedValueChanged += new System.EventHandler(this.ListCameras_SelectedValueChanged);
            // 
            // buttonRefresh
            // 
            this.buttonRefresh.Location = new System.Drawing.Point(21, 483);
            this.buttonRefresh.Margin = new System.Windows.Forms.Padding(4);
            this.buttonRefresh.Name = "buttonRefresh";
            this.buttonRefresh.Size = new System.Drawing.Size(112, 34);
            this.buttonRefresh.TabIndex = 3;
            this.buttonRefresh.Text = "刷新";
            this.buttonRefresh.UseVisualStyleBackColor = true;
            this.buttonRefresh.Click += new System.EventHandler(this.ButtonRefresh_Click);
            // 
            // buttonConnect
            // 
            this.buttonConnect.Enabled = false;
            this.buttonConnect.Location = new System.Drawing.Point(150, 483);
            this.buttonConnect.Margin = new System.Windows.Forms.Padding(4);
            this.buttonConnect.Name = "buttonConnect";
            this.buttonConnect.Size = new System.Drawing.Size(112, 34);
            this.buttonConnect.TabIndex = 3;
            this.buttonConnect.Text = "连接";
            this.buttonConnect.UseVisualStyleBackColor = true;
            this.buttonConnect.Click += new System.EventHandler(this.ButtonConnect_Click);
            // 
            // buttonTakePhoto
            // 
            this.buttonTakePhoto.Location = new System.Drawing.Point(150, 528);
            this.buttonTakePhoto.Name = "buttonTakePhoto";
            this.buttonTakePhoto.Size = new System.Drawing.Size(112, 34);
            this.buttonTakePhoto.TabIndex = 4;
            this.buttonTakePhoto.Text = "拍照";
            this.buttonTakePhoto.UseVisualStyleBackColor = true;
            this.buttonTakePhoto.Click += new System.EventHandler(this.buttonTakePhoto_Click);
            // 
            // ServerForm
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(9F, 18F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(1272, 600);
            this.Controls.Add(this.buttonTakePhoto);
            this.Controls.Add(this.buttonConnect);
            this.Controls.Add(this.buttonRefresh);
            this.Controls.Add(this.listCameras);
            this.Controls.Add(this.label1);
            this.Controls.Add(this.videoPlayer);
            this.Margin = new System.Windows.Forms.Padding(4);
            this.MaximizeBox = false;
            this.Name = "ServerForm";
            this.Text = "摄像头服务器";
            this.Load += new System.EventHandler(this.ServerForm_Load);
            this.ResumeLayout(false);
            this.PerformLayout();

        }

        #endregion

        private AForge.Controls.VideoSourcePlayer videoPlayer;
        private System.Windows.Forms.Label label1;
        private System.Windows.Forms.ListBox listCameras;
        private System.Windows.Forms.Button buttonRefresh;
        private System.Windows.Forms.Button buttonConnect;
        private System.Windows.Forms.Button buttonTakePhoto;
    }
}

