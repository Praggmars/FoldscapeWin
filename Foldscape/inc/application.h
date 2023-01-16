#pragma once

#include "ui/shaderimage.h"
#include "ui/textbox.h"

namespace foldscape
{
	class Application
	{
		HWND m_mainWindow;
		std::wstring m_title;
		ShaderImage m_shaderImage;
		std::unique_ptr<ShaderRenderer> m_shaderRenderer;
		Textbox m_tbfunction;
		HWND m_cbShapeSelector;
		HWND m_slider;
		HBRUSH m_backgroundBrush;
		int m_border;
		int m_menuWidth;

	private:
		void CreateMainWindow(const wchar_t* title, int width, int height);
		void CreateUI(int width, int height);

		void OnResize(int width, int height);

	public:
		Application();
		~Application();

		void Init(const wchar_t* title, int width, int height);
		void Run();
		LRESULT MessageHandler(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam);
	};
}