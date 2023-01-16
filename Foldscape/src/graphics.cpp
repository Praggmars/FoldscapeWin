#include "graphics.h"

namespace foldscape
{
	void Graphics::CreateDevices()
	{
		ComPtr<ID3D11Device> d3dDevice;
		ComPtr<ID3D11DeviceContext> d3dContext;
		ComPtr<IDXGIDevice4> dxgiDevice;
		ComPtr<ID2D1Factory7> d2dFactory;
		const D3D_FEATURE_LEVEL featureLevels[] =
		{
			D3D_FEATURE_LEVEL_12_1, D3D_FEATURE_LEVEL_12_0,
			D3D_FEATURE_LEVEL_11_1, D3D_FEATURE_LEVEL_11_0
		};
		D3D_FEATURE_LEVEL featureLevel;

		ThrowIfFailed(D3D11CreateDevice(
			nullptr, D3D_DRIVER_TYPE_HARDWARE, nullptr, D3D11_CREATE_DEVICE_BGRA_SUPPORT,
			featureLevels, ARRAYSIZE(featureLevels), D3D11_SDK_VERSION,
			&d3dDevice, &featureLevel, &d3dContext));
		ThrowIfFailed(d3dDevice.As(&m_device3D));
		ThrowIfFailed(d3dContext.As(&m_context3D));

		ThrowIfFailed(m_device3D.As(&dxgiDevice));
		ThrowIfFailed(D2D1CreateFactory(D2D1_FACTORY_TYPE_MULTI_THREADED, IID_PPV_ARGS(&d2dFactory)));
		ThrowIfFailed(d2dFactory->CreateDevice(dxgiDevice.Get(), &m_device2D));
		ThrowIfFailed(m_device2D->CreateDeviceContext(D2D1_DEVICE_CONTEXT_OPTIONS_ENABLE_MULTITHREADED_OPTIMIZATIONS, &m_context2D));
	}
	void Graphics::CreateScreenResources(HWND target)
	{
		m_width = 1;
		m_height = 1;
		m_targetWindow = target;

		ComPtr<IDXGISwapChain> swapchain;
		DXGI_SWAP_CHAIN_DESC swapChainDesc{};
		swapChainDesc.BufferCount = 2;
		swapChainDesc.BufferDesc.Width = m_width;
		swapChainDesc.BufferDesc.Height = m_height;
		swapChainDesc.BufferDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
		swapChainDesc.BufferDesc.RefreshRate.Numerator = 0;
		swapChainDesc.BufferDesc.RefreshRate.Denominator = 1;
		swapChainDesc.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
		swapChainDesc.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
		swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
		swapChainDesc.OutputWindow = target;
		swapChainDesc.SampleDesc.Count = 1;
		swapChainDesc.SampleDesc.Quality = 0;
		swapChainDesc.Windowed = true;
		swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
		swapChainDesc.Flags = 0;

		ComPtr<IDXGIFactory5> factory;
		ThrowIfFailed(CreateDXGIFactory(IID_PPV_ARGS(&factory)));
		ThrowIfFailed(factory->CreateSwapChain(m_device3D.Get(), &swapChainDesc, &swapchain));
		ThrowIfFailed(swapchain.As(&m_swapchain));

		Make2DTarget();
	}
	void Graphics::Make2DTarget()
	{
		ComPtr<IDXGISurface2> backBuffer;

		ThrowIfFailed(m_swapchain->GetBuffer(0, IID_PPV_ARGS(&backBuffer)));
		ThrowIfFailed(m_context2D->CreateBitmapFromDxgiSurface(backBuffer.Get(),
			D2D1::BitmapProperties1(
				D2D1_BITMAP_OPTIONS_TARGET | D2D1_BITMAP_OPTIONS_CANNOT_DRAW,
				D2D1::PixelFormat(DXGI_FORMAT_B8G8R8A8_UNORM, D2D1_ALPHA_MODE_PREMULTIPLIED)),
			&m_targetBitmap));
		m_context2D->SetTarget(m_targetBitmap.Get());
	}
	Graphics::Graphics() :
		m_targetWindow{},
		m_width{},
		m_height{} {}
	void Graphics::Init(HWND target)
	{
		CreateDevices();
		CreateScreenResources(target);
		ThrowIfFailed(DWriteCreateFactory(DWRITE_FACTORY_TYPE_SHARED, __uuidof(&m_writeFactory), static_cast<IUnknown**>(&m_writeFactory)));
	}
	void Graphics::Resize(int width, int height)
	{
		m_width = std::max(1, width);
		m_height = std::max(1, height);
		m_context2D->SetTarget(nullptr);
		m_targetBitmap.Reset();
		ThrowIfFailed(m_swapchain->ResizeBuffers(2, m_width, m_height, DXGI_FORMAT_UNKNOWN, 0));
		Make2DTarget();
	}
	void Graphics::RequestRedraw() const
	{
		InvalidateRect(m_targetWindow, nullptr, false);
	}
	void Graphics::BeginRender() const
	{
		m_context2D->SetTarget(m_targetBitmap.Get());
		m_context2D->BeginDraw();
	}
	void Graphics::EndRender() const
	{
		m_context2D->EndDraw();
		m_swapchain->Present(1, 0);
	}
}