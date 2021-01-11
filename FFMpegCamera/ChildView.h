
// ChildView.h: CChildView 类的接口
//


#pragma once


#include "FFCamera.h"
#include "FFEncoder.h"
#include "FFDecoder.h"

// CChildView 窗口

class CChildView : public CWnd ,public FFCameraEventHandler
{
// 构造
public:
	CChildView();

// 特性
public:
	FFCamera* m_pCamera;

	FFEncoder m_encoder;
	FFDecoder m_decoder;

	BYTE* m_bufPic;
	DWORD m_dwSize;
	CRect m_rcCamera;
// 操作
public:
	virtual void OnFrameReceived(uint8_t* picBuf[], int linesize[], int width, int height);
// 重写
	protected:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);

// 实现
public:
	virtual ~CChildView();

	// 生成的消息映射函数
protected:
	afx_msg void OnPaint();
	DECLARE_MESSAGE_MAP()
	virtual BOOL OnWndMsg(UINT message, WPARAM wParam, LPARAM lParam, LRESULT* pResult);
public:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
};

