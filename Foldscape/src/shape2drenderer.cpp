#include "shape2drenderer.h"

namespace foldscape
{
	static const float SCALER_BIG_R = 20.0f;
	static const float SCALER_SMALL_R = 7.0f;
	static const float SCALER_LINE_THICKNESSES[2] = { 10.0f, 5.0f };

	struct ShaderData
	{
		mth::double2 center;
		mth::double2 offset;
		mth::double2 scale;
		mth::float4 fractalColor;
		mth::float2 resolution;
		float colorScale;
		unsigned iterationCount;
		float escapeRadius;
		unsigned isMandelbrot;
		unsigned startOnZ;
		unsigned padding;

		ShaderData() = default;
	};

	void Shape2DRenderer::CreateScreenResources()
	{
		auto device3D = m_graphics.Device3D();

		D3D11_TEXTURE2D_DESC textureDesc{};
		textureDesc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
		textureDesc.ArraySize = 1;
		textureDesc.MipLevels = 1;
		textureDesc.Usage = D3D11_USAGE_DEFAULT;
		textureDesc.SampleDesc.Count = 1;
		textureDesc.SampleDesc.Quality = 0;
		textureDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_UNORDERED_ACCESS | D3D11_BIND_RENDER_TARGET;
		textureDesc.Width = m_graphics.Width();
		textureDesc.Height = m_graphics.Height();
		ComPtr<ID3D11Texture2D> texture;
		ThrowIfFailed(device3D->CreateTexture2D(&textureDesc, nullptr, &texture));

		D3D11_UNORDERED_ACCESS_VIEW_DESC uavd{};
		uavd.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
		uavd.ViewDimension = D3D11_UAV_DIMENSION_TEXTURE2D;
		uavd.Texture2D.MipSlice = 0;
		ThrowIfFailed(device3D->CreateUnorderedAccessView(texture.Get(), &uavd, &m_shaderOutputImage));

		ComPtr<IDXGISurface2> dxgiSurface;
		ThrowIfFailed(texture.As(&dxgiSurface));
		auto bmpprop = D2D1::BitmapProperties(D2D1::PixelFormat(textureDesc.Format, D2D1_ALPHA_MODE_PREMULTIPLIED));
		ThrowIfFailed(m_graphics.Context2D()->CreateSharedBitmap(__uuidof(IDXGISurface2), dxgiSurface.Get(), &bmpprop, &m_outputImage2D));
	}

	void Shape2DRenderer::DrawScalerPoints() const
	{
		if (m_scalersActive > 0)
		{
			const D2D1_COLOR_F colors[2] = { D2D1::ColorF(0.0f, 0.0f, 0.0f), D2D1::ColorF(1.0f, 1.0f, 1.0f) };
			ID2D1DeviceContext6* context2D = m_graphics.Context2D();

			for (int i = 0; i < 2; i++)
			{
				m_brush->SetColor(colors[i]);
				const mth::vec2<int> scaler0Screen = CoordToScreen(m_scalerPoints[0]);
				const D2D1_POINT_2F p1 = D2D1::Point2F(static_cast<float>(scaler0Screen.x), static_cast<float>(scaler0Screen.y));
				context2D->DrawEllipse(D2D1::Ellipse(p1, SCALER_BIG_R, SCALER_BIG_R), m_brush.Get(), SCALER_LINE_THICKNESSES[i]);
				context2D->DrawEllipse(D2D1::Ellipse(p1, SCALER_SMALL_R, SCALER_SMALL_R), m_brush.Get(), SCALER_LINE_THICKNESSES[i] * 0.5f);
				if (m_scalersActive > 1)
				{
					const mth::vec2<int> scaler1Screen = CoordToScreen(m_scalerPoints[1]);
					const D2D1_POINT_2F p2 = D2D1::Point2F(static_cast<float>(scaler1Screen.x), static_cast<float>(scaler1Screen.y));
					context2D->DrawEllipse(D2D1::Ellipse(p2, SCALER_BIG_R, SCALER_BIG_R), m_brush.Get(), SCALER_LINE_THICKNESSES[i]);
					context2D->DrawEllipse(D2D1::Ellipse(p2, SCALER_SMALL_R, SCALER_SMALL_R), m_brush.Get(), SCALER_LINE_THICKNESSES[i] * 0.5f);

					const float dx = p2.x - p1.x;
					const float dy = p2.y - p1.y;
					const float offset = SCALER_BIG_R / std::sqrt(dx * dx + dy * dy);
					context2D->DrawLine(
						D2D1::Point2F(p1.x + dx * offset, p1.y + dy * offset),
						D2D1::Point2F(p2.x - dx * offset, p2.y - dy * offset),
						m_brush.Get(), SCALER_LINE_THICKNESSES[i]);
				}
			}
		}
	}

	Shape2DRenderer::Shape2DRenderer(const Graphics& graphics) :
		m_graphics(graphics),
		m_scalersActive{},
		m_grabbedScaler(-1)
	{
		ID3D11Device5* device3D = m_graphics.Device3D();
		ComPtr<ID3DBlob> shaderByteCode = LoadBinary(L"mandelbrot.cso");
		ThrowIfFailed(device3D->CreateComputeShader(shaderByteCode->GetBufferPointer(), shaderByteCode->GetBufferSize(), nullptr, &m_shader));

		D3D11_BUFFER_DESC bufferDesc{};
		bufferDesc.Usage = D3D11_USAGE_DYNAMIC;
		bufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
		bufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
		bufferDesc.ByteWidth = sizeof ShaderData;
		ThrowIfFailed(device3D->CreateBuffer(&bufferDesc, nullptr, &m_buffer));

		ThrowIfFailed(m_graphics.Context2D()->CreateSolidColorBrush(D2D1::ColorF(0), &m_brush));

		CreateScreenResources();

		m_center = mth::double2(-0.75, 0.0);
		m_offset = 0.0;
		m_scaler = mth::double2(1.5, 0.0);
		m_iterationCount = 500;
		m_escapeRadius = 4.0f;
		m_isMandelbrot = 1;
		m_startOnZ = 0;
		m_screenSize = mth::float2(static_cast<float>(m_graphics.Width()), static_cast<float>(m_graphics.Height()));
	}

	void Shape2DRenderer::Render() const
	{
		D3D11_MAPPED_SUBRESOURCE resource{};
		auto context = m_graphics.Context3D();

		m_graphics.BeginRender();

		if (SUCCEEDED(context->Map(m_buffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &resource)))
		{
			ShaderData& shaderData = *reinterpret_cast<ShaderData*>(resource.pData);
			shaderData.center = m_center;
			shaderData.offset = m_offset;
			shaderData.scale = m_scaler;
			shaderData.fractalColor = mth::float4(0.0f, 0.0f, 0.0f, 1.0f);
			shaderData.resolution = m_screenSize;
			shaderData.colorScale = 10.0f;
			shaderData.iterationCount = m_iterationCount;
			shaderData.escapeRadius = m_escapeRadius;
			shaderData.isMandelbrot = m_isMandelbrot;
			shaderData.startOnZ = m_startOnZ;
			shaderData.padding = 0;

			context->Unmap(m_buffer.Get(), 0);
		}

		context->CSSetShader(m_shader.Get(), nullptr, 0);
		context->CSSetConstantBuffers(0, 1, m_buffer.GetAddressOf());
		context->CSSetUnorderedAccessViews(0, 1, m_shaderOutputImage.GetAddressOf(), nullptr);
		context->Dispatch((m_graphics.Width() + 15) / 16, (m_graphics.Height() + 15) / 16, 1);

		m_graphics.Context2D()->DrawBitmap(m_outputImage2D.Get());

		DrawScalerPoints();

		m_graphics.EndRender();
	}

	void Shape2DRenderer::OnMouseMove(int x, int y, int dx, int dy, InputFlags flags)
	{
		if (flags.leftButton)
		{
			MovePlane(mth::vec2<int>(x, y), m_moveGrabbedPoint);
			m_graphics.RequestRedraw();
		}
		if (flags.rightButton)
		{
			MoveScalerPoint(mth::vec2<int>(x, y) + m_scalerGrabOffset, m_grabbedScaler, m_scalersActive == 2);
			m_graphics.RequestRedraw();
		}
	}

	void Shape2DRenderer::OnMouseWheel(int x, int y, int delta, InputFlags flags)
	{
		Zoom(ScreenToCoord(mth::vec2<int>(x, y)), delta < 0 ? 1.1 : 1.0 / 1.1);
		m_graphics.RequestRedraw();
	}

	void Shape2DRenderer::OnMouseButtonDown(int x, int y, MouseButtonType button, InputFlags flags)
	{
		const mth::vec2<int> cursor(x, y);
		const mth::double2 coord = ScreenToCoord(cursor);

		switch (button)
		{
		case MouseButtonType::Left:
			m_moveGrabbedPoint = coord;
			break;
		case MouseButtonType::Right:
			switch (m_scalersActive)
			{
			case 0:
				m_scalerPoints[0] = coord;
				m_scalersActive = 1;
				m_graphics.RequestRedraw();
				break;
			case 1:
				m_scalerGrabOffset = CoordToScreen(m_scalerPoints[0]) - cursor;
				if (static_cast<float>(m_scalerGrabOffset.LengthSquare()) <= SCALER_BIG_R * SCALER_BIG_R)
				{
					m_grabbedScaler = 0;
				}
				else
				{
					m_scalerPoints[1] = coord;
					m_scalersActive = 2;
					m_graphics.RequestRedraw();
				}
				break;
			default:
				m_scalerGrabOffset = CoordToScreen(m_scalerPoints[0]) - cursor;
				if (static_cast<float>(m_scalerGrabOffset.LengthSquare()) <= SCALER_BIG_R * SCALER_BIG_R)
				{
					m_grabbedScaler = 0;
				}
				else
				{
					m_scalerGrabOffset = CoordToScreen(m_scalerPoints[1]) - cursor;
					if (static_cast<float>(m_scalerGrabOffset.LengthSquare()) <= SCALER_BIG_R * SCALER_BIG_R)
					{
						m_grabbedScaler = 1;
					}
					else
					{
						m_grabbedScaler = -1;
						m_scalersActive = 0;
						m_graphics.RequestRedraw();
					}
				}
				break;
			}
			break;
		}
	}

	void Shape2DRenderer::OnMouseButtonUp(int x, int y, MouseButtonType button, InputFlags flags)
	{
		switch (button)
		{
		case MouseButtonType::Right:
			m_grabbedScaler = -1;
			break;
		}
	}

	void Shape2DRenderer::OnResize()
	{
		m_screenSize = mth::float2(static_cast<float>(m_graphics.Width()), static_cast<float>(m_graphics.Height()));
		CreateScreenResources();
	}
}