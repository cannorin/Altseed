﻿/// <summary>
/// 円を描画するサンプル。
/// </summary>
public class GeometryObject2D_CircleShape : ISample
{
    public string Description
    {
        get { return ""; }
    }

    public void Run()
    {   
        // Altseedを初期化する。
        asd.Engine.Initialize("GeometryObject2D_CircleShape", 640, 480, new asd.EngineOption());

        // 図形描画オブジェクトのインスタンスを生成する。
        var geometryObj = new asd.GeometryObject2D();

        // 図形描画オブジェクトのインスタンスをエンジンに追加する。
        asd.Engine.AddObject2D(geometryObj);

        // 円の図形クラスのインスタンスを生成する。
        var arc = new asd.CircleShape();

        // 円の外径、中心位置を指定する。
        arc.OuterDiameter = 400;
        arc.Position = new asd.Vector2DF(320, 240);

        // 円を描画する図形として設定する。
        geometryObj.Shape = arc;

        // Altseedのウインドウが閉じられていないか確認する。
        while (asd.Engine.DoEvents())
        {
            // Altseedを更新する。
            asd.Engine.Update();

            Recorder.TakeScreenShot("GeometryObject2D_CircleShape", 30);
        }

        // Altseedを終了する。
        asd.Engine.Terminate();
    }
}
