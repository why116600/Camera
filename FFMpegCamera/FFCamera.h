#pragma once

class FFCameraEventHandler
{
public:
	virtual void OnFrameReceived(uint8_t* picBuf[], int linesize[], int width, int height) = 0;
};

typedef void (*PFRAME_RECV_FUNC)(uint8_t* picBuf[], int linesize[], int width, int height, void* pArg);

class FFCamera
{
public:
	FFCamera();
	~FFCamera();

public:
	ULONGLONG m_u64Latency;

private:
	bool m_bRunning;
	AVInputFormat* m_iFmt;
	AVFormatContext* m_pCtx;
	int m_iVideo;
	AVStream* m_pVideoStream;
	AVCodecContext* m_pDec;
public:
	static FFCamera* CaptureCamera(const char* szFmtName, const char* szCameraName);
	int GetWidth();
	int GetHeight();
	void Close();
	// …„œÒÕ∑œﬂ≥Ã
	void CameraThread(PFRAME_RECV_FUNC pFunc, AVPixelFormat pix_fmt,void *pArg=NULL);
	void CameraThread(FFCameraEventHandler* pHandler, AVPixelFormat pix_fmt);
};

