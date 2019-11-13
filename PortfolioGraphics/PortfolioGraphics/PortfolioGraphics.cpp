// PortfolioGraphics.cpp : Defines the entry point for the application.
//

#include "framework.h"
#include "PortfolioGraphics.h"
#include "MyVShader.csh"
#include "MyPShader.csh"
#include "MyVMeshShader.csh" // don't add a .csh to your project!
#include "MyVSkyShader.csh"
#include "MyPSkyShader.csh"
#include "Assets/StoneHenge.h"
#include "Assets/StoneHenge_Texture.h"
#include "Assets/wheel.h"
#include "Assets/skyBox.h"
#include "Assets/DDSTextureLoader.h"
#include <d3d11.h>
#pragma comment(lib, "d3d11.lib")
#include <DirectXMath.h>
#include <windows.h>
#include "XTime.h"
using namespace DirectX;
using namespace std;
//for init
ID3D11Device* myDev;
IDXGISwapChain* mySwap;
ID3D11DeviceContext* myCon;

XTime timer;
float speed = 5.0f;
float zoom = 1.0f;
float zNear = 0.1f;
float zFar = 1000.0f;
XMMATRIX camera;

//for drawing
ID3D11RenderTargetView* myRtv;
D3D11_VIEWPORT myPort;
float aspectRatio = 1;
struct MyVertex
{
	float xyzw[4];
	float rgba[4];
};
unsigned int numVerts = 0;

ID3D11Buffer* vBuff;
ID3D11InputLayout* vLayout;
ID3D11VertexShader* vShader; //HLSL
ID3D11PixelShader* pShader; //HLSL

ID3D11Buffer* cBuff;//shader vars

// mesh data
ID3D11Buffer* vBuffMesh; //vertex buffer
ID3D11Buffer* iBuffMesh; //index buffer

// Z buffer for debth sorting
ID3D11Texture2D* zBuffer;
ID3D11DepthStencilView* zBufferView;

// texture variables
ID3D11Texture2D* diffuseTexture; // what we load our pixel data into
ID3D11ShaderResourceView* srv;
ID3D11SamplerState* SS = nullptr;

// mesh vertex shader
ID3D11VertexShader* vMeshShader; // HLSL
ID3D11InputLayout* vMeshLayout;
ID3D11InputLayout* vMeshLayout2;

ID3D11Buffer* skyVBuff;
ID3D11Buffer* skyIBuff;
ID3D11VertexShader* skyVShader;
ID3D11PixelShader* skyPShader;
ID3D11Texture2D* skyTex;
ID3D11ShaderResourceView* skySRV;
//ID3D11InputLayout skyLayout;


// Math stuff
struct WVP
{
	XMFLOAT4X4 wMatrix; // storage type
	XMFLOAT4X4 vMatrix;
	XMFLOAT4X4 pMatrix;
}MyMatricies;

#define MAX_LOADSTRING 100

// Global Variables:
HINSTANCE hInst;                                // current instance
WCHAR szTitle[MAX_LOADSTRING];                  // The title bar text
WCHAR szWindowClass[MAX_LOADSTRING];            // the main window class name

// Forward declarations of functions included in this code module:
ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
	_In_opt_ HINSTANCE hPrevInstance,
	_In_ LPWSTR    lpCmdLine,
	_In_ int       nCmdShow)
{
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);

	// TODO: Place code here.
	
	// Initialize global strings
	LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
	LoadStringW(hInstance, IDC_PORTFOLIOGRAPHICS, szWindowClass, MAX_LOADSTRING);
	MyRegisterClass(hInstance);

	// Perform application initialization:
	if (!InitInstance(hInstance, nCmdShow))
	{
		return FALSE;
	}

	HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_PORTFOLIOGRAPHICS));

	MSG msg;

	camera = XMMatrixInverse(nullptr, XMMatrixLookAtLH({ 0,0,0 }, { 1,0,0 }, { 0,1,0 }));
	// Main message loop:
	while (true)
	{
		timer.Signal(); 
		PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE);
		if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		if (msg.wParam == WM_QUIT)
			break;

		// rendering here
		float color[] = { 0.0f, 1.0f, 1.0f, 1.0f };
		myCon->ClearRenderTargetView(myRtv, color);

		myCon->ClearDepthStencilView(zBufferView, D3D11_CLEAR_DEPTH, 1, 0);
		
		//Setup the pipeline
		
		//output manager
		myCon->PSSetSamplers(0, 1, &SS);
		ID3D11RenderTargetView* tempRTV[] = { myRtv };
		myCon->OMSetRenderTargets(1, tempRTV, zBufferView);
		//rasterizer
		myCon->RSSetViewports(1, &myPort);
		//input assembler
		//myCon->IASetInputLayout(vLayout);
		ID3D11Buffer* tempVB[] = {vBuff};
		/*UINT strides[] = {sizeof(MyVertex)};
		UINT offsets[] = {0};
		myCon->IASetVertexBuffers(0, 1, tempVB, strides, offsets);*/
		myCon->IASetPrimitiveTopology((D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST));
		ID3D11Buffer* constants[] = { cBuff };
		myCon->VSSetConstantBuffers(0, 1, constants);
		//Vertex Shader Stage
		//myCon->VSSetShader(vShader, 0, 0);
		////Pixel Shader Stage
		//myCon->PSSetShader(pShader, 0, 0);
		
		
		// Try and make the triangle 3d  -  Check

			//make into a pyramid (more verts)  -  Check

			//make a world view & projection matrix
			static float rot = 0; //rot += 0.001f;
			XMMATRIX temp = XMMatrixIdentity();
			temp = XMMatrixTranslation(3, 2, -5);
			XMMATRIX temp2 = XMMatrixRotationY(rot);
			temp = XMMatrixMultiply(temp2, temp);
			XMStoreFloat4x4(&MyMatricies.wMatrix, temp);

			float dTime = timer.Delta();
		{
			
			//view movement
				//W (Forward)
			if (GetAsyncKeyState(0x57))
			{
				XMMATRIX tmp = XMMatrixTranslation(0, 0, speed * dTime * 3);
				camera = XMMatrixMultiply(tmp, camera);
			}
				//S (Backward)
			if (GetAsyncKeyState(0x53))
			{
				XMMATRIX tmp = XMMatrixTranslation(0, 0, -speed * dTime* 3);
				camera = XMMatrixMultiply(tmp, camera);
			}
				//A (Left)
			if (GetAsyncKeyState(0x41))
			{
				XMMATRIX tmp = XMMatrixTranslation(-speed * dTime * 3, 0, 0);
				camera = XMMatrixMultiply(tmp, camera);
			}
				//D (Right)
			if (GetAsyncKeyState(0x44))
			{
				XMMATRIX tmp = XMMatrixTranslation(speed * dTime * 3, 0, 0);
				camera = XMMatrixMultiply(tmp, camera);
			}
				//Spacebar (up)
			if (GetAsyncKeyState(VK_SPACE))
			{
				XMMATRIX tmp = XMMatrixTranslation(0, speed * dTime * 3, 0);
				camera = XMMatrixMultiply(tmp, camera);
			}
				//Left Shift (Down)
			if (GetAsyncKeyState(VK_LSHIFT))
			{
				XMMATRIX tmp = XMMatrixTranslation(0, -speed * dTime * 3, 0);
				camera = XMMatrixMultiply(tmp, camera);
			}
				//Up Arrow (Look Up)
			if (GetAsyncKeyState(VK_UP))
			{
				XMVECTOR pos = camera.r[3];
				camera.r[3] = XMVectorSet(0, 0, 0, 1);
				XMMATRIX rotX = XMMatrixRotationX(-speed * dTime);
				camera = XMMatrixMultiply(rotX, camera);
				camera.r[3] = pos;
			}
				//Down Arrow (Look Down)
			if (GetAsyncKeyState(VK_DOWN))
			{
				XMVECTOR pos = camera.r[3];
				camera.r[3] = XMVectorSet(0, 0, 0, 1);
				XMMATRIX rotX = XMMatrixRotationX(speed * dTime);
				camera = XMMatrixMultiply(rotX, camera);
				camera.r[3] = pos;
			}
			//Left Arrow (Look Left)
			if (GetAsyncKeyState(VK_LEFT))
			{
				XMVECTOR pos = camera.r[3];
				camera.r[3] = XMVectorSet(0, 0, 0, 1);
				XMMATRIX rotY = XMMatrixRotationY(-speed * dTime);
				camera = XMMatrixMultiply(camera, rotY);
				camera.r[3] = pos;
			}
			//Right Arrow (Look Right)
			if (GetAsyncKeyState(VK_RIGHT))
			{
				XMVECTOR pos = camera.r[3];
				camera.r[3] = XMVectorSet(0, 0, 0, 1);
				XMMATRIX rotY = XMMatrixRotationY(speed * dTime);
				camera = XMMatrixMultiply(camera, rotY);
				camera.r[3] = pos;
			}
			//Minus (zoom out)
			if (GetAsyncKeyState(VK_OEM_MINUS))
			{
				if(zoom > 0.9f)
					zoom -= 0.01f;
			}
			//Plus (zoom in)
			if (GetAsyncKeyState(VK_OEM_PLUS))
			{
				if (zoom < 5.0f)
					zoom += 0.01f;
			}
			//{ (near plane closer)
			if (GetAsyncKeyState(VK_OEM_4))
			{
				if (zNear > 0.1f)
					zNear -= 0.1f;
			}
			//} (near plane further)
			if (GetAsyncKeyState(VK_OEM_6))
			{
				if (zNear <= zFar)
					zNear += 0.1f;
			}
			//: (far plane closer)
			if (GetAsyncKeyState(VK_OEM_1))
			{
				if (zFar >= zNear)
					zFar -= 0.1f;
			}
			//' (far plane further)
			if (GetAsyncKeyState(VK_OEM_7))
			{
				if (zFar < 10000.0f)
					zFar += 0.1f;
			}
		}
			//view
			XMStoreFloat4x4(&MyMatricies.vMatrix, XMMatrixInverse(nullptr, camera));
			//projection
			temp = XMMatrixPerspectiveFovLH(3.14f / (2.0f * zoom), aspectRatio, zNear, zFar);
			XMStoreFloat4x4(&MyMatricies.pMatrix, temp);

		
			//upload those matricies to the video card
				//create and update a constant buffer (move variables from c++ to shaders)
			D3D11_MAPPED_SUBRESOURCE gpuBuffer;
			HRESULT hr = myCon->Map(cBuff, 0, D3D11_MAP_WRITE_DISCARD, 0, &gpuBuffer);
			*((WVP*)(gpuBuffer.pData)) = MyMatricies;
			//memcpy(gpuBuffer.pData, &MyMatricies, sizeof(WVP));
			myCon->Unmap(cBuff, 0);
		
			UINT mesh_sky_strides[] = { sizeof(_OBJ_VERT_) };
			UINT mesh_sky_offsets[] = { 0 };
			ID3D11Buffer* skyMeshVB[] = { skyVBuff };
			myCon->IASetVertexBuffers(0, 1, skyMeshVB, mesh_sky_strides, mesh_sky_offsets);
			myCon->IASetIndexBuffer(skyIBuff, DXGI_FORMAT_R32_UINT, 0);
			myCon->VSSetShader(skyVShader, 0, 0);
			myCon->PSSetShader(skyPShader, 0, 0);
			myCon->IASetInputLayout(vMeshLayout);


			temp = XMMatrixIdentity();
			XMVECTOR camPos = camera.r[3];
		
		    temp = XMMatrixTranslation(XMVectorGetX(camPos), XMVectorGetY(camPos), XMVectorGetZ(camPos));
			XMStoreFloat4x4(&MyMatricies.wMatrix, temp);

			myCon->Map(cBuff, 0, D3D11_MAP_WRITE_DISCARD, 0, &gpuBuffer);
			*((WVP*)(gpuBuffer.pData)) = MyMatricies;
			myCon->Unmap(cBuff, 0);
			ID3D11ShaderResourceView* texSkyViews[] = { skySRV };
			myCon->PSSetShaderResources(0, 1, texSkyViews);
			myCon->DrawIndexed(sizeof(skyBox_indicies), 0, 0);

			myCon->ClearDepthStencilView(zBufferView, D3D11_CLEAR_DEPTH, 1, 0);
			// Apply matrix math in Vertex Shader  -  check
			// connect constant buffer to pipeline  -  check
			// remember by default HLSL matricies are COLUMN MAJOR
			
		
			// Draw?
			//myCon->Draw(numVerts, 0);

			//immediate context

			//get a more complex pre-made mesh (FBX, OBJ, custom header)
			//load it onto the card (vertex buffer, index buffer)
			//make sure our shaders can process it
			//place it somewhere else in the enviroment

			//set pipeline
			UINT mesh_strides[] = { sizeof(_OBJ_VERT_) };
			UINT mesh_offsets[] = { 0 };
			ID3D11Buffer* meshVB[] = { vBuffMesh };
			myCon->IASetVertexBuffers(0, 1, meshVB, mesh_strides, mesh_offsets);
			myCon->IASetIndexBuffer(iBuffMesh, DXGI_FORMAT_R32_UINT, 0);
			myCon->VSSetShader(vMeshShader, 0, 0);
			myCon->PSSetShader(pShader, 0, 0);
			myCon->IASetInputLayout(vMeshLayout);

			ID3D11ShaderResourceView* texViews[] = { srv };
			myCon->PSSetShaderResources(0, 1, texViews);
			
		


			// modify world matrix before drawing next thing
			temp = XMMatrixIdentity();
			XMStoreFloat4x4(&MyMatricies.wMatrix, temp);
			// send it to the CARD
			hr = myCon->Map(cBuff, 0, D3D11_MAP_WRITE_DISCARD, 0, &gpuBuffer);
			*((WVP*)(gpuBuffer.pData)) = MyMatricies;
			//memcpy(gpuBuffer.pData, &MyMatricies, sizeof(WVP));
			myCon->Unmap(cBuff, 0);

			myCon->DrawIndexedInstanced(sizeof(StoneHenge_indicies), 2, 0, 0, 0);
		mySwap->Present(0, 0);  //can limit framerate and synch with these params
	}
	//release all our D3D11 interfaces
	
	myRtv->Release();
	vBuff->Release();
	myCon->Release();
	mySwap->Release();
	vShader->Release();
	pShader->Release();
	vLayout->Release();
	myDev->Release();

	return (int)msg.wParam;
}



//
//  FUNCTION: MyRegisterClass()
//
//  PURPOSE: Registers the window class.
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
	WNDCLASSEXW wcex;

	wcex.cbSize = sizeof(WNDCLASSEX);

	wcex.style = CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc = WndProc;
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.hInstance = hInstance;
	wcex.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_PORTFOLIOGRAPHICS));
	wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
	wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	wcex.lpszMenuName = MAKEINTRESOURCEW(IDC_PORTFOLIOGRAPHICS);
	wcex.lpszClassName = szWindowClass;
	wcex.hIconSm = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

	return RegisterClassExW(&wcex);
}

//
//   FUNCTION: InitInstance(HINSTANCE, int)
//
//   PURPOSE: Saves instance handle and creates main window
//
//   COMMENTS:
//
//        In this function, we save the instance handle in a global variable and
//        create and display the main program window.
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
	hInst = hInstance; // Store instance handle in our global variable

	HWND hWnd = CreateWindowW(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, nullptr, nullptr, hInstance, nullptr);

	if (!hWnd)
	{
		return FALSE;
	}

	ShowWindow(hWnd, nCmdShow);
	UpdateWindow(hWnd);
	
	RECT myWinR;
	GetClientRect(hWnd, &myWinR);
	
	//  attach d3d11 to our window
	D3D_FEATURE_LEVEL dx11 = D3D_FEATURE_LEVEL_11_0;
	DXGI_SWAP_CHAIN_DESC swap;
	ZeroMemory(&swap, sizeof(DXGI_SWAP_CHAIN_DESC));
	swap.BufferCount = 2;
	swap.OutputWindow = hWnd;
	swap.Windowed = true;
	swap.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
	swap.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	swap.BufferDesc.Width = myWinR.right - myWinR.left;
	swap.BufferDesc.Height = myWinR.bottom - myWinR.top;
	swap.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	swap.SampleDesc.Count = 1;

	aspectRatio = swap.BufferDesc.Width / float(swap.BufferDesc.Height);
	
	HRESULT hr;
	hr = D3D11CreateDeviceAndSwapChain(nullptr, D3D_DRIVER_TYPE_HARDWARE, nullptr, D3D11_CREATE_DEVICE_DEBUG,
		&dx11, 1, D3D11_SDK_VERSION, &swap, &mySwap, &myDev, nullptr, &myCon);


	ID3D11Resource* backBuffer;
	hr = mySwap->GetBuffer(0, _uuidof(backBuffer), (void**)(&backBuffer));
	hr = myDev->CreateRenderTargetView(backBuffer, nullptr, &myRtv);

	backBuffer->Release();

	myPort.Width = swap.BufferDesc.Width;
	myPort.Height = swap.BufferDesc.Height;
	myPort.TopLeftX = myPort.TopLeftY = 0;
	myPort.MinDepth = 0;
	myPort.MaxDepth = 1;

	MyVertex tri[] = // NDC Normalized Device Coordinates
	{ // xyzw, rgba
		// front
		{{0, 1.0f, 0, 1}, {1,1,1,1}},
		{{0.25f, -0.25, -0.25f, 1}, {1,0,1,1}},
		{{-0.25f, -0.25, -0.25f, 1}, {1,1,0,1}},
		// right
		{ {0, 1.0f, 0, 1}, {1,1,1,1}},
		{{0.25f, -0.25, 0.25f, 1}, {1,0,1,1}},
		{{0.25f, -0.25, -0.25f, 1}, {1,1,0,1}},
		// back
		{ {0, 1.0f, 0, 1}, {1,1,1,1}},
		{{-0.25f, -0.25, 0.25f, 1}, {1,0,1,1}},
		{{0.25f, -0.25, 0.25f, 1}, {1,1,0,1}},
		// left
		{{0, 1.0f, 0, 1}, {1,1,1,1}},
		{{-0.25f, -0.25, -0.25f, 1}, {1,0,1,1}},
		{{-0.25f, -0.25, 0.25f, 1}, {1,1,0,1}}
	};
	numVerts = ARRAYSIZE(tri);
	//load it on the card
	D3D11_BUFFER_DESC bDesc;
	D3D11_SUBRESOURCE_DATA subData;
	ZeroMemory(&bDesc, sizeof(bDesc));
	ZeroMemory(&subData, sizeof(subData));

	bDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bDesc.ByteWidth = sizeof(MyVertex) * numVerts;
	bDesc.CPUAccessFlags = 0;
	bDesc.MiscFlags = 0;
	bDesc.StructureByteStride = 0;
	bDesc.Usage = D3D11_USAGE_DEFAULT;

	subData.pSysMem = tri; //points to the data you actually want to load in this case my triangle

	hr = myDev->CreateBuffer(&bDesc, &subData, &vBuff);
	//write, compile, and load our shaders

	myDev->CreateVertexShader(MyVShader, sizeof(MyVShader), nullptr, &vShader);
	myDev->CreatePixelShader(MyPShader, sizeof(MyPShader), nullptr, &pShader);
	//describe it to D3D11
	D3D11_INPUT_ELEMENT_DESC ieDesc[] =
	{
		{"POSITION", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 16, D3D11_INPUT_PER_VERTEX_DATA, 0}
	};
	
	hr = myDev->CreateInputLayout(ieDesc, 2, MyVShader, sizeof(MyVShader), &vLayout);
	
	// create constant buffer
	ZeroMemory(&bDesc, sizeof(bDesc));

	bDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	bDesc.ByteWidth = sizeof(WVP);
	bDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	bDesc.MiscFlags = 0;
	bDesc.StructureByteStride = 0;
	bDesc.Usage = D3D11_USAGE_DYNAMIC;

	hr = myDev->CreateBuffer(&bDesc, nullptr, &cBuff);

	// load our complex mesh onto the card

	bDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bDesc.ByteWidth = sizeof(StoneHenge_data);
	bDesc.CPUAccessFlags = 0;
	bDesc.MiscFlags = 0;
	bDesc.StructureByteStride = 0;
	bDesc.Usage = D3D11_USAGE_IMMUTABLE;

	subData.pSysMem = StoneHenge_data;
	hr = myDev->CreateBuffer(&bDesc, &subData, &vBuffMesh); // vertex buffer
	// index buffer mesh
	bDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	bDesc.ByteWidth = sizeof(StoneHenge_indicies) * 2;
	subData.pSysMem = StoneHenge_indicies;
	hr = myDev->CreateBuffer(&bDesc, &subData, &iBuffMesh);
	hr = myDev->CreateVertexShader(MyVMeshShader, sizeof(MyVMeshShader), nullptr, &vMeshShader);

	
	bDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bDesc.ByteWidth = sizeof(skyBox_data);
	bDesc.CPUAccessFlags = 0;
	bDesc.MiscFlags = 0;
	bDesc.StructureByteStride = 0;
	bDesc.Usage = D3D11_USAGE_IMMUTABLE;

	subData.pSysMem = skyBox_data;
	hr = myDev->CreateBuffer(&bDesc, &subData, &skyVBuff); // vertex buffer
	// index buffer mesh
	bDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	bDesc.ByteWidth = sizeof(skyBox_indicies);
	subData.pSysMem = skyBox_indicies;
	hr = myDev->CreateBuffer(&bDesc, &subData, &skyIBuff);
	hr = myDev->CreateVertexShader(MyVSkyShader, sizeof(MyVSkyShader), nullptr, &skyVShader);
	hr = myDev->CreatePixelShader(MyPSkyShader, sizeof(MyPSkyShader), nullptr, &skyPShader);
	hr = CreateDDSTextureFromFile(myDev, L"Assets/SkyboxOcean.dds", nullptr, &skySRV);
	// load our new mesh shader
	

	// make matching input layout for mesh vertex
	D3D11_INPUT_ELEMENT_DESC meshInputDesc[] =
	{
		{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"TEXCOORD", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 24, D3D11_INPUT_PER_VERTEX_DATA, 0}
	};

	hr = myDev->CreateInputLayout(meshInputDesc, 3, MyVMeshShader, sizeof(MyVMeshShader), &vMeshLayout);


	
	// create Z buffer & view
	D3D11_TEXTURE2D_DESC zDesc;
	ZeroMemory(&zDesc, sizeof(zDesc));
	zDesc.ArraySize = 1;
	zDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	zDesc.Width = swap.BufferDesc.Width;
	zDesc.Height = swap.BufferDesc.Height;
	zDesc.Format = DXGI_FORMAT_D32_FLOAT;
	zDesc.Usage = D3D11_USAGE_DEFAULT;
	zDesc.MipLevels = 1;
	zDesc.SampleDesc.Count = 1;
	
	hr = myDev->CreateTexture2D(&zDesc, nullptr, &zBuffer);
	hr = CreateDDSTextureFromFile(myDev, L"Assets/Stonehenge.dds", nullptr, &srv);
	
	CD3D11_SAMPLER_DESC sd = CD3D11_SAMPLER_DESC(CD3D11_DEFAULT());
	
	myDev->CreateSamplerState(&sd, &SS);
	myDev->CreateDepthStencilView(zBuffer, nullptr, &zBufferView);


	
	return TRUE;
}

//
//  FUNCTION: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  PURPOSE: Processes messages for the main window.
//
//  WM_COMMAND  - process the application menu
//  WM_PAINT    - Paint the main window
//  WM_DESTROY  - post a quit message and return
//
//
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_COMMAND:
	{
		int wmId = LOWORD(wParam);
		// Parse the menu selections:
		switch (wmId)
		{
		case IDM_ABOUT:
			DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
			break;
		case IDM_EXIT:
			DestroyWindow(hWnd);
			break;
		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
		}
	}
	break;
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}

// Message handler for about box.
INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	UNREFERENCED_PARAMETER(lParam);
	switch (message)
	{
	case WM_INITDIALOG:
		return (INT_PTR)TRUE;

	case WM_COMMAND:
		if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
		{
			EndDialog(hDlg, LOWORD(wParam));
			return (INT_PTR)TRUE;
		}
		break;
	}
	return (INT_PTR)FALSE;
}
