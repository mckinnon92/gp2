#include "GameApplication.h"

//Creates the vertex buffer we ned to define a stucture to hold the vertex
struct Vertex 
{
	D3DXVECTOR3 Pos;
};



CGameApplication::CGameApplication(void)
{
	m_pWindow=NULL;
	m_pD3D10Device=NULL;
	m_pRenderTargetView=NULL;
	m_pSwapChain=NULL;
	//Sets the vertex
	m_pVertexBuffer=NULL;
	m_pVertexLayout=NULL;
}

CGameApplication::~CGameApplication(void)
{
	if(m_pD3D10Device)
		m_pD3D10Device->ClearState();

	if(m_pVertexBuffer)
		m_pVertexBuffer->Release();
	if(m_pVertexLayout)
		m_pVertexLayout->Release();
	if(m_pEffect)
		m_pEffect->Release();

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
		if (!initGame())
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
		m_pD3D10Device->ClearRenderTargetView(m_pRenderTargetView,ClearColor);

		D3D10_TECHNIQUE_DESC techDesc;
		m_pTechnique->GetDesc(&techDesc);
		for (UINT p = 0; p< techDesc.Passes; ++p)
		{
			m_pTechnique->GetPassByIndex(p)->Apply(0);
			m_pD3D10Device->Draw(3,0);
		}

		//Flips the Swap Chain so the back buffer will be copied to the front buffer and our rendered scene should appear.	
		m_pSwapChain->Present(0,0);
	}

	void CGameApplication::update()
	{
	}

	bool CGameApplication::initGame()
	{
		D3D10_BUFFER_DESC bd;
		//How the buffer is read/written to 
		bd.Usage = D3D10_USAGE_DEFAULT;
		// The size of the bufter (Will hold 3 vertices in this case)
		bd.ByteWidth = sizeof(Vertex)*3;
		//Type of buffer we are creating 
		bd.BindFlags = D3D10_BIND_VERTEX_BUFFER;
		//Specifies that buffer can be read/written by the CPU
		bd.CPUAccessFlags = 0;
		//Used for additional options (none in this case)
		bd.MiscFlags = 0;

		//Defines an array of 3 simple vertices and then we initialize the D3D10_SUBRESOURCE_DATA structure and set the pSysMem variable of this structure to equal our vertices 
		Vertex vertices[] =
		{
			D3DXVECTOR3(0.0f, 0.5f, 0.5f), D3DXVECTOR3(0.5f, -0.5f, 0.5f), D3DXVECTOR3(-0.5f, -0.5f, 0.5f),
		};
		D3D10_SUBRESOURCE_DATA initData;
		initData.pSysMem =vertices;

		DWORD dwShaderFlags = D3D10_SHADER_ENABLE_STRICTNESS;
		#if defined(DEBUG) || defined(_DEBUG)
			dwShaderFlags |= D3D10_SHADER_DEBUG;
		#endif

			ID3D10Blob *pErrors = NULL;

		if(FAILED(D3DX10CreateEffectFromFile(TEXT("ScreenSpace.fx"), NULL, NULL, "fx_4_0", dwShaderFlags, 0, m_pD3D10Device, NULL, NULL, &m_pEffect, &pErrors, NULL)))
		{
			MessageBoxA(NULL,(char*)pErrors->GetBufferPointer(), "Error", MB_OK);
			return false;
		}
		//Retrieve the technique from the effect file
		m_pTechnique=m_pEffect->GetTechniqueByName("Render");

		//Create our buffer
		if (FAILED(m_pD3D10Device->CreateBuffer(
			//Pointer to the buffer description
			&bd,
			//Pointer to resource data
			&initData,
			//Memory address of a pointer to a buffer
			&m_pVertexBuffer)))
			return false;
		

		D3D10_INPUT_ELEMENT_DESC layout[] = 
		{
			{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D10_INPUT_PER_VERTEX_DATA, 0}
		};

		UINT numElements = sizeof(layout)/sizeof(D3D10_INPUT_ELEMENT_DESC);
		D3D10_PASS_DESC PassDesc;
		m_pTechnique->GetPassByIndex(0)->GetDesc(&PassDesc);

		if(FAILED(m_pD3D10Device->CreateInputLayout(layout, numElements, PassDesc.pIAInputSignature, PassDesc.IAInputSignatureSize, &m_pVertexLayout)))
		{
			return false;
		}

		//Tells the Input Assembler about the input layout we have just entered 
		m_pD3D10Device->IASetInputLayout(m_pVertexLayout);
		UINT stride = sizeof(Vertex);
		UINT offset = 0;
		m_pD3D10Device->IASetVertexBuffers(0,1,&m_pVertexBuffer, &stride, &offset);

		m_pD3D10Device->IASetPrimitiveTopology(D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

		return true;
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
		sd.SampleDesc.Quality= 0;

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

		return true;

	}

	

	bool CGameApplication::initWindow()
	{
		m_pWindow=new CWin32Window();
		if (!m_pWindow->init(TEXT("Lab 1 - Create Device"),800,640,false))
			return false;

		return true;


	}