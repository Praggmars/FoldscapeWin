#include "ui/shaderimage.h"

namespace foldscape
{
	void ShaderRenderer::OnMouseMove(int x, int y, int dx, int dy, InputFlags flags) {}
	void ShaderRenderer::OnMouseWheel(int x, int y, int delta, InputFlags flags) {}
	void ShaderRenderer::OnMouseButtonDown(int x, int y, MouseButtonType button, InputFlags flags) {}
	void ShaderRenderer::OnMouseButtonUp(int x, int y, MouseButtonType button, InputFlags flags) {}
	void ShaderRenderer::OnResize() {}

	void ShaderImage::OnPaint()
	{
		ValidateRect(m_canvas, nullptr);
		if (m_renderer)
			m_renderer->Render();
	}

	void ShaderImage::OnMouseMove(int x, int y, InputFlags flags)
	{
		if (m_renderer)
			m_renderer->OnMouseMove(x, y, x - m_prevCursor.x, y - m_prevCursor.y, flags);
		m_prevCursor = POINT{ x, y };
	}

	void ShaderImage::OnMouseWheel(int x, int y, int delta, InputFlags flags)
	{
		if (m_renderer)
			m_renderer->OnMouseWheel(x, y, delta, flags);
	}

	void ShaderImage::OnMouseButtonDown(int x, int y, MouseButtonType button, InputFlags flags)
	{
		SetCapture(m_canvas);
		m_prevCursor = POINT{ x, y };

		if (m_renderer)
			m_renderer->OnMouseButtonDown(x, y, button, flags);
	}

	void ShaderImage::OnMouseButtonUp(int x, int y, MouseButtonType button, InputFlags flags)
	{
		if (!flags.flags)
			ReleaseCapture();
		m_prevCursor = POINT{ x, y };

		if (m_renderer)
			m_renderer->OnMouseButtonUp(x, y, button, flags);
	}

	void ShaderImage::OnResize(int width, int height)
	{
		m_graphics.Resize(width, height);

		if (m_renderer)
			m_renderer->OnResize();
	}

	ShaderImage::ShaderImage() :
		m_prevCursor{},
		m_renderer{} {}

	ShaderImage::~ShaderImage()
	{
		SafeDestroyWindow(m_canvas);
	}

	void ShaderImage::Init(HWND parent, int x, int y, int w, int h)
	{
		WNDCLASSEX wc{};
		wc.cbSize = sizeof wc;
		wc.hInstance = GetModuleHandle(nullptr);
		wc.lpszClassName = L"Canvas";
		wc.hCursor = LoadCursor(nullptr, IDC_ARROW);
		wc.style = CS_HREDRAW | CS_VREDRAW;
		wc.lpfnWndProc = DefWindowProc;
		RegisterClassEx(&wc);

		m_canvas = CreateWindowEx(0, wc.lpszClassName, wc.lpszClassName, WS_CHILD | WS_VISIBLE,
			x, y, w, h, parent, nullptr, wc.hInstance, nullptr);

		m_graphics.Init(m_canvas);
		OnResize(w, h);

		SetWindowLongPtr(m_canvas, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(this));
		SetWindowLongPtr(m_canvas, GWLP_WNDPROC, reinterpret_cast<LONG_PTR>(static_cast<LRESULT(*)(HWND, UINT, WPARAM, LPARAM)>(
			[](HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)->LRESULT {
				return reinterpret_cast<ShaderImage*>(GetWindowLongPtr(hwnd, GWLP_USERDATA))->MessageHandler(hwnd, msg, wparam, lparam);
			})));
	}

	LRESULT ShaderImage::MessageHandler(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
	{
		switch (msg)
		{
		case WM_PAINT:
			OnPaint();
			return 0;
		case WM_MOUSEMOVE:
			OnMouseMove(GET_X_LPARAM(lparam), GET_Y_LPARAM(lparam), InputFlags(wparam));
			return 0;
		case WM_MOUSEWHEEL:
			OnMouseWheel(m_prevCursor.x, m_prevCursor.y, GET_WHEEL_DELTA_WPARAM(wparam), InputFlags(wparam));
			return 0;
		case WM_LBUTTONDOWN:
			OnMouseButtonDown(GET_X_LPARAM(lparam), GET_Y_LPARAM(lparam), MouseButtonType::Left, InputFlags(wparam));
			return 0;
		case WM_MBUTTONDOWN:
			OnMouseButtonDown(GET_X_LPARAM(lparam), GET_Y_LPARAM(lparam), MouseButtonType::Middle, InputFlags(wparam));
			return 0;
		case WM_RBUTTONDOWN:
			OnMouseButtonDown(GET_X_LPARAM(lparam), GET_Y_LPARAM(lparam), MouseButtonType::Right, InputFlags(wparam));
			return 0;
		case WM_XBUTTONDOWN:
			OnMouseButtonDown(GET_X_LPARAM(lparam), GET_Y_LPARAM(lparam), GET_XBUTTON_WPARAM(wparam) == 1 ? MouseButtonType::XButton1 : MouseButtonType::XButton2, InputFlags(wparam));
			return 0;
		case WM_LBUTTONUP:
			OnMouseButtonUp(GET_X_LPARAM(lparam), GET_Y_LPARAM(lparam), MouseButtonType::Left, InputFlags(wparam));
			return 0;
		case WM_MBUTTONUP:
			OnMouseButtonUp(GET_X_LPARAM(lparam), GET_Y_LPARAM(lparam), MouseButtonType::Middle, InputFlags(wparam));
			return 0;
		case WM_RBUTTONUP:
			OnMouseButtonUp(GET_X_LPARAM(lparam), GET_Y_LPARAM(lparam), MouseButtonType::Right, InputFlags(wparam));
			return 0;
		case WM_XBUTTONUP:
			OnMouseButtonUp(GET_X_LPARAM(lparam), GET_Y_LPARAM(lparam), GET_XBUTTON_WPARAM(wparam) == 1 ? MouseButtonType::XButton1 : MouseButtonType::XButton2, InputFlags(wparam));
			return 0;
		case WM_SIZE:
			OnResize(LOWORD(lparam), HIWORD(lparam));
			return 0;
		}
		return DefWindowProc(hwnd, msg, wparam, lparam);
	}
}