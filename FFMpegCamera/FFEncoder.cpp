#include "pch.h"
#include "FFEncoder.h"

bool g_bNetworkInit = false;

FFEncoder::FFEncoder()
	:m_pCtx(NULL)
	, m_sws(NULL)
	, m_pPacketHandler(NULL)
	, m_pPacketHandleFunc(NULL)
	, m_pArg(NULL)
	, m_iErr(0)
	, m_width(0)
	, m_height(0)
	, m_rtmp(NULL)
	, m_pStream(NULL)
	, m_bConnected(false)
	, m_counter(0)
{

	if (!g_bNetworkInit)
	{
		avformat_network_init();
		g_bNetworkInit = true;
	}
}

FFEncoder::~FFEncoder()
{
	Disconnect();
	Close();

}

bool FFEncoder::OpenEncoder(AVCodecID codecID, int width, int height, int bitRate, AVPixelFormat sourceFmt)
{
	AVCodec* pCodec;
	pCodec = avcodec_find_encoder(codecID);
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
	m_pCtx->pix_fmt = pCodec->pix_fmts[0];//AV_PIX_FMT_YUV420P;
	av_opt_set(m_pCtx->priv_data, "tune", "zerolatency", 0);
	if ((m_iErr = avcodec_open2(m_pCtx, pCodec, NULL)) < 0)
	{
		avcodec_free_context(&m_pCtx);
		m_pCtx = NULL;
		return false;
	}
	m_width = width;
	m_height = height;
	m_sourceFmt = sourceFmt;
	if (sourceFmt != m_pCtx->pix_fmt)
	{
		m_sws = sws_getContext(width, height, sourceFmt,
			width, height, m_pCtx->pix_fmt,
			SWS_BICUBIC, NULL, NULL, NULL);
		av_image_alloc(m_data, m_linesize, width, height, m_pCtx->pix_fmt, 32);
	}
	return true;
}

bool FFEncoder::FeedFrame(uint8_t* picBuf[], int linesize[], int width, int height, PPACKET_HANDLER_FUNC pFunc, void* pArg)
{
	bool bRet = false;
	AVPacket* pkt = av_packet_alloc();
	AVFrame* frame = av_frame_alloc();
	frame->format = m_pCtx->pix_fmt;
	frame->width = m_pCtx->width;
	frame->height = m_pCtx->height;
	if (m_sws)
	{
		sws_scale(m_sws, picBuf, linesize, 0, m_height, m_data, m_linesize);
		memcpy(frame->data, m_data, sizeof(frame->data));
		memcpy(frame->linesize, m_linesize, sizeof(frame->linesize));
	}
	else
	{
		memcpy(frame->data, picBuf, sizeof(frame->data));
		memcpy(frame->linesize, linesize, sizeof(frame->linesize));
	}
	do
	{
		/*m_iErr = AVERROR(EAGAIN);
		while (m_iErr == AVERROR(EAGAIN))
		{
		}
		if (m_iErr < 0)
			break;*/
		if ((m_iErr = avcodec_send_frame(m_pCtx, frame)) < 0)
			break;
		m_iErr = avcodec_receive_packet(m_pCtx, pkt);
		if (m_iErr == AVERROR(EAGAIN))
		{
			bRet = true;
			m_iErr = 0;
			break;
		}
		if (m_rtmp)
		{
			pkt->dts = m_counter;
			pkt->pts = m_counter;
			m_counter++;
			m_iErr = av_interleaved_write_frame(m_rtmp, pkt);
		}
		if(pFunc)
			pFunc(pkt->data, pkt->size, pArg);
		bRet = true;
	} while (false);
	av_frame_free(&frame);
	av_packet_free(&pkt);
	return bRet;
}

void EncoderPacketHandler(uint8_t* data, int nSize, void* pArg)
{
	FFEncodeEventHandler* pHandler = (FFEncodeEventHandler*)pArg;
	pHandler->OnPacketReceived(data, nSize);
}

bool FFEncoder::FeedFrame(uint8_t* picBuf[], int linesize[], int width, int height, FFEncodeEventHandler* pHandler)
{
	return FeedFrame(picBuf, linesize, width, height, EncoderPacketHandler, pHandler);
}

void FFEncoder::OnFrameReceived(uint8_t* picBuf[], int linesize[], int width, int height)
{
	if (m_pPacketHandler)
		FeedFrame(picBuf, linesize, width, height, m_pPacketHandler);
	else if (m_pPacketHandleFunc)
		FeedFrame(picBuf, linesize, width, height, m_pPacketHandleFunc, m_pArg);
}

bool FFEncoder::ConnectRtmp(const char* szURL)
{
	bool bRet = false;
	if ((m_iErr = avformat_alloc_output_context2(&m_rtmp, NULL, "flv", szURL)) < 0)
		return false;
	m_pStream = avformat_new_stream(m_rtmp, NULL);
	if ((m_iErr = avio_open2(&m_rtmp->pb, szURL, AVIO_FLAG_READ_WRITE, NULL, NULL)) < 0)
	{
		avformat_free_context(m_rtmp);
		m_pCtx = NULL;
		return false;

	}
	do
	{
		if ((m_iErr = avcodec_parameters_from_context(m_pStream->codecpar, m_pCtx)) < 0)
			break;
		if ((m_iErr = avformat_write_header(m_rtmp, NULL)) < 0)
			break;
		bRet = true;
		m_bConnected = true;
	} while (false);
	if (!bRet)
	{
		Disconnect();
	}
	m_counter = 0;
	return bRet;
}

void FFEncoder::Close()
{
	if (m_sws)
	{
		av_freep(m_data);
		sws_freeContext(m_sws);
		m_sws = NULL;
	}
	if (m_pCtx)
	{
		avcodec_close(m_pCtx);
		avcodec_free_context(&m_pCtx);
	}
}

void FFEncoder::Disconnect()
{
	if (m_rtmp)
	{
		if (m_bConnected)
		{
			av_write_trailer(m_rtmp);
			m_bConnected = false;
		}
		avio_close(m_rtmp->pb);
		avformat_free_context(m_rtmp);
		m_rtmp = NULL;
		m_pStream = NULL;
	}
}

int FFEncoder::GetLastError()
{
	return m_iErr;
}