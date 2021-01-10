#include "pch.h"
#include "FFDecoder.h"


FFDecoder::FFDecoder()
	:m_pCtx(NULL)
	, m_sws(NULL)
	, m_pFrame(NULL)
	, m_iErr(0)
	, m_width(0)
	, m_height(0)
{

}

FFDecoder::~FFDecoder()
{
	Close();
}


bool FFDecoder::OpenDecoder(AVCodecID codecID, int width, int height, int bitRate,AVPixelFormat targetFmt)
{
	AVCodec* pCodec;
	pCodec = avcodec_find_decoder(codecID);
	if (!pCodec)
		return false;
	m_pCtx = avcodec_alloc_context3(pCodec);


	m_pCtx->bit_rate = bitRate;
	m_pCtx->width = width;
	m_pCtx->height = height;
	m_pCtx->time_base.den = 25;
	m_pCtx->time_base.num = 1;
	m_pCtx->gop_size = 10;
	m_pCtx->max_b_frames = 1;
	m_pCtx->pix_fmt = AV_PIX_FMT_YUV420P;
	if ((m_iErr = avcodec_open2(m_pCtx, pCodec, NULL)) < 0)
	{
		avcodec_free_context(&m_pCtx);
		m_pCtx = NULL;
		return false;
	}
	m_targetPixFmt = targetFmt;
	m_sws= sws_getContext(width, height, m_pCtx->pix_fmt,
		width, height, targetFmt,
		SWS_BICUBIC, NULL, NULL, NULL);
	m_pFrame = av_frame_alloc();
	av_image_alloc(m_pFrame->data, m_pFrame->linesize, width, height, m_pCtx->pix_fmt, 32);
	m_width = width;
	m_height = height;
	return true;

}

bool FFDecoder::FeedPacket(uint8_t* data, int size, PFRAME_RECV_FUNC pFunc, void* pArg)
{
	if (!m_pCtx)
		return false;
	bool bRet = false;
	AVPacket* pkt;
	uint8_t* buf[8];
	int linesize[8];
	pkt = av_packet_alloc();
	pkt->data = data;
	pkt->size = size;
	av_image_alloc(buf, linesize, m_width, m_height, m_targetPixFmt, 32);
	do
	{
		if ((m_iErr = avcodec_send_packet(m_pCtx, pkt)) < 0)
			break;
		m_iErr = avcodec_receive_frame(m_pCtx, m_pFrame);
		if (m_iErr == AVERROR(EAGAIN))
		{
			bRet = true;
			m_iErr = 0;
			break;
		}
		if (m_iErr < 0)
			break;
		if ((m_iErr = sws_scale(m_sws, m_pFrame->data, m_pFrame->linesize, 0, m_height, buf, linesize)) < 0)
			break;
		pFunc(buf, linesize, m_width, m_height, pArg);
		bRet = true;
	} while (false);
	av_freep(buf);
	av_packet_free(&pkt);
	return bRet;
}

void PacketDecodeHandler(uint8_t* picBuf[], int linesize[], int width, int height, void* pArg)
{
	FFCameraEventHandler* pHandler = (FFCameraEventHandler*)pArg;
	pHandler->OnFrameReceived(picBuf, linesize, width, height);
}

bool FFDecoder::FeedPacket(uint8_t* data, int size, FFCameraEventHandler* pHandler)
{
	return FeedPacket(data, size, PacketDecodeHandler, pHandler);
}

void FFDecoder::OnPacketReceived(uint8_t* data, int size)
{
	if (m_pFrameHandler)
		FeedPacket(data,size, m_pFrameHandler);
	else if (m_pFrameHandleFunc)
		FeedPacket(data,size, m_pFrameHandleFunc, m_pArg);
}

void FFDecoder::Close()
{
	if (m_pFrame)
	{
		av_freep(m_pFrame->data);
		av_frame_free(&m_pFrame);
	}
	if (m_pCtx)
	{
		avcodec_close(m_pCtx);
		avcodec_free_context(&m_pCtx);
	}
	if (m_sws)
	{
		sws_freeContext(m_sws);
		m_sws = NULL;
	}
}
int FFDecoder::GetLastError()
{
	return m_iErr;
}