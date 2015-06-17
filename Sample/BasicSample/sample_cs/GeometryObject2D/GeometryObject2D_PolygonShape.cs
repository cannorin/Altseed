﻿using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

/// <summary>
/// 多角形を表示するサンプル。
/// </summary>
class GeometryObject2D_PolygonShape : ISample
{
	public void Run()
	{
		// Altseedを初期化する
		asd.Engine.Initialize("Geometry2D_PolygonShape", 640, 480, new asd.EngineOption());

		var texture = asd.Engine.Graphics.CreateTexture2D("Data/Texture/Sample1.png");

		// 図形描画クラスのコンストラクタを呼び出す
		var geometryObj = new asd.GeometryObject2D();

		// 図形描画クラスをレイヤーに追加する。
		asd.Engine.AddObject2D(geometryObj);

		// 多角形を図形描画クラスにて描画する。
		{

			var polygon = new asd.PolygonShape();
			// 多角形を構成する頂点を追加していく。（星形になるようにする。）
			for (int i = 0; i < 10; ++i)
			{
				asd.Vector2DF vec = new asd.Vector2DF(1, 0);
				vec.Degree = i * 36;
				vec.Length = (i % 2 == 0) ? 100 : 55;
				polygon.AddVertex(vec + new asd.Vector2DF(500, 250));

			}

			// 多角形を描画する図形として設定し、合成するテクスチャも設定。
			geometryObj.Shape = polygon;
			geometryObj.Texture = texture;
			geometryObj.Position = new asd.Vector2DF(0, 0);
		}

		// Altseedのウインドウが閉じられていないか確認する。
		while (asd.Engine.DoEvents())
		{
			// Altseedを更新する。
			asd.Engine.Update();
		}

		// Altseedの終了処理をする。
		asd.Engine.Terminate();
	}
}