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
	m_pIndexBuffer=NULL;
	m_pVertexLayout=NULL;
	m_pDepthStencilView=NULL;
	m_pDepthStencilTexture=NULL;
	m_pEffect=NULL;
}

CGameApplication::~CGameApplication(void)
{
	if(m_pD3D10Device)
		m_pD3D10Device->ClearState();

	if(m_pVertexBuffer)
		m_pVertexBuffer->Release();
	if(m_pIndexBuffer)
		m_pIndexBuffer->Release();
	if(m_pVertexLayout)
		m_pVertexLayout->Release();
	if(m_pEffect)
		m_pEffect->Release();
	if(m_pRenderTargetView)
		m_pRenderTargetView->Release();
	if (m_pDepthStencilTexture)
		m_pDepthStencilTexture->Release();
	if (m_pDepthStencilView)
		m_pDepthStencilView->Release();
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
		m_pD3D10Device->ClearDepthStencilView(m_pDepthStencilView, D3D10_CLEAR_DEPTH, 1.0f, 0);

		m_pViewMatrixVariable->SetMatrix((float*)m_matView);

		m_pWorldMatrixVariable->SetMatrix((float*)m_matWorld);

		D3D10_TECHNIQUE_DESC techDesc;
		m_pTechnique->GetDesc(&techDesc);

		for (UINT p = 0; p< techDesc.Passes; ++p)
		{
			m_pTechnique->GetPassByIndex(p)->Apply(0);
			m_pD3D10Device->DrawIndexed(36,0,0);
			//m_pD3D10Device->Draw(3,0);
		}

		//Flips the Swap Chain so the back buffer will be copied to the front buffer and our rendered scene should appear.	
		m_pSwapChain->Present(0,0);
	}

	void CGameApplication::update()
	{

				m_vecRotation.x+=0.0001f;
m_vecRotation.y+=0.0001f;
m_vecRotation.z+=0.0001f;

		D3DXMatrixScaling(&m_matScale, m_vecScale.x, m_vecScale.y, m_vecScale.z);
		D3DXMatrixRotationYawPitchRoll(&m_matRotation, m_vecRotation.y, m_vecRotation.x, m_vecRotation.z);
		D3DXMatrixTranslation(&m_matTranslation, m_vecPosition.x, m_vecPosition.y, m_vecPosition.z);

		D3DXMatrixMultiply(&m_matWorld, &m_matScale, &m_matRotation);
		D3DXMatrixMultiply(&m_matWorld, &m_matWorld, &m_matTranslation);
	}

	bool CGameApplication::initGame()
	{
		D3D10_BUFFER_DESC bd;
		//How the buffer is read/written to 
		bd.Usage = D3D10_USAGE_DEFAULT;
		// The size of the bufter (Will hold 3 vertices in this case)
		bd.ByteWidth = sizeof(Vertex)*8;
		//Type of buffer we are creating 
		bd.BindFlags = D3D10_BIND_VERTEX_BUFFER;
		//Specifies that buffer can be read/written by the CPU
		bd.CPUAccessFlags = 0;
		//Used for additional options (none in this case)
		bd.MiscFlags = 0;

		//Defines an array of 3 simple vertices and then we initialize the D3D10_SUBRESOURCE_DATA structure and set the pSysMem variable of this structure to equal our vertices 
		Vertex vertices[] =
		{
			D3DXVECTOR3(-0.5f, 0.5f, 0.5f), D3DXVECTOR3(0.5f, -0.5f, 0.5f), D3DXVECTOR3(-0.5f, -0.5f, 0.5f), D3DXVECTOR3(0.5f, 0.5f, 0.5f), D3DXVECTOR3(-0.5f, 0.5f, 1.5f), D3DXVECTOR3(0.5f, -0.5f, 1.5f), D3DXVECTOR3(-0.5f, -0.5f, 1.5f), D3DXVECTOR3(0.5f, 0.5f, 1.5f),
		};
		D3D10_SUBRESOURCE_DATA initData;
		initData.pSysMem =vertices;

			//Create our buffer
		if (FAILED(m_pD3D10Device->CreateBuffer(
			//Pointer to the buffer description
			&bd,
			//Pointer to resource data
			&initData,
			//Memory address of a pointer to a buffer
			&m_pVertexBuffer)))
			return false;

			D3D10_BUFFER_DESC ibd;
				//Create the buffer description
		int indices[]={0,1,2,0,1,3,4,5,6,4,5,7,2,5,1,2,5,6,0,6,4,0,6,2,3,5,1,3,5,7,0,7,3,0,7,4};
		//How the buffer is read/written to 
		ibd.Usage = D3D10_USAGE_DEFAULT;
		// The size of the bufter (Will hold 3 vertices in this case)
		ibd.ByteWidth = sizeof(int)*36;
		//Type of buffer we are creating 
		ibd.BindFlags = D3D10_BIND_INDEX_BUFFER;
		//Specifies that buffer can be read/written by the CPU
		ibd.CPUAccessFlags = 0;
		//Used for additional options (none in this case)
		ibd.MiscFlags = 0;
	

		D3D10_SUBRESOURCE_DATA IndexBufferInitialData;
		IndexBufferInitialData.pSysMem = indices;
		if (FAILED(m_pD3D10Device->CreateBuffer (&ibd, &IndexBufferInitialData, &m_pIndexBuffer)))
			return false;
		
		m_pD3D10Device->IASetIndexBuffer(m_pIndexBuffer, DXGI_FORMAT_R32_UINT, 0);

		DWORD dwShaderFlags = D3D10_SHADER_ENABLE_STRICTNESS;
		#if defined(DEBUG) || defined(_DEBUG)
			dwShaderFlags |= D3D10_SHADER_DEBUG;
		#endif

			ID3D10Blob *pErrors = NULL;

		if(FAILED(D3DX10CreateEffectFromFile(TEXT("Transform.fx"), NULL, NULL, "fx_4_0", dwShaderFlags, 0, m_pD3D10Device, NULL, NULL, &m_pEffect, &pErrors, NULL)))
		{
			MessageBoxA(NULL,(char*)pErrors->GetBufferPointer(), "Error", MB_OK);
			return false;
		}
		//Retrieve the technique from the effect file
		m_pTechnique=m_pEffect->GetTechniqueByName("Render");

	
	

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


		D3D10_VIEWPORT vp;
		UINT numViewPorts = 1;
		m_pD3D10Device->RSGetViewports(&numViewPorts,&vp);

		D3DXMatrixPerspectiveFovLH(&m_matProjection,(float)D3DX_PI * 0.25f, vp.Width / (FLOAT)vp.Height, 0.1f, 100.0f);

		D3DXVECTOR3 cameraPosition=D3DXVECTOR3(0.0f,0.0f,-10.0f);
		D3DXVECTOR3 up=D3DXVECTOR3(0.0f,1.0f,0.0f);
		D3DXVECTOR3 lookAt=D3DXVECTOR3(0.0f,0.0f,0.0f);

		D3DXMatrixLookAtLH(&m_matView,&cameraPosition,&lookAt,&up);

		m_pViewMatrixVariable = m_pEffect->GetVariableByName("matView")->AsMatrix();
		m_pProjectionMatrixVariable = m_pEffect->GetVariableByName("matProjection")->AsMatrix();

		m_pProjectionMatrixVariable->SetMatrix((float*)m_matProjection);

		m_vecPosition=D3DXVECTOR3(0.0f, 0.0f, 0.0f);
		m_vecScale= D3DXVECTOR3(1.0f,1.0f,1.0f);
		m_vecRotation = D3DXVECTOR3(0.0f,0.0f,0.0f);
		m_pWorldMatrixVariable = m_pEffect->GetVariableByName("matWorld")->AsMatrix();



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

		D3D10_TEXTURE2D_DESC descDepth;
		descDepth.Width = width;
		descDepth.Height = height;
		descDepth.MipLevels = 1;
		descDepth.ArraySize = 1;
		descDepth.Format = DXGI_FORMAT_D32_FLOAT;
		descDepth.SampleDesc.Count =1;
		descDepth.SampleDesc.Quality = 0;
		descDepth.Usage = D3D10_USAGE_DEFAULT;
		descDepth.BindFlags = D3D10_BIND_DEPTH_STENCIL;
		descDepth.CPUAccessFlags = 0;
		descDepth.MiscFlags = 0;

		if(FAILED(m_pD3D10Device->CreateTexture2D( &descDepth, NULL, &m_pDepthStencilTexture)))
			return false;

		D3D10_DEPTH_STENCIL_VIEW_DESC descDSV;
		descDSV.Format = descDepth.Format;
		descDSV.ViewDimension = D3D10_DSV_DIMENSION_TEXTURE2D;
		descDSV.Texture2D.MipSlice = 0;

		if (FAILED(m_pD3D10Device->CreateDepthStencilView(m_pDepthStencilTexture,NULL,&m_pDepthStencilView)))
		{
			return false;
		}


		//Binds an array of Render Targets to the Output Merger stage of pipeline
		m_pD3D10Device->OMSetRenderTargets(1, &m_pRenderTargetView, m_pDepthStencilView);

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