﻿#include <Altseed.h>
#include <gtest/gtest.h>
#include <memory>
#include "../EngineTest.h"

using namespace std;
using namespace asd;

class ObjectSystem_ParentObject : public EngineTest
{
public:
	ObjectSystem_ParentObject(bool isOpenGLMode)
		: EngineTest(asd::ToAString("ParentObject"), isOpenGLMode, 50)
		, m_parent(nullptr)
	{
	}

protected:
	shared_ptr<TextureObject2D> m_parent;

	void OnStart()
	{
		auto scene = make_shared<Scene>();
		auto layer = make_shared<Layer2D>();
		auto child = make_shared<TextureObject2D>();
		auto child2 = make_shared<TextureObject2D>();
		m_parent = make_shared<TextureObject2D>();

		auto file = asd::Engine::GetFile();
		file->AddRootDirectory(asd::ToAString("Data/Texture").c_str());

		asd::Engine::ChangeScene(scene);
		scene->AddLayer(layer);
		layer->AddObject(m_parent);
		layer->AddObject(child);
		layer->AddObject(child2);
		m_parent->AddChild(child, ChildManagementMode::Nothing, ChildTransformingMode::All);
		child->AddChild(child2, ChildManagementMode::Nothing, ChildTransformingMode::Nothing);

		auto g = asd::Engine::GetGraphics();
		auto texture = g->CreateTexture2D(asd::ToAString("Cloud1.png").c_str());
		m_parent->SetTexture(texture);
		child->SetTexture(texture);
		child2->SetTexture(texture);

		m_parent->SetPosition(Vector2DF(320, 240));
		m_parent->SetCenterPosition(Vector2DF(128, 128));
		m_parent->SetColor(Color(255, 255, 128, 255));
		child->SetPosition(Vector2DF(50, 50));
		child2->SetPosition(Vector2DF(60, 60));
	}

	void OnUpdating()
	{
		m_parent->SetAngle(m_parent->GetAngle() + 0.2f);
		m_parent->SetScale(m_parent->GetScale() + Vector2DF(0.001f, 0.001f));
	}
};

ENGINE_TEST(ObjectSystem, ParentObject)
