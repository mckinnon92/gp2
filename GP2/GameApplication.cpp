#include "GameApplication.h"

CGameApplication::CGameApplication(void)
{
	m_pWindow=NULL;
	m_pD3D10Device=NULL;
	m_pRenderTargetView=NULL;
	m_pSwapChain=NULL;
}

CGameApplication::~CGameApplication(void)
{
	if(m_pD3D10Device)
		m_pD3D10Device->ClearState();

	if(m_pRenderTargetView)
		m_pRenderTargetView->Release();
	if(m_pSwapChain)
		m_pSwapChain->Release();
	if (m_pD3D10Device)
		m_pD3D10Device->Release();

	if (m_pWindow)
	{
		delete m_pWindow;
		m_pWindow=NULL;
	}
}

			bool CGameApplication::init()
	{
		if (!initWindow())
			return false;

		if (!initGraphics())
			return false;

		return true;
	}

	bool CGameApplication::run()
	{
		while(m_pWindow->running())
		{
			if(!m_pWindow->checkForWindowMessages())
			{
				update();
				render();
			}
		}
		return false;
	}

	void CGameApplication::render()
	{
		//Sets up a float array for colors (R,G,B,A), which has values from 0 to 1 for each component
		float ClearColor[4] = { 0.0f, 0.125f, 0.3f, 1.0f };
		//Uses the above color value and will clear the render target to that color
		m_pD3D10Device->ClearRenderTargetView( m_pRenderTargetView, ClearColor);
		//Flips the Swap Chain so the back buffer will be copied to the front buffer and our rendered scene should appear.	
		m_pSwapChain->Present(0,0);
	}

	void CGameApplication::update()
	{
	}

	bool CGameApplication::initGraphics()
	{
		//initializes the Direct3D10
		RECT windowRect;
		GetClientRect(m_pWindow->getHandleToWindow(),&windowRect);

		UINT width=windowRect.right-windowRect.left;
		UINT height=windowRect.bottom-windowRect.top;

		//Retrieves the width and height of the window, stores these values 
		UINT createDeviceFlags=0;
#ifdef _DEBUG
		createDeviceFlags|=D3D10_CREATE_DEVICE_DEBUG;
#endif

		DXGI_SWAP_CHAIN_DESC sd;
		ZeroMemory(&sd, sizeof(sd));
		return true;

		//Checks to see if the application window is in full screen, if it is we use two buffers
		if (m_pWindow->isFullScreen())
			sd.BufferCount = 2;
		else
			sd.BufferCount = 1;

		sd.OutputWindow = m_pWindow->getHandleToWindow();
		sd.Windowed = (BOOL)(!m_pWindow->isFullScreen());
		sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;

		//Sets multisampling parameters for a swap chain
		sd.SampleDesc.Count = 1;
		sd.SampleDesc.Count = 0;

		//Sets the width and height of the buffer
		sd.BufferDesc.Width = width;
		sd.BufferDesc.Height = height;
		//Sets the format of the buffer
		sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		//Set the refresh rate of 60Hz
		sd.BufferDesc.RefreshRate.Numerator = 60;
		sd.BufferDesc.RefreshRate.Denominator = 1;

		//Creates a swap chain and device in one call
		if (FAILED(D3D10CreateDeviceAndSwapChain(NULL, D3D10_DRIVER_TYPE_HARDWARE, NULL, createDeviceFlags, D3D10_SDK_VERSION, &sd, &m_pSwapChain, &m_pD3D10Device)))
			return false; 

		//Associates a buffer from the swap chain with the Render Target View
		ID3D10Texture2D *pBackBuffer;
		if ( FAILED (m_pSwapChain->GetBuffer(0,__uuidof(ID3D10Texture2D), (void**)&pBackBuffer)))
			return false;

		//Creates the Render Target View
		if (FAILED(m_pD3D10Device->CreateRenderTargetView(pBackBuffer, NULL, &m_pRenderTargetView)))
		{
			pBackBuffer->Release();
			return false;
		}
		pBackBuffer->Release();

		//Binds an array of Render Targets to the Output Merger stage of pipeline
		m_pD3D10Device->OMSetRenderTargets(1, &m_pRenderTargetView, NULL);

		//Sets up a D3D10_VIEWPORT instance, this is the same width and height of the window 
		D3D10_VIEWPORT vp;	
		vp.Width = width;
		vp.Height = height;
		vp.MinDepth = 0.0f;
		vp.MaxDepth = 1.0f;
		vp.TopLeftX = 0;
		vp.TopLeftY = 0;
		//Sets the view port which is bound to the pipeline
		m_pD3D10Device->RSSetViewports(1, &vp);

	}

	

	bool CGameApplication::initWindow()
	{
		m_pWindow=new CWin32Window();
		if (!m_pWindow->init(TEXT("Lab 1 - Create Device"),800,640,false))
			return false;

		return true;
	}