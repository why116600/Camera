#pragma once

#include "FFCamera.h"


class FFEncodeEventHandler
{
public:
	virtual void OnPacketReceived(uint8_t* data, int size) = 0;
};

typedef void (*PPACKET_HANDLER_FUNC)(uint8_t* data, int nSize, void* pArg);

class FFEncoder : public FFCameraEventHandler
{
public:
	FFEncoder();
	~FFEncoder();

	bool OpenEncoder(AVCodecID codecID,int width,int height,int bitRate, AVPixelFormat sourceFmt);
	bool FeedFrame(uint8_t* picBuf[], int linesize[], int width, int height, PPACKET_HANDLER_FUNC pFunc, void* pArg);
	bool FeedFrame(uint8_t* picBuf[], int linesize[], int width, int height, FFEncodeEventHandler* pHandler);
	virtual void OnFrameReceived(uint8_t* picBuf[], int linesize[], int width, int height);
	void Close();
	int GetLastError();

	bool ConnectRtmp(const char* szURL);
	void Disconnect();
private:
	AVPixelFormat m_sourceFmt;
	AVCodecContext* m_pCtx;
	SwsContext* m_sws;
	int m_iErr;
	int m_width, m_height;
	uint8_t* m_data[8];
	int m_linesize[8];
private:
	AVFormatContext* m_rtmp;
	AVStream* m_pStream;
	bool m_bConnected;
	int m_counter;
public:
	FFEncodeEventHandler* m_pPacketHandler = NULL;
	void* m_pArg;
	PPACKET_HANDLER_FUNC m_pPacketHandleFunc;
};

