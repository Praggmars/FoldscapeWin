#include "navigator2d.h"

namespace foldscape
{
	static mth::float2 ScreenToUv(mth::vec2<int> point, mth::float2 screenSize)
	{
		const mth::float2 screenSizePrime = (screenSize - 1.0f) * 0.5f;
		return (point.WithType<float>() - screenSizePrime) / screenSizePrime.Min();
	}

	static mth::vec2<int> UvToScreen(mth::float2 uv, mth::float2 screenSize)
	{
		const mth::float2 screenSizePrime = (screenSize - 1.0f) * 0.5f;
		return (uv * screenSizePrime.Min() + screenSizePrime).WithType<int>();
	}

	static mth::float2 CoordToUv(mth::double2 coord, mth::double2 center, mth::double2 scaler)
	{
		return mth::CpxDiv(coord - center, scaler).WithType<float>();
	}

	static mth::double2 UvToCoord(mth::float2 uv, mth::double2 center, mth::double2 scaler)
	{
		return mth::CpxMul(uv.WithType<double>(), scaler) + center;
	}

	mth::vec2<int> Navigator2D::CoordToScreen(mth::double2 coord) const
	{
		return UvToScreen(CoordToUv(coord, m_center, m_scaler), m_screenSize);
	}

	mth::double2 Navigator2D::ScreenToCoord(mth::vec2<int> screen) const
	{
		return UvToCoord(ScreenToUv(screen, m_screenSize), m_center, m_scaler);
	}

	void Navigator2D::MoveScalerPoint(mth::vec2<int> screen, int pointIndex, bool transformPlane)
	{
		if (!(pointIndex >> 1))
		{
			if (transformPlane)
			{
				mth::float2 uv[2];
				uv[pointIndex] = ScreenToUv(screen, m_screenSize);
				uv[1 - pointIndex] = CoordToUv(m_scalerPoints[1 - pointIndex], m_center, m_scaler);

				m_scaler = mth::CpxDiv(m_scalerPoints[1] - m_scalerPoints[0], (uv[1] - uv[0]).WithType<double>());
				m_center = m_scalerPoints[0] - mth::CpxMul(uv[0].WithType<double>(), m_scaler);
			}
			else
			{
				m_scalerPoints[pointIndex] = ScreenToCoord(screen);
			}
		}
	}

	void Navigator2D::MovePlane(mth::vec2<int> screen, mth::double2 grabbedCoord)
	{
		m_center = grabbedCoord - mth::CpxMul(ScreenToUv(screen, m_screenSize).WithType<double>(), m_scaler);
	}

	void Navigator2D::Zoom(mth::double2 zoomCenter, double zoomFactor)
	{
		m_center = (m_center - zoomCenter) * zoomFactor + zoomCenter;
		m_scaler *= zoomFactor;
	}
}