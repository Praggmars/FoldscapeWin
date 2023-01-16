#pragma once

#include "math/position.hpp"

namespace foldscape
{
	class Navigator2D
	{
	protected:
		mth::float2 m_screenSize;
		mth::double2 m_center;
		mth::double2 m_scaler;
		mth::double2 m_scalerPoints[2];

	protected:
		mth::vec2<int> CoordToScreen(mth::double2 coord) const;
		mth::double2 ScreenToCoord(mth::vec2<int> screen) const;

	public:
		void MoveScalerPoint(mth::vec2<int> screen, int pointIndex, bool transformPlane);
		void MovePlane(mth::vec2<int> screen, mth::double2 grabbedCoord);
		void Zoom(mth::double2 zoomCenter, double zoomFactor);
	};
}