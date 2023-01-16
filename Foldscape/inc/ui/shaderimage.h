#pragma once

#include "graphics.h"

namespace foldscape
{
	class ShaderRenderer
	{
	public:
		virtual void Render() const = 0;
		virtual void OnMouseMove(int x, int y, int dx, int dy, InputFlags flags);
		virtual void OnMouseWheel(int x, int y, int delta, InputFlags flags);
		virtual void OnMouseButtonDown(int x, int y, MouseButtonType button, InputFlags flags);
		virtual void OnMouseButtonUp(int x, int y, MouseButtonType button, InputFlags flags);
		virtual void OnResize();
	};

	class ShaderImage
	{
		HWND m_canvas;

		Graphics m_graphics;

		POINT m_prevCursor;
		ShaderRenderer* m_renderer;

	private:
		void OnPaint();
		void OnMouseMove(int x, int y, InputFlags flags);
		void OnMouseWheel(int x, int y, int delta, InputFlags flags);
		void OnMouseButtonDown(int x, int y, MouseButtonType button, InputFlags flags);
		void OnMouseButtonUp(int x, int y, MouseButtonType button, InputFlags flags);
		void OnResize(int width, int height);

	public:
		ShaderImage();
		~ShaderImage();
		void Init(HWND parent, int x, int y, int w, int h);

		LRESULT MessageHandler(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam);

		inline void SetRenderer(ShaderRenderer* renderer) { m_renderer = renderer; }

		inline HWND WindowHandle() const { return m_canvas; }
		inline const Graphics& Graphics() const { return m_graphics; }
	};
}