﻿#include <Altseed.h>

/**
@brief	Collision2Dのサンプル。マウスによって操作する円が固定されたいずれかのオブジェクトにヒットしたら円が赤く変化します。
*/
void Collision2D_Geometries()
{
	// Altseedを初期化する。
	asd::Engine::Initialize(asd::ToAString("Collision2D_Geometries").c_str(), 640, 480, asd::EngineOption());

	// 図形描画オブジェクトのインスタンスを生成する。
	std::shared_ptr<asd::GeometryObject2D> geometryObj0 = std::make_shared<asd::GeometryObject2D>();
	std::shared_ptr<asd::GeometryObject2D> geometryObj1 = std::make_shared<asd::GeometryObject2D>();
	std::shared_ptr<asd::GeometryObject2D> geometryObj2 = std::make_shared<asd::GeometryObject2D>();
	std::shared_ptr<asd::GeometryObject2D> geometryObj3 = std::make_shared<asd::GeometryObject2D>();

	// マウスによって動かす円。
	std::shared_ptr<asd::CircleShape> selfCircle;

	// 停止している円、三角形、矩形。
	std::shared_ptr<asd::CircleShape> circle;
	std::shared_ptr<asd::TriangleShape> triangle;
	std::shared_ptr<asd::RectangleShape> rect;


	// 図形描画オブジェクトをエンジンに追加する。
	asd::Engine::AddObject2D(geometryObj0);
	asd::Engine::AddObject2D(geometryObj1);
	asd::Engine::AddObject2D(geometryObj2);
	asd::Engine::AddObject2D(geometryObj3);

	// マウスによって動かす円の形状と描画の設定を行う。
	{
		// 円の外径を指定する。
		selfCircle = std::make_shared<asd::CircleShape>();
		selfCircle->SetOuterDiameter(100);

		// マウスによって動かす円を描画する図形として設定する。
		geometryObj0->SetShape(selfCircle);
	}

	// 停止している円の形状と描画の設定を行う。
	{
		// 円の外径、中心位置を指定する。
		circle = std::make_shared<asd::CircleShape>();
		circle->SetOuterDiameter(100);
		circle->SetPosition(asd::Vector2DF(400, 100));

		// 停止している円を描画する図形として設定する。
		geometryObj1->SetShape(circle);
	}

	// 停止している三角形の形状と描画の設定を行う。
	{
		// 三角形の各頂点の位置を指定する。
		triangle = std::make_shared<asd::TriangleShape>();
		triangle->SetPointByIndex(asd::Vector2DF(440, 350), 0);
		triangle->SetPointByIndex(asd::Vector2DF(340, 450), 1);
		triangle->SetPointByIndex(asd::Vector2DF(580, 450), 2);

		// 停止している三角形を描画する図形として設定する。
		geometryObj2->SetShape(triangle);
		geometryObj2->SetPosition(asd::Vector2DF(0, 0));
	}

	// 停止している矩形の形状と描画の設定を行う。
	{
		rect = std::make_shared<asd::RectangleShape>();
		rect->SetDrawingArea(asd::RectF(70, 110, 300, 200));
		rect->SetAngle(15);
		
		// 停止している矩形を描画する図形として設定する。
		geometryObj3->SetShape(rect);
	}

	// Altseedのウインドウが閉じられていないか確認する。
	while (asd::Engine::DoEvents())
	{
		// マウスによって制御される円の中心位置をマウスの位置とする。
		selfCircle->SetPosition(asd::Engine::GetMouse()->GetPosition());

		// 停止している円・三角形・矩形のいずれかに、マウスによって動く円が衝突した時に円を赤く変化させる。
		// そうでない時は白く変化させる。
		if (selfCircle->GetIsCollidedWith(circle)
			|| selfCircle->GetIsCollidedWith(triangle)
			|| selfCircle->GetIsCollidedWith(rect))
		{
			geometryObj0->SetColor(asd::Color(255, 0, 0, 255));
		}
		else
		{
			geometryObj0->SetColor(asd::Color(255, 255, 255, 255));
		}

		// Altseedを更新する。
		asd::Engine::Update();
	}

	// Altseedの終了処理をする。
	asd::Engine::Terminate();

	return;
}
