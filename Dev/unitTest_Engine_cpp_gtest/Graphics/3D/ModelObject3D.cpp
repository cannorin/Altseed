﻿
#include "../../EngineGraphics3DTest.h"


class Graphics_ModelObject3D : public EngineGraphics3DTest
{

public:

	Graphics_ModelObject3D(bool isOpenGLMode) :
		EngineGraphics3DTest(asd::ToAString("ModelObject3D"), isOpenGLMode, 15,true)
	{}

protected:
	std::shared_ptr<asd::ModelObject3D> meshObj;


	void OnStart() override
	{
		asd::RenderSettings settings;
		settings.IsLightweightMode = false;
		SetRenderSettings(settings);

		EngineGraphics3DTest::OnStart();

		meshObj = std::make_shared<asd::ModelObject3D>();
		auto lightObj = std::make_shared<asd::DirectionalLightObject3D>();

		auto graphics = asd::Engine::GetGraphics();

		auto model = graphics->CreateModel(asd::ToAString("Data/Model/Test_Animation.mdl").c_str());

		meshObj->SetModel(model);
		meshObj->SetPosition(asd::Vector3DF(0, 0, 0));
		lightObj->SetRotation(asd::Vector3DF(120, 50, 0));

		GetLayer3D()->AddObject(meshObj);
		GetLayer3D()->AddObject(lightObj);

		SetCameraParameter(30, 0, 0, 1, 60, 20);
		GetLayer3D()->SetSkyAmbientColor(asd::Color(50, 50, 70, 255));
		GetLayer3D()->SetGroundAmbientColor(asd::Color(70, 70, 50, 255));

		meshObj->PlayAnimation(0, asd::ToAString("anime1").c_str());
		
	}

	void OnUpdating() override
	{
		EngineGraphics3DTest::OnUpdating();
		//auto rot = meshObj->GetRotation();
		//meshObj->SetRotation(rot+asd::Vector3DF(5,0,0));

		if (GetTime() == 60)
		{
			meshObj->CrossFadeAnimation(0, asd::ToAString("anime1").c_str(), 1.0f);
		}
	}


};

ENGINE_TEST(Graphics, ModelObject3D)