#pragma once

#include "common.h"

namespace foldscape
{
	class Textbox
	{
		HWND m_textbox;
		LRESULT(*m_origMsgHandler)(HWND, UINT, WPARAM, LPARAM);
		std::function<void(const wchar_t*)> m_onAction;

	public:
		Textbox();
		~Textbox();
		void Init(HWND parent, int x, int y, int w, int h);
		void AssignOnAction(std::function<void(const wchar_t*)> onAction);

		LRESULT MessageHandler(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam);
	};
}