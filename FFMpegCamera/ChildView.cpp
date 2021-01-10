
// ChildView.cpp: CChildView 类的实现
//

#include "pch.h"
#include "framework.h"
#include "FFMpegCamera.h"
#include "ChildView.h"

#include "FFCamera.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CChildView
#define WM_CAMERA_PICTURE (WM_USER+100)

DWORD WINAPI CameraThread(LPVOID pParam)
{
	CChildView* pView = (CChildView*)pParam;
	pView->m_pCamera->CameraThread(pView, AV_PIX_FMT_BGRA);
	return 0;
}

CChildView::CChildView()
	:m_pCamera(NULL)
{
	m_pCamera = FFCamera::CaptureCamera("dshow", "video=USB2.0 PC CAMERA");
}

CChildView::~CChildView()
{
	if (m_pCamera)
		delete m_pCamera;
}


BEGIN_MESSAGE_MAP(CChildView, CWnd)
	ON_WM_PAINT()
	ON_WM_CREATE()
END_MESSAGE_MAP()



// CChildView 消息处理程序

BOOL CChildView::PreCreateWindow(CREATESTRUCT& cs) 
{
	if (!CWnd::PreCreateWindow(cs))
		return FALSE;

	cs.dwExStyle |= WS_EX_CLIENTEDGE;
	cs.style &= ~WS_BORDER;
	cs.lpszClass = AfxRegisterWndClass(CS_HREDRAW|CS_VREDRAW|CS_DBLCLKS, 
		::LoadCursor(nullptr, IDC_ARROW), reinterpret_cast<HBRUSH>(COLOR_WINDOW+1), nullptr);

	return TRUE;
}

void CChildView::OnPaint() 
{
	CPaintDC dc(this); // 用于绘制的设备上下文
	
	// TODO: 在此处添加消息处理程序代码
	
	// 不要为绘制消息而调用 CWnd::OnPaint()
}

void CChildView::OnFrameReceived(uint8_t* picBuf[], int linesize[], int width, int height)
{
	RECT rc = { 0,0,width,height };
	SendMessage(WM_CAMERA_PICTURE, (WPARAM)picBuf[0], (LPARAM)&rc);
}

BOOL CChildView::OnWndMsg(UINT message, WPARAM wParam, LPARAM lParam, LRESULT* pResult)
{
	// TODO: 在此添加专用代码和/或调用基类
	uint8_t* picBuf;
	RECT* pRC;
	CBitmap bmp;
	CDC mdc;
	if (message == WM_CAMERA_PICTURE)
	{
		CClientDC dc(this);
		picBuf = (uint8_t*)wParam;
		pRC = (RECT*)lParam;
		bmp.CreateCompatibleBitmap(&dc, pRC->right, pRC->bottom);
		bmp.SetBitmapBits(pRC->right * pRC->bottom * 4, picBuf);
		mdc.CreateCompatibleDC(&dc);
		mdc.SelectObject(&bmp);
		dc.BitBlt(0, 0, pRC->right, pRC->bottom, &mdc, 0, 0, SRCCOPY);
		bmp.DeleteObject();
	}

	return CWnd::OnWndMsg(message, wParam, lParam, pResult);
}


int CChildView::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CWnd::OnCreate(lpCreateStruct) == -1)
		return -1;

	CreateThread(NULL, 0, CameraThread, this, 0, NULL);

	return 0;
}
