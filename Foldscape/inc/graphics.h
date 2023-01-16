#pragma once

#include "common.h"

namespace foldscape
{
	class Graphics
	{
		ComPtr<ID2D1Device6> m_device2D;
		ComPtr<ID2D1DeviceContext6> m_context2D;	
		ComPtr<ID3D11Device5> m_device3D;
		ComPtr<ID3D11DeviceContext4> m_context3D;
		ComPtr<IDXGISwapChain4> m_swapchain;
		ComPtr<ID2D1Bitmap1> m_targetBitmap;
		ComPtr<IDWriteFactory7> m_writeFactory;
		HWND m_targetWindow;
		int m_width;
		int m_height;

	private:
		void CreateDevices();
		void CreateScreenResources(HWND target);
		void Make2DTarget();

	public:
		Graphics();

		void Init(HWND hwnd);
		void Resize(int width, int height);

		void RequestRedraw() const;

		void BeginRender() const;
		void EndRender() const;

		inline ID3D11Device5* Device3D() const { return m_device3D.Get(); }
		inline ID3D11DeviceContext4* Context3D() const { return m_context3D.Get(); }
		inline ID2D1Device6* Device2D() const { return m_device2D.Get(); }
		inline ID2D1DeviceContext6* Context2D() const { return m_context2D.Get(); }
		inline IDWriteFactory7* WriteFactory() const { return m_writeFactory.Get(); }
		inline int Width() const { return m_width; }
		inline int Height() const { return m_height; }
	};
}