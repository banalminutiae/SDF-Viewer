#pragma comment(lib, "user32.lib")
#pragma comment(lib, "gdi32.lib")
#pragma comment(lib, "d3d11")
#pragma comment(lib, "d3dcompiler")

#define WIN32_LEAN_AND_MEAN

#include <windows.h>					
#include <d3d11.h>
#include <d3dcompiler.h>

LRESULT CALLBACK window_proc(HWND window, UINT message, WPARAM w_param, LPARAM l_param)
{
    switch (message)
    {
        case WM_DESTROY:
            PostQuitMessage(0);
            return 0;
    }

    return DefWindowProcA(window, message, w_param, l_param);
}

BOOL init_app(HINSTANCE instance) {
	WNDCLASS wc = {0};
	wc.style = CS_OWNDC | CS_HREDRAW | CS_VREDRAW;
	wc.lpfnWndProc = window_proc;
	wc.hInstance = instance;
	wc.hCursor = LoadCursor (NULL, IDC_ARROW);
	wc.lpszClassName = "WindowClass";

	return RegisterClass(&wc);	
}

int CALLBACK WinMain(HINSTANCE instance, HINSTANCE prev_instance, LPSTR cmd_line, int show_code) {
	if (!init_app(instance)) {
		return FALSE;
	}

	HWND window_handle = CreateWindowExA(
        0,
        "WindowClass",
        "app",
        WS_OVERLAPPEDWINDOW | WS_VISIBLE,
        CW_USEDEFAULT,
        CW_USEDEFAULT,
        CW_USEDEFAULT,
        CW_USEDEFAULT,
        0,
        0,
        instance,
        0
    );

	if (window_handle) {
		ShowWindow(window_handle, show_code);
	}

	
	//////////////////////////////////////////////////////////////////////////

	D3D_FEATURE_LEVEL featurelevels[] = { D3D_FEATURE_LEVEL_11_0 };
	
	DXGI_SWAP_CHAIN_DESC swapchaindesc = {0};
	swapchaindesc.BufferDesc.RefreshRate.Numerator   = 0;
	swapchaindesc.BufferDesc.RefreshRate.Denominator = 1;	
    swapchaindesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    swapchaindesc.SampleDesc.Count  = 1;
    swapchaindesc.BufferUsage       = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    swapchaindesc.BufferCount       = 2;
    swapchaindesc.OutputWindow      = window_handle;
    swapchaindesc.Windowed          = TRUE;
    swapchaindesc.SwapEffect        = DXGI_SWAP_EFFECT_FLIP_DISCARD;

	IDXGISwapChain *swapchain;
	
	ID3D11Device *device;
    ID3D11DeviceContext *devicecontext;

	HRESULT hr = D3D11CreateDeviceAndSwapChain(nullptr, D3D_DRIVER_TYPE_HARDWARE, nullptr, D3D11_CREATE_DEVICE_BGRA_SUPPORT, featurelevels, ARRAYSIZE(featurelevels), D3D11_SDK_VERSION, &swapchaindesc, &swapchain, &device, nullptr, &devicecontext);

	swapchain->GetDesc(&swapchaindesc);

	ID3D11RenderTargetView* render_target_view;
	ID3D11Texture2D *framebuffer;

	swapchain->GetBuffer(0, __uuidof(ID3D11Texture2D), (void**)&framebuffer);

	device->CreateRenderTargetView(framebuffer, nullptr, &render_target_view);

	UINT flags = D3DCOMPILE_ENABLE_STRICTNESS | D3DCOMPILE_DEBUG;

	//////////////////////////////////////////////////////////////////////////

	ID3DBlob *vs_blob = NULL, *ps_blob = NULL, *err_blob = NULL;
	HRESULT vs_hr = D3DCompileFromFile(
        L"shaders.hlsl",
        nullptr,
        D3D_COMPILE_STANDARD_FILE_INCLUDE,
        "vs_main",
        "vs_5_0",
        flags,
        0,
        &vs_blob,
        &err_blob
    );

	if (FAILED(vs_hr)) {
		if (err_blob) OutputDebugStringA((char*)err_blob->GetBufferPointer());
		if (vs_blob) vs_blob->Release();
	}
	
	HRESULT ps_hr = D3DCompileFromFile(
        L"shaders.hlsl",
        nullptr,
        D3D_COMPILE_STANDARD_FILE_INCLUDE,
        "ps_main",
        "ps_5_0",
        flags,
        0,
        &ps_blob,
        &err_blob
    );

	if (FAILED(ps_hr)) {
		if (err_blob) OutputDebugStringA((char*)err_blob->GetBufferPointer());
		if (ps_blob) ps_blob->Release();
	}

	ID3D11VertexShader *vertex_shader = NULL;
	ID3D11PixelShader *pixel_shader = NULL;

	device->CreateVertexShader(vs_blob->GetBufferPointer(), vs_blob->GetBufferSize(), NULL, &vertex_shader);
	device->CreatePixelShader(ps_blob->GetBufferPointer(), ps_blob->GetBufferSize(), NULL, &pixel_shader);

	ID3D11InputLayout* input_layout = NULL;
	D3D11_INPUT_ELEMENT_DESC inputElementDesc[] = {
		{ "POS", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		/*
		  { "COL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		  { "NOR", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		  { "TEX", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		*/
	};

	device->CreateInputLayout(inputElementDesc, ARRAYSIZE(inputElementDesc), vs_blob->GetBufferPointer(), vs_blob->GetBufferSize(), &input_layout);
	
	//////////////////////////////////////////////////////////////////////////

	MSG message;
	for (;;) {
		BOOL result = GetMessage(&message, 0, 0, 0);
		if (result <= 0) {
			break;
		}
		TranslateMessage(&message);
		DispatchMessage(&message);
	}
	if (FAILED(hr)) {
		if (err_blob) OutputDebugStringA((char*)err_blob->GetBufferPointer());
		if (ps_blob) vs_blob->Release();
	}
	return 0;
}
