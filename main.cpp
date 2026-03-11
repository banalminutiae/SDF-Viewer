#pragma comment(lib, "user32.lib")
#pragma comment(lib, "gdi32.lib")
#pragma comment(lib, "d3d11")
#pragma comment(lib, "d3dcompiler")

#define WIN32_LEAN_AND_MEAN

#include <windows.h>					
#include <d3d11.h>
#include <d3dcompiler.h>

#include "imgui/imgui.h"
#include "imgui/imgui_impl_win32.h"
#include "imgui/imgui_impl_dx11.h"

#include "file_watcher.cpp"

ID3D11Device* device;
ID3D11DeviceContext* devicecontext;

ID3D11VertexShader *vertex_shader = nullptr;
ID3D11PixelShader *pixel_shader = nullptr;

void compile_shaders();


/*
TODO:
- Real shader hot reloading by detecting file changes
- Dear ImGui integration

 */
LRESULT CALLBACK window_proc(HWND window, UINT message, WPARAM w_param, LPARAM l_param) {
	switch (message) {
	case WM_KEYDOWN:
		if (w_param == VK_ESCAPE) {
			PostQuitMessage(0);
			return 0;
		}
		if (w_param == VK_F5) {
			compile_shaders();
		}
		break;
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
	wc.hCursor = LoadCursor (nullptr, IDC_ARROW);
	wc.lpszClassName = "WindowClass";

	return RegisterClass(&wc);	
}

void compile_shaders() {
	UINT flags = D3DCOMPILE_ENABLE_STRICTNESS | D3DCOMPILE_DEBUG;
	
	ID3DBlob *vs_blob = nullptr, *ps_blob = nullptr, *err_blob = nullptr;

	devicecontext->VSSetShader(nullptr, nullptr, 0);
	devicecontext->PSSetShader(nullptr, nullptr, 0);

	HRESULT vs_hr = D3DCompileFromFile(L"shaders.hlsl", nullptr, D3D_COMPILE_STANDARD_FILE_INCLUDE, "vs_main", "vs_5_0", flags, 0, &vs_blob, &err_blob);
	HRESULT ps_hr = D3DCompileFromFile(L"shaders.hlsl", nullptr, D3D_COMPILE_STANDARD_FILE_INCLUDE, "ps_main", "ps_5_0", flags, 0, &ps_blob, &err_blob);

	if (FAILED(vs_hr)) {
		if (err_blob) OutputDebugStringA((char*)err_blob->GetBufferPointer());
		if (vs_blob) vs_blob->Release();
	}
	
	if (FAILED(ps_hr)) {
		if (err_blob) OutputDebugStringA((char*)err_blob->GetBufferPointer());
		if (ps_blob) ps_blob->Release();
	}

	device->CreateVertexShader(vs_blob->GetBufferPointer(), vs_blob->GetBufferSize(), nullptr, &vertex_shader);
	device->CreatePixelShader(ps_blob->GetBufferPointer(), ps_blob->GetBufferSize(), nullptr, &pixel_shader);

	devicecontext->VSSetShader(vertex_shader, nullptr, 0);
	devicecontext->PSSetShader(pixel_shader, nullptr, 0);
}

int CALLBACK WinMain(HINSTANCE instance, HINSTANCE prev_instance, LPSTR cmd_line, int show_code) {
	if (!init_app(instance)) {
		return 0;
	}

	HWND window_handle = CreateWindowExA(0, "WindowClass", "app", WS_OVERLAPPEDWINDOW | WS_VISIBLE, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, 0, 0, instance, 0);

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

	D3D11CreateDeviceAndSwapChain(nullptr, D3D_DRIVER_TYPE_HARDWARE, nullptr, D3D11_CREATE_DEVICE_BGRA_SUPPORT | D3D11_CREATE_DEVICE_DEBUG, featurelevels, ARRAYSIZE(featurelevels), D3D11_SDK_VERSION, &swapchaindesc, &swapchain, &device, nullptr, &devicecontext);

	swapchain->GetDesc(&swapchaindesc);

	ID3D11RenderTargetView* render_target_view;
	ID3D11Texture2D *framebuffer;

	swapchain->GetBuffer(0, __uuidof(ID3D11Texture2D), (void**)&framebuffer);

	device->CreateRenderTargetView(framebuffer, nullptr, &render_target_view);

	//////////////////////////////////////////////////////////////////////////

	UINT flags = D3DCOMPILE_ENABLE_STRICTNESS | D3DCOMPILE_DEBUG;

	ID3DBlob *vs_blob = nullptr, *ps_blob = nullptr, *err_blob = nullptr;
	HRESULT vs_hr = D3DCompileFromFile(L"shaders.hlsl", nullptr, D3D_COMPILE_STANDARD_FILE_INCLUDE, "vs_main", "vs_5_0", flags, 0, &vs_blob, &err_blob);

	if (FAILED(vs_hr)) {
		if (err_blob) OutputDebugStringA((char*)err_blob->GetBufferPointer());
		if (vs_blob) vs_blob->Release();
	}
	
	HRESULT ps_hr = D3DCompileFromFile(L"shaders.hlsl", nullptr, D3D_COMPILE_STANDARD_FILE_INCLUDE, "ps_main", "ps_5_0", flags, 0, &ps_blob, &err_blob);

	if (FAILED(ps_hr)) {
		if (err_blob) OutputDebugStringA((char*)err_blob->GetBufferPointer());
		if (ps_blob) ps_blob->Release();
	}

	device->CreateVertexShader(vs_blob->GetBufferPointer(), vs_blob->GetBufferSize(), nullptr, &vertex_shader);
	device->CreatePixelShader(ps_blob->GetBufferPointer(), ps_blob->GetBufferSize(), nullptr, &pixel_shader);

	//////////////////////////////////////////////////////////////////////////

	D3D11_RASTERIZER_DESC rasterizerdesc = { D3D11_FILL_SOLID, D3D11_CULL_NONE }; 

    ID3D11RasterizerState* rasterizerstate;

    device->CreateRasterizerState(&rasterizerdesc, &rasterizerstate);

	//////////////////////////////////////////////////////////////////////////
	
	D3D11_TEXTURE2D_DESC fb_desc;
	framebuffer->GetDesc(&fb_desc);

	float aspect_ratio = (float)fb_desc.Width / (float)fb_desc.Height;
	float constants[4] = { aspect_ratio, 0, 0, 0 };

	D3D11_BUFFER_DESC const_buf_desc= {0};
	const_buf_desc.ByteWidth = (sizeof(constants) + 15) & ~15u; // constant buffer size must be multiple of 16
    const_buf_desc.Usage     = D3D11_USAGE_IMMUTABLE; 
    const_buf_desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;

    D3D11_SUBRESOURCE_DATA const_buf_srd = {0};
	const_buf_srd.pSysMem = constants;

    ID3D11Buffer* const_buf;

    HRESULT cb_hr = device->CreateBuffer(&const_buf_desc, &const_buf_srd, &const_buf);

	if (FAILED(cb_hr)) {
		return 0;
	}

	MSG message;
	for (;;) {
		BOOL result = GetMessage(&message, 0, 0, 0);
		if (result <= 0) {
			vertex_shader->Release();
			pixel_shader->Release();
			devicecontext->ClearState();
			devicecontext->Flush();
			devicecontext->Release();
			devicecontext = nullptr;
			break;
		}
		TranslateMessage(&message);
		DispatchMessage(&message);

		D3D11_TEXTURE2D_DESC fb_desc;
		framebuffer->GetDesc(&fb_desc);

		D3D11_VIEWPORT viewport = { 0, 0, (float)fb_desc.Width, (float)fb_desc.Height, 0, 1 };
		devicecontext->RSSetViewports(1, &viewport);
		devicecontext->RSSetState(rasterizerstate);

		devicecontext->OMSetRenderTargets(1, &render_target_view, nullptr);

		devicecontext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		devicecontext->IASetVertexBuffers(0, 0, nullptr, nullptr, nullptr);
		
		devicecontext->PSSetConstantBuffers(0, 1, &const_buf);
		
		devicecontext->VSSetShader(vertex_shader, nullptr, 0);
		devicecontext->PSSetShader(pixel_shader, nullptr, 0);

		devicecontext->Draw(3, 0);

		swapchain->Present(1, 0);
	}
	return 0;
}
