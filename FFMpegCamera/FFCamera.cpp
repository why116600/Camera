#include "pch.h"
#include "FFCamera.h"


bool g_bInit = false;

FFCamera::FFCamera()
	:m_iFmt(NULL)
	, m_pCtx(NULL)
	, m_pVideoStream(NULL)
	, m_pDec(NULL)
	, m_iVideo(-1)
	, m_bRunning(false)
{
	if (!g_bInit)
	{
		avdevice_register_all();
		g_bInit = true;
	}
}

FFCamera::~FFCamera()
{
	Close();
}


FFCamera* FFCamera::CaptureCamera(const char* szFmtName, const char* szCameraName)
{
	int err;
	FFCamera* pRet = new FFCamera;
	bool bSuccess = false;
	AVCodec* pCodec;
	do
	{
		pRet->m_iFmt = av_find_input_format(szFmtName);
		if (!pRet->m_iFmt)
			break;
		if ((err = avformat_open_input(&pRet->m_pCtx, szCameraName, pRet->m_iFmt, NULL)) < 0)
			break;

		if ((err = avformat_find_stream_info(pRet->m_pCtx, NULL)) < 0)
			break;
		for (UINT i = 0; i < pRet->m_pCtx->nb_streams; i++)
		{
			if (pRet->m_pCtx->streams[i]->codecpar->codec_type == AVMEDIA_TYPE_VIDEO)
			{
				pRet->m_iVideo = i;
				pRet->m_pVideoStream = pRet->m_pCtx->streams[i];
				break;
			}
		}
		if (pRet->m_iVideo < 0)
			break;
		pCodec = avcodec_find_decoder(pRet->m_pVideoStream->codecpar->codec_id);
		if (!pCodec)
			break;
		pRet->m_pDec = avcodec_alloc_context3(pCodec);
		if ((err = avcodec_parameters_to_context(pRet->m_pDec, pRet->m_pVideoStream->codecpar)) < 0)
			break;
		if ((err = avcodec_open2(pRet->m_pDec, pCodec, NULL)) < 0)
			break;
		bSuccess = true;
	} while (false);
	if (!bSuccess)
	{
		delete pRet;
		pRet = NULL;
	}
	return pRet;
}


void FFCamera::Close()
{
	if (m_bRunning)
	{
		m_bRunning = false;
		return;
	}
	m_iVideo = -1;
	m_pVideoStream = NULL;
	if (m_pDec)
	{
		avcodec_close(m_pDec);
		avcodec_free_context(&m_pDec);
	}
	if (m_pCtx)
		avformat_close_input(&m_pCtx);
}


// ÉãÏñÍ·Ïß³Ì
void FFCamera::CameraThread(PFRAME_RECV_FUNC pFunc, AVPixelFormat pix_fmt, void* pArg)
{
	if (!m_pDec || !m_pCtx)
		return;
	int err = 0;
	uint8_t* rgbData[8];
	int rgbLineSize[8] = { 0 };
	AVFrame* frame = av_frame_alloc();
	AVPacket* pkt = av_packet_alloc();
	SwsContext* sws_ctx = NULL;
	int width = m_pDec->width;
	int height = m_pDec->height;
	sws_ctx= sws_getContext(width, height, m_pDec->pix_fmt,
		width, height, pix_fmt,
		SWS_BICUBIC, NULL, NULL, NULL);
	av_image_alloc(frame->data, frame->linesize, width, height, m_pDec->pix_fmt, 32);
	av_image_alloc(rgbData, rgbLineSize, width, height, pix_fmt, 32);
	m_bRunning = true;
	while (m_bRunning && m_pDec && m_pCtx)
	{
		if ((err = av_read_frame(m_pCtx, pkt)) < 0)
		{
			break;
		}
		if ((err = avcodec_send_packet(m_pDec, pkt)) < 0)
			break;
		while (true)
		{
			err = avcodec_receive_frame(m_pDec, frame);
			if (err == AVERROR(EAGAIN) || err == AVERROR_EOF)
			{
				err = 0;
				break;
			}
			if (err < 0)
				break;
			if ((err = sws_scale(sws_ctx, frame->data, frame->linesize, 0, height, rgbData, rgbLineSize)) < 0)
				break;
			pFunc(rgbData, rgbLineSize, width, height, pArg);
		}
		if (err < 0)
			break;
	}
	if (sws_ctx)
		sws_freeContext(sws_ctx);
	av_freep(rgbData);
	av_freep(frame->data);
	av_frame_free(&frame);
	av_packet_free(&pkt);
	Close();
}

void CameraFrameHandler(uint8_t* picBuf[], int linesize[], int width, int height, void* pArg)
{
	FFCameraEventHandler* pHandler = (FFCameraEventHandler*)pArg;
	pHandler->OnFrameReceived(picBuf, linesize, width, height);
}

void FFCamera::CameraThread(FFCameraEventHandler* pHandler, AVPixelFormat pix_fmt)
{
	CameraThread(CameraFrameHandler, pix_fmt, pHandler);
}