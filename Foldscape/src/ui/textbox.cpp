#include "ui/textbox.h"

namespace foldscape
{
	Textbox::Textbox() :
		m_textbox{},
		m_origMsgHandler{} {}

	Textbox::~Textbox()
	{
		SafeDestroyWindow(m_textbox);
	}

	void Textbox::Init(HWND parent, int x, int y, int w, int h)
	{
		m_textbox = CreateWindowEx(0, WC_EDIT, L"", WS_BORDER | WS_CHILD | WS_VISIBLE | ES_LEFT | ES_AUTOHSCROLL, x, y, w, h, parent, static_cast<HMENU>(nullptr), nullptr, nullptr);
		
		m_origMsgHandler = reinterpret_cast<decltype(m_origMsgHandler)>(GetWindowLongPtr(m_textbox, GWLP_WNDPROC));
		SetWindowLongPtr(m_textbox, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(this));
		SetWindowLongPtr(m_textbox, GWLP_WNDPROC, reinterpret_cast<LONG_PTR>(static_cast<LRESULT(*)(HWND, UINT, WPARAM, LPARAM)>(
			[](HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)->LRESULT {
				return reinterpret_cast<Textbox*>(GetWindowLongPtr(hwnd, GWLP_USERDATA))->MessageHandler(hwnd, msg, wparam, lparam);
			})));
	}

	void Textbox::AssignOnAction(std::function<void(const wchar_t*)> onAction)
	{
		m_onAction = onAction;
	}

	LRESULT Textbox::MessageHandler(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
	{
		if (msg == WM_CHAR && wparam == VK_RETURN)
		{
			if (m_onAction)
			{
				const int length = GetWindowTextLength(m_textbox);
				wchar_t* text = new wchar_t[length + 1];
				text[length] = L'\0';
				GetWindowText(m_textbox, text, length);
				m_onAction(text);
				delete[] text;
			}
			return 0;
		}
		return m_origMsgHandler(hwnd, msg, wparam, lparam);
	}
}