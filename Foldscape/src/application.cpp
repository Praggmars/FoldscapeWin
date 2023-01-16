#include "application.h"
#include "shape2drenderer.h"

namespace foldscape
{
	enum class ActionItemIDs
	{
		FunctionInput
	};

	enum class ShapeSelector
	{
		Shape2D,
		Cloud2D,
		Rotate4D
	};

	void Application::CreateMainWindow(const wchar_t* title, int width, int height)
	{
		m_title = title;
		m_backgroundBrush = CreateSolidBrush(RGB(51, 51, 51));

		WNDCLASSEX wc{};
		wc.cbSize = sizeof wc;
		wc.hInstance = GetModuleHandle(nullptr);
		wc.lpszClassName = m_title.c_str();
		wc.hCursor = LoadCursor(nullptr, IDC_ARROW);
		wc.style = CS_HREDRAW | CS_VREDRAW;
		wc.lpfnWndProc = DefWindowProc;
		wc.hbrBackground = m_backgroundBrush;
		RegisterClassEx(&wc);

		RECT rect{ 0, 0, static_cast<LONG>(width), static_cast<LONG>(height) };
		const DWORD style = WS_OVERLAPPEDWINDOW;
		const DWORD exStyle = 0;
		AdjustWindowRectEx(&rect, style, false, exStyle);
		m_mainWindow = CreateWindowEx(exStyle, wc.lpszClassName, wc.lpszClassName, style,
			CW_USEDEFAULT, CW_USEDEFAULT, rect.right - rect.left, rect.bottom - rect.top,
			nullptr, nullptr, nullptr, nullptr);

		SetWindowLongPtr(m_mainWindow, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(this));
		SetWindowLongPtr(m_mainWindow, GWLP_WNDPROC, reinterpret_cast<LONG_PTR>(static_cast<LRESULT(*)(HWND, UINT, WPARAM, LPARAM)>(
			[](HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)->LRESULT {
				return reinterpret_cast<Application*>(GetWindowLongPtr(hwnd, GWLP_USERDATA))->MessageHandler(hwnd, msg, wparam, lparam);
			})));
	}

	void Application::CreateUI(int width, int height)
	{
		int y = m_border;
		int ctrlHeight = 24;

		m_tbfunction.Init(m_mainWindow, m_border, y, m_menuWidth - 2 * m_border, ctrlHeight);
		m_tbfunction.AssignOnAction([](const wchar_t* text) {
			OutputDebugString(text);
			});

		y += ctrlHeight + m_border;
		m_cbShapeSelector = CreateWindowEx(WS_EX_STATICEDGE, WC_COMBOBOX, nullptr, WS_CHILD | WS_VISIBLE | CBS_DROPDOWN | CBS_HASSTRINGS, m_border, y, m_menuWidth - 2 * m_border, ctrlHeight + 100, m_mainWindow, nullptr, nullptr, nullptr);
		SendMessage(m_cbShapeSelector, CB_ADDSTRING, 0, reinterpret_cast<LPARAM>(L"2D shape"));
		SendMessage(m_cbShapeSelector, CB_ADDSTRING, 0, reinterpret_cast<LPARAM>(L"2D cloud"));
		SendMessage(m_cbShapeSelector, CB_ADDSTRING, 0, reinterpret_cast<LPARAM>(L"4D rotate"));
		SendMessage(m_cbShapeSelector, CB_SETCURSEL, 0, 0);
		y += ctrlHeight + m_border;

		m_slider = CreateWindowEx(0, TRACKBAR_CLASS, L"Slider", WS_CHILD | WS_VISIBLE, m_border, y, m_menuWidth - 2 * m_border, ctrlHeight, m_mainWindow, nullptr, nullptr, nullptr);
		y += ctrlHeight + m_border;

		m_shaderImage.Init(m_mainWindow, m_menuWidth, 0, width - m_menuWidth, height);
		m_shaderRenderer = std::make_unique<Shape2DRenderer>(m_shaderImage.Graphics());
		m_shaderImage.SetRenderer(m_shaderRenderer.get());
	}

	void Application::OnResize(int width, int height)
	{
		SetWindowPos(m_shaderImage.WindowHandle(), nullptr, m_menuWidth, 0, width - m_menuWidth, height, SWP_NOREDRAW | SWP_DEFERERASE);
	}

	Application::Application() :
		m_mainWindow{},
		m_cbShapeSelector{},
		m_slider{},
		m_backgroundBrush{},
		m_menuWidth{},
		m_border{} {}

	Application::~Application()
	{
		SafeDestroyWindow(m_cbShapeSelector);

		if (m_backgroundBrush)
			DeleteObject(m_backgroundBrush);
	}

	void Application::Init(const wchar_t* title, int width, int height)
	{
		m_border = 10;
		m_menuWidth = 300;

		CreateMainWindow(title, width, height);
		CreateUI(width, height); 

		ShowWindow(m_mainWindow, SW_SHOWDEFAULT);
		UpdateWindow(m_mainWindow);
	}

	void Application::Run()
	{
		MSG message{};
		while (GetMessage(&message, nullptr, 0, 0))
		{
			TranslateMessage(&message);
			DispatchMessage(&message);
		}
	}

	LRESULT Application::MessageHandler(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
	{
		switch (msg)
		{
		case WM_SIZE:
			OnResize(LOWORD(lparam), HIWORD(lparam));
			return 0;
		case WM_DESTROY:
			PostQuitMessage(0);
			return 0;
		}
		return DefWindowProc(hwnd, msg, wparam, lparam);
	}
}
