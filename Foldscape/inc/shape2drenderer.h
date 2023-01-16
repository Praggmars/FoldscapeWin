#pragma once

#include "ui/shaderimage.h"
#include "navigator2d.h"

namespace foldscape
{
	class Shape2DRenderer : public ShaderRenderer, public Navigator2D
	{
		const Graphics& m_graphics;
		ComPtr<ID3D11ComputeShader> m_shader;
		ComPtr<ID3D11Buffer> m_buffer;
		ComPtr<ID3D11UnorderedAccessView> m_shaderOutputImage;
		ComPtr<ID2D1Bitmap> m_outputImage2D;

		ComPtr<ID2D1SolidColorBrush> m_brush;

		int m_scalersActive;
		int m_grabbedScaler;
		mth::vec2<int> m_scalerGrabOffset;
		mth::double2 m_moveGrabbedPoint;
		mth::double2 m_offset;
		unsigned m_iterationCount;
		float m_escapeRadius;
		unsigned m_isMandelbrot;
		unsigned m_startOnZ;

	private:
		void CreateScreenResources();
		void DrawScalerPoints() const;

	public:
		Shape2DRenderer(const Graphics& graphics);

		virtual void Render() const override;
		virtual void OnMouseMove(int x, int y, int dx, int dy, InputFlags flags);
		virtual void OnMouseWheel(int x, int y, int delta, InputFlags flags);
		virtual void OnMouseButtonDown(int x, int y, MouseButtonType button, InputFlags flags);
		virtual void OnMouseButtonUp(int x, int y, MouseButtonType button, InputFlags flags);
		virtual void OnResize();
	};
}