﻿
#include "asd.Scene.h"
#include "asd.Layer.h"

using namespace std;

namespace asd
{
	void Layer::Update()
	{
		if (!m_isUpdatedCurrent || !m_isAlive)
		{
			return;
		}

		m_commonObject->BeginMeasureUpdateTime();

		m_updateTimer += m_updateFrequency;
		while (m_updateTimer >= 1)
		{
			OnUpdating();
			UpdateInternal();
			OnUpdated();
			m_updateTimer -= 1;
		}

		m_commonObject->EndMeasureUpdateTime();
	}

	void Layer::Start()
	{
		OnStart();
	}

	void Layer::Draw()
	{
		m_commonObject->Draw();
	}

	void Layer::OnStart()
	{
	}

	void Layer::OnUpdating()
	{
	}

	void Layer::OnUpdated()
	{
	}

	void Layer::OnDrawAdditionally()
	{
	}

	void Layer::OnVanish()
	{
	}

	void Layer::OnDispose()
	{
	}

	Layer::Layer()
		: m_scene(nullptr)
		, m_isUpdated(true)
		, m_isDrawn(true)
		, m_isAlive(true)
		, m_name(asd::ToAString("Layer"))
		, m_updatePriority(0)
		, m_updateFrequency(1)
		, m_updateTimer(0)
	{
	}

	Layer::~Layer()
	{
		m_commonObject.reset();
	}

	Scene* Layer::GetScene() const
	{
		return m_scene;
	}

	void Layer::SetScene(Scene* value)
	{
		m_scene = value;
	}

	std::shared_ptr<CoreLayer> Layer::GetCoreLayer() const
	{ 
		return m_commonObject; 
	}

	bool Layer::GetIsUpdated() const
	{
		return m_isUpdated;
	}

	void Layer::SetIsUpdated(bool value)
	{
		m_isUpdated = value;
	}

	bool Layer::GetIsDrawn() const
	{
		return m_isDrawn;
	}

	void Layer::SetIsDrawn(bool value)
	{
		m_isDrawn = value;
		m_commonObject->SetIsDrawn(value);
	}

	bool Layer::GetIsAlive() const
	{
		return m_isAlive;
	}

	int Layer::GetDrawingPriority() const
	{
		return m_commonObject->GetDrawingPriority();
	}

	void Layer::SetDrawingPriority(int value)
	{
		m_commonObject->SetDrawingPriority(value);
	}

	void Layer::AddPostEffect(const std::shared_ptr<PostEffect>& postEffect)
	{
		m_postEffects.push_back(postEffect);
		m_commonObject->AddPostEffect(postEffect->GetCoreObject());
	}

	void Layer::ClearPostEffects()
	{
		m_postEffects.clear();
		m_commonObject->ClearPostEffects();
	}

	void Layer::Vanish()
	{
		m_isAlive = false;
		OnVanish();
	}

	int Layer::GetTimeForUpdate() const
	{
		return m_commonObject->GetTimeForUpdate();
	}

	astring Layer::GetName() const
	{
		return m_name;
	}

	void Layer::SetName(astring value)
	{
		m_name = value;
	}

	int Layer::GetUpdatePriority() const
	{
		return m_updatePriority;
	}

	void Layer::SetUpdatePriority(int value)
	{
		m_updatePriority = value;
	}

	float Layer::GetUpdateFrequency() const
	{
		return m_updateFrequency;
	}

	void Layer::SetUpdateFrequency(float value)
	{
		m_updateFrequency = value;
	}

}