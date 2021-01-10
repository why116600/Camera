#pragma once

#include "FFCamera.h"

typedef void (*PPACKET_HANDLER_FUNC)(uint8_t* data, int nSize, void* pArg);

class FFEncoder : public FFCameraEventHandler
{
public:
	FFEncoder();
	~FFEncoder();

	bool OpenEncoder(AVCodecID codecID,int width,int height,int bitRate, PPACKET_HANDLER_FUNC pFunc,void *pArg);
	virtual void OnFrameReceived(uint8_t* picBuf[], int linesize[], int width, int height);
	void Close();
	int GetLastError();
private:
	void* m_pArg;
	PPACKET_HANDLER_FUNC m_pPacketHandler;
	AVCodecContext* m_pCtx;
	SwsContext* m_sws;
	int m_iErr;
	int m_width, m_height;
};

