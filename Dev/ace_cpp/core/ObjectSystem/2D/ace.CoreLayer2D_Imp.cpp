﻿
#include <list>
#include "ace.CoreLayer2D_Imp.h"
#include "../../Window/ace.Window_Imp.h"
#include "../../Graphics/Common/ace.Graphics_Imp.h"
#include "../../Graphics/Common/2D/ace.LayerRenderer.h"

using namespace std;

namespace ace
{
	//----------------------------------------------------------------------------------
	//
	//----------------------------------------------------------------------------------
	CoreLayer2D_Imp::CoreLayer2D_Imp(Graphics* graphics, Log* log, Vector2DI windowSize)
		: CoreLayer_Imp(graphics, windowSize)
		, m_objects(list<ObjectPtr>())
		, m_cameras(set<CoreCameraObject2D*>())
		, m_renderer(nullptr)
	{
		m_renderer = new Renderer2D_Imp(graphics, log, windowSize);
	}

	//----------------------------------------------------------------------------------
	//
	//----------------------------------------------------------------------------------
	CoreLayer2D_Imp::~CoreLayer2D_Imp()
	{
		SafeDelete(m_renderer);

		for (auto& object : m_objects)
		{
			SafeRelease(object);
		}

	}

	//----------------------------------------------------------------------------------
	//
	//----------------------------------------------------------------------------------
	void CoreLayer2D_Imp::AddObject(ObjectPtr object)
	{
		if (object->GetIsCamera())
		{
			auto camera = (CoreCameraObject2D*)object;
			m_cameras.insert(camera);
			SafeAddRef(camera);
		}
		else
		{
			m_objects.push_back(object);
		}

		object->SetLayer(this);
		SafeAddRef(object);
	}

	//----------------------------------------------------------------------------------
	//
	//----------------------------------------------------------------------------------
	void CoreLayer2D_Imp::RemoveObject(ObjectPtr object)
	{
		if (object->GetIsCamera())
		{
			auto camera = (CoreCameraObject2D*)object;
			m_cameras.erase(camera);
			SafeRelease(camera);
		}
		else
		{
			m_objects.remove(object);
		}

		object->SetLayer(nullptr);
		SafeRelease(object);
	}

	//----------------------------------------------------------------------------------
	//
	//----------------------------------------------------------------------------------
	void CoreLayer2D_Imp::BeginUpdating()
	{

	}

	//----------------------------------------------------------------------------------
	//
	//----------------------------------------------------------------------------------
	void CoreLayer2D_Imp::EndUpdating()
	{

	}

	void CoreLayer2D_Imp::DrawObjects(Matrix33 cameraMatrix)
	{
		for (auto& x : m_objects)
		{
			x->Draw(cameraMatrix);
		}
	}

	//----------------------------------------------------------------------------------
	//
	//----------------------------------------------------------------------------------
	void CoreLayer2D_Imp::BeginDrawing()
	{
		auto vanished = vector<ObjectPtr>();
		for (auto& x : m_objects)
		{
			if (!x->GetIsAlive())
			{
				vanished.push_back(x);
			}
		}

		for (auto& x : vanished)
		{
			RemoveObject(x);
		}
	}

	//----------------------------------------------------------------------------------
	//
	//----------------------------------------------------------------------------------
	void CoreLayer2D_Imp::Draw()
	{
		if (!m_isDrawn)
		{
			return;
		}

		if (!m_cameras.empty())
		{
			for (auto& c : m_cameras)
			{
				c->SetForRenderTarget();
				DrawObjects(c->GetCameraMatrix());
			}
		}
		else
		{
			DrawObjects(Matrix33());
		}

		m_scene->SetRenderTargetForDrawingLayer();

		for (auto& c : m_cameras)
		{
			c->DrawBuffer();
		}
	}

	//----------------------------------------------------------------------------------
	//
	//----------------------------------------------------------------------------------
	void CoreLayer2D_Imp::EndDrawing()
	{
		m_renderer->DrawCache();
		m_renderer->ClearCache();
	}
}