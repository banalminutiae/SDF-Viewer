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

	ID311RenderTargetView* render_target_view_ptr;
	ID3D11Texture2D *framebuffer;

	swapchain->GetBuffer(0, __uuidof(ID3D11Texture2D), (void**)&framebuffer);

	device->CreateRenderTargetView(framebuffer, nullptr, &render_target_view_tr);

	MSG message;
	for (;;) {
		BOOL result = GetMessage(&message, 0, 0, 0);
		if (result <= 0) {
			break;
		}
		TranslateMessage(&message);
		DispatchMessage(&message);
	}
	
	return 0;
}
