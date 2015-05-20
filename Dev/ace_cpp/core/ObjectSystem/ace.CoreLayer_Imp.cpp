﻿
#include "ace.CoreLayer_Imp.h"

#include "PostEffect/ace.CorePostEffect_Imp.h"

#include "../Graphics/ace.Graphics_Imp.h"
#include "../Graphics/2D/ace.LayerRenderer.h"
#include <Utility/ace.Timer.h>

namespace ace
{

	CoreLayer_Imp::CoreLayer_Imp(Graphics* graphics, Vector2DI windowSize)
		: m_graphics(nullptr)
		, m_isDrawn(true)
		, m_drawingPriority(0)
		, m_layerSize(windowSize)
		, m_windowSize(windowSize)
		, m_layerRenderer(nullptr)
		, m_previousUpdateTime(0)
		, m_timeAtUpdateStart(0)
	{
		m_graphics = (Graphics_Imp*) graphics;
		SafeAddRef(m_graphics);

		m_layerRenderer = new LayerRenderer(graphics);
		m_layerRenderer->SetWindowSize(windowSize);

		{
			ace::Vector2DF lpos[4];
			lpos[0].X = 0;
			lpos[0].Y = 0;
			lpos[1].X = windowSize.X;
			lpos[1].Y = 0;
			lpos[2].X = windowSize.X;
			lpos[2].Y = windowSize.Y;
			lpos[3].X = 0;
			lpos[3].Y = windowSize.Y;
			m_layerRenderer->SetLayerPosition(lpos);
		}
	}

	CoreLayer_Imp::~CoreLayer_Imp()
	{
		ClearPostEffects();

		SafeRelease(m_layerRenderer);

		SafeRelease(m_graphics);
	}

	int CoreLayer_Imp::GetDrawingPriority() const
	{
		return m_drawingPriority;
	}

	void CoreLayer_Imp::SetDrawingPriority(int value)
	{
		m_drawingPriority = value;
	}

	bool CoreLayer_Imp::GetIsDrawn() const
	{
		return m_isDrawn;
	}

	void CoreLayer_Imp::SetIsDrawn(bool value)
	{
		m_isDrawn = value;
	}

	void CoreLayer_Imp::AddPostEffect(CorePostEffect* postEffect)
	{
		SafeAddRef(postEffect);
		m_postEffects.push_back(postEffect);
	}

	void CoreLayer_Imp::ClearPostEffects()
	{
		for (auto& p : m_postEffects)
		{
			SafeRelease(p);
		}

		m_postEffects.clear();
	}

	void CoreLayer_Imp::SetScene(CoreScene* scene)
	{
		m_scene = scene;
	}

	void CoreLayer_Imp::BeginMeasureUpdateTime()
	{
		m_timeAtUpdateStart = ace::GetTime();
	}

	void CoreLayer_Imp::EndMeasureUpdateTime()
	{
		m_previousUpdateTime = ace::GetTime() - m_timeAtUpdateStart;
	}

	int CoreLayer_Imp::GetTimeForUpdate() const
	{
		return m_previousUpdateTime;
	}

}