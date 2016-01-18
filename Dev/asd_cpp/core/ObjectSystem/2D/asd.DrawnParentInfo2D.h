﻿#pragma once
#include "asd.ParentInfo2D.h"
#include "../asd.ChildManagementMode.h"
#include "../asd.ChildTransformingMode.h"
#include "../asd.ChildDrawingMode.h"
#include "asd.CoreDrawnObject2D.h"

namespace asd
{
	class DrawnParentInfo2D : public ParentInfo2D
	{
	protected:
		CoreDrawnObject2D* m_drawnParent;
		ChildDrawingMode::Flags m_drawingMode;

	public:
		DrawnParentInfo2D(CoreDrawnObject2D* parent,
			ChildManagementMode::Flags managementMode,
			ChildTransformingMode transformingMode,
			ChildDrawingMode::Flags drawingMode)
			: ParentInfo2D(parent, managementMode, transformingMode)
			, m_drawingMode(drawingMode)
			, m_drawnParent(parent)
		{
		}

		Color GetInheritedColor() const
		{
			return ((m_drawingMode & ChildDrawingMode::Color) != 0)
				? m_drawnParent->GetAbsoluteColor()
				: Color(255, 255, 255, 255);
		}

		int GetInheritedDrawingPriority() const
		{
			return ((m_drawingMode & ChildDrawingMode::DrawingPriority) != 0)
				? m_drawnParent->GetAbsoluteDrawingPriority()
				: 0;
		}
	};
}