﻿#include <exception>
#include <list>
#include "asd.Scene.h"
using namespace std;

namespace asd
{
	extern ObjectSystemFactory* g_objectSystemFactory;

	//----------------------------------------------------------------------------------
	//
	//----------------------------------------------------------------------------------
	Scene::Scene()
		: m_layersToDraw(list<Layer::Ptr>())
		, m_layersToUpdate(list<Layer::Ptr>())
		, m_coreScene(nullptr)
		, alreadyFirstUpdate(false)
		, m_componentManager(this)
		, m_isAlive(true)
	{
		m_coreScene = CreateSharedPtrWithReleaseDLL(g_objectSystemFactory->CreateScene());
	}

	//----------------------------------------------------------------------------------
	//
	//----------------------------------------------------------------------------------
	Scene::~Scene()
	{
		for (auto& layer : m_layersToDraw)
		{
			layer->SetScene(nullptr);
		}
	}


	// ========
	// 基本イベントハンドラ
	// ========

	void Scene::Dispose()
	{
		m_isAlive = false;
		for (auto& l : m_layersToUpdate)
		{
			if (l->GetIsAlive())
			{
				l->Dispose();
			}
		}
		OnDispose();
	}

	void Scene::Update()
	{
		m_layersToUpdate.sort([](const Layer::Ptr& x, const Layer::Ptr& y) -> bool
		{
			return x->GetUpdatePriority() < y->GetUpdatePriority();
		});

		auto beVanished = list<Layer::Ptr>();

		executing = true;

		OnUpdating();

		for (auto& layer : m_layersToUpdate)
		{
			layer->BeginUpdating();
		}

		for (auto& layer : m_layersToUpdate)
		{
			layer->Update();
			if (!layer->GetIsAlive())
			{
				beVanished.push_back(layer);
			}
		}

		for (auto& layer : m_layersToUpdate)
		{
			layer->EndUpdateting();
		}

		m_componentManager.Update();

		OnUpdated();

		executing = false;

		for (auto& layer : beVanished)
		{
			RemoveLayer(layer);
			layer->Dispose();
		}

		CommitChanges();
	}

	void Scene::CommitChanges()
	{
		for (auto layer : addingLayer)
		{
			AddLayer(layer);
		}

		for (auto layer : removingLayer)
		{
			RemoveLayer(layer);
		}
		addingLayer.clear();
		removingLayer.clear();
	}

	void Scene::Draw()
	{
		executing = true;

		m_layersToDraw.sort([](const Layer::Ptr& x, const Layer::Ptr& y) -> bool
		{
			return x->GetDrawingPriority() < y->GetDrawingPriority();
		});

		for (auto& layer : m_layersToDraw)
		{
			layer->DrawAdditionally();
		}

		m_coreScene->BeginDrawing();

		for (auto& layer : m_layersToDraw)
		{
			layer->BeginDrawing();
		}

		for (auto& layer : m_layersToDraw)
		{
			layer->Draw();
		}

		for (auto& layer : m_layersToDraw)
		{
			layer->EndDrawing();
		}

		m_coreScene->EndDrawing();

		executing = false;

		CommitChanges();
	}

	void Scene::OnDispose()
	{
	}

	void Scene::OnUpdating()
	{
	}

	void Scene::OnUpdated()
	{
	}

	// ========
	// 登録・登録解除関連イベントハンドラ
	// ========

	void Scene::RaiseOnRegistered()
	{
		OnRegistered();
	}

	void Scene::RaiseOnStartUpdating()
	{
		OnStartUpdating();
	}

	void Scene::RaiseOnTransitionFinished()
	{
		OnTransitionFinished();
	}

	void Scene::RaiseOnChanging()
	{
		OnChanging();
	}

	void Scene::RaiseOnStopUpdating()
	{
		OnStopUpdating();
	}

	void Scene::RaiseOnUnregistered()
	{
		OnUnregistered();
	}

	void Scene::OnRegistered()
	{
	}
	
	void Scene::OnStartUpdating()
	{
	}

	void Scene::OnTransitionFinished()
	{
	}

	void Scene::OnChanging()
	{
	}

	void Scene::OnStopUpdating()
	{
	}

	void Scene::OnUnregistered()
	{
	}


	// ========
	// その他
	// ========

	//----------------------------------------------------------------------------------
	//
	//----------------------------------------------------------------------------------
	bool Scene::GetHDRMode() const
	{
		return m_coreScene->GetHDRMode();
	}

	void Scene::SetHDRMode(bool value)
	{
		m_coreScene->SetHDRMode(value);
	}

	//----------------------------------------------------------------------------------
	//
	//----------------------------------------------------------------------------------
	void Scene::AddLayer(const Layer::Ptr& layer)
	{
		if (executing)
		{
			addingLayer.push_back(layer);
			return;
		}

		ACE_ASSERT(layer->GetScene() == nullptr, "追加しようとしたレイヤーは、すでに別のシーンに所属しています。");
		m_layersToDraw.push_back(layer);
		m_layersToUpdate.push_back(layer);
		m_coreScene->AddLayer(layer->GetCoreLayer().get());
		layer->SetScene(this);
		layer->RaiseOnAdded();
	}

	//----------------------------------------------------------------------------------
	//
	//----------------------------------------------------------------------------------
	void Scene::RemoveLayer(const Layer::Ptr& layer)
	{
		if (executing)
		{
			removingLayer.push_back(layer);
			return;
		}

		layer->RaiseOnRemoved();
		layer->SetScene(nullptr);
		m_layersToDraw.remove(layer);
		m_layersToUpdate.remove(layer);
		m_coreScene->RemoveLayer(layer->GetCoreLayer().get());
	}

	//----------------------------------------------------------------------------------
	//
	//----------------------------------------------------------------------------------
	void Scene::AddComponent(const SceneComponent::Ptr& component, astring key)
	{
		m_componentManager.Add(component, key);
	}

	//----------------------------------------------------------------------------------
	//
	//----------------------------------------------------------------------------------
	const SceneComponent::Ptr& Scene::GetComponent(astring key)
	{
		return m_componentManager.Get(key);
	}

	//----------------------------------------------------------------------------------
	//
	//----------------------------------------------------------------------------------
	bool Scene::RemoveComponent(astring key)
	{
		return m_componentManager.Remove(key);
	}

	//----------------------------------------------------------------------------------
	//
	//----------------------------------------------------------------------------------
	std::shared_ptr<RenderTexture2D> Scene::GetEffectedScreen()
	{
		auto target = m_coreScene->GetBaseTarget();
		if (target == nullptr) return std::shared_ptr<RenderTexture2D>();

		SafeAddRef(target);
		return CreateSharedPtrWithReleaseDLL(target);
	}

	const std::list<Layer::Ptr>& Scene::GetLayers() const
	{
		return m_layersToUpdate;
	}

	bool Scene::GetIsAlive() const
	{
		return m_isAlive;
	}
}
