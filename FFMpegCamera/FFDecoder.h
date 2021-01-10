#pragma once

#include "FFCamera.h"
#include "FFEncoder.h"

class FFDecoder : public FFEncodeEventHandler
{
public:
	FFDecoder();
	~FFDecoder();

	bool OpenDecoder(AVCodecID codecID, int width, int height, int bitRate, AVPixelFormat targetFmt);
	bool FeedPacket(uint8_t* data, int size, PFRAME_RECV_FUNC pFunc, void* pArg);
	bool FeedPacket(uint8_t* data, int size, FFCameraEventHandler* pHandler);
	void Close();
	int GetLastError();
	virtual void OnPacketReceived(uint8_t* data, int size);
private:
	AVFrame* m_pFrame;
	AVCodecContext* m_pCtx;
	SwsContext* m_sws;
	AVPixelFormat m_targetPixFmt;
	int m_iErr;
	int m_width, m_height;
public:
	FFCameraEventHandler* m_pFrameHandler = NULL;
	void* m_pArg;
	PFRAME_RECV_FUNC m_pFrameHandleFunc;
};

