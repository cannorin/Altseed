﻿using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace asd
{
	public class EngineOption
	{
		/// <summary>
		/// フルスクリーンで起動するか?
		/// </summary>
		public bool IsFullScreen = false;

		/// <summary>
		/// 描画に使用するデバイス
		/// </summary>
		public GraphicsDeviceType GraphicsDevice = GraphicsDeviceType.Default;

		/// <summary>
		/// ウインドウの初期配置
		/// </summary>
		public WindowPositionType WindowPosition = WindowPositionType.Default;

		/// <summary>
		/// リソースの再読み込みを有効にするかどうか?
		/// </summary>
		public bool IsReloadingEnabled = false;

		/// <summary>
		/// 起動時に自動的に生成されるシーンとレイヤーの種類
		/// </summary>
		public AutoGeneratedLayerType AutoGeneratedLayer = AutoGeneratedLayerType.Layer2D;
	};

	public class Engine
	{
		#region 遷移状態クラス
		abstract class SceneTransitionState
		{
			/// <summary>
			/// シーン遷移に関する処理を進行させる。
			/// </summary>
			public virtual void Proceed()
			{
			}
			/// <summary>
			/// シーン遷移処理を更新する。
			/// </summary>
			public virtual void Update()
			{
			}
			/// <summary>
			/// シーンを描画する。
			/// </summary>
			public abstract void Draw();
		}

		class NeutralState : SceneTransitionState
		{
			public override void Draw()
			{
				if(CurrentScene != null)
				{
					core.DrawSceneToWindow(CurrentScene.CoreInstance);
				}
			}
		}

		class FadingOutState : SceneTransitionState
		{
			private Transition transition;

			public FadingOutState(Transition transition, Scene nextScene)
			{
				this.transition = transition;
				Engine.nextScene = nextScene;
			}

			public override void Proceed()
			{
				if(transition.IsSceneChanged)
				{
					if(CurrentScene != null)
					{
						CurrentScene.RaiseOnStopUpdating();
					}
					if(nextScene != null)
					{
						nextScene.RaiseOnStartUpdating();
						core.ChangeScene(nextScene.CoreInstance);
					}
					else
					{
						core.ChangeScene(null);
					}
					transitionState = new FadingInState(transition, CurrentScene);
					CurrentScene = nextScene;
				}
			}

			public override void Update()
			{
				transition.OnUpdate();
			}

			public override void Draw()
			{
				var curScene = CurrentScene != null ? CurrentScene.CoreInstance : null;
				core.DrawSceneToWindowWithTransition(null, curScene, transition.SwigObject);
			}
		}

		class FadingInState : SceneTransitionState
		{
			private Transition transition;
			private Scene previousScene;

			public FadingInState(Transition transition, Scene previousScene)
			{
				this.transition = transition;
				this.previousScene = previousScene;
			}

			public override void Proceed()
			{
				if(transition.IsFinished)
				{
					if(previousScene != null)
					{
						previousScene.RaiseOnUnregistered();
					}
					if(CurrentScene != null)
					{
						CurrentScene.RaiseOnTransitionFinished();
					}
					transitionState = new NeutralState();
				}
			}

			public override void Update()
			{
				transition.OnUpdate();
			}

			public override void Draw()
			{
				var curScene = CurrentScene != null ? CurrentScene.CoreInstance : null;
				var prevScene = previousScene != null ? previousScene.CoreInstance : null;
				core.DrawSceneToWindowWithTransition(curScene, prevScene, transition.SwigObject);
			}
		}

		class QuicklyChangingState : SceneTransitionState
		{
			public QuicklyChangingState(Scene nextScene)
			{
				Engine.nextScene = nextScene;
			}

			public override void Proceed()
			{
				if(CurrentScene != null)
				{
					CurrentScene.RaiseOnStopUpdating();
					CurrentScene.RaiseOnUnregistered();
				}
				if(nextScene != null)
				{
					nextScene.RaiseOnStartUpdating();
					nextScene.RaiseOnTransitionFinished();
					core.ChangeScene(nextScene.CoreInstance);
				}
				else
				{
					core.ChangeScene(null);
				}
				transitionState = new NeutralState();
			}

			public override void Draw()
			{
				if(CurrentScene != null)
				{
					core.DrawSceneToWindow(CurrentScene.CoreInstance);
				}
			}
		}
		#endregion

		static swig.Core_Imp core = null;
		static swig.LayerProfiler layerProfiler = null;

		/// <summary>
		/// 現在描画対象となっているシーンを表す asd.Scene クラスのインスタンスを取得します。
		/// </summary>
		public static Scene CurrentScene { get; private set; }
		public static Log Logger { get; private set; }
		public static Profiler Profiler { get; private set; }
		public static Keyboard Keyboard { get; private set; }
		public static Mouse Mouse { get; private set; }
		public static JoystickContainer JoystickContainer { get; private set; }
		public static Sound Sound { get; private set; }
		public static Graphics Graphics { get; private set; }
		public static AnimationSystem AnimationSystem { get; private set; }
		public static File File { get; private set; }

		/// <summary>
		/// プロファイリング結果を画面に表示するかどうかを表す真偽値を取得または設定する。
		/// </summary>
		public static bool ProfilerIsVisible
		{
			get { return core.GetProfilerVisibility(); }
			set { core.SetProfilerVisibility(value); }
		}

		internal static ObjectSystemFactory ObjectSystemFactory { get; private set; }
		private static Scene nextScene;
		private static SceneTransitionState transitionState;

		/// <summary>
		/// 初期化を行う。
		/// </summary>
		/// <param name="title">タイトル</param>
		/// <param name="width">横幅</param>
		/// <param name="height">縦幅</param>
		/// <param name="option">オプション</param>
		/// <returns>成否</returns>
		public static bool Initialize(string title, int width, int height, EngineOption option)
		{
			if(core != null) return false;

			if (!Particular.Helper.CheckInitialize()) return false;

			core = Particular.Helper.CreateCore();
			if (core == null) return false;

			var graphicsType = option.GraphicsDevice;
			if(graphicsType == GraphicsDeviceType.Default)
			{
				graphicsType = Particular.Helper.GetDefaultDevice();
			}

			var coreOption = new swig.CoreOption();
			coreOption.GraphicsDevice = (swig.GraphicsDeviceType)graphicsType;
			coreOption.IsFullScreen = option.IsFullScreen;
			coreOption.WindowPosition = (swig.WindowPositionType)option.WindowPosition;
			coreOption.IsReloadingEnabled = option.IsReloadingEnabled;

			var result = core.Initialize(title, width, height, coreOption);

			if(result)
			{
				GC.Initialize();
				SetupMembers();

				if(option.AutoGeneratedLayer == AutoGeneratedLayerType.Layer2D)
				{
					var scene = new Scene();
					var layer = new Layer2D();
					scene.AddLayer(layer);
					ChangeScene(scene);
				}
				else if(option.AutoGeneratedLayer == AutoGeneratedLayerType.Layer3D)
				{
					var scene = new Scene();
					var layer = new Layer3D();
					scene.AddLayer(layer);
					ChangeScene(scene);
				}

				return true;
			}
			else
			{
				core.Release();
				core = null;
				return false;
			}
		}

		/// <summary>
		/// 外部ウインドウにゲーム画面を表示する初期化を行う。
		/// </summary>
		/// <param name="handle1">ハンドル1</param>
		/// <param name="handle2">ハンドル2</param>
		/// <param name="width">横幅</param>
		/// <param name="height">縦幅</param>
		/// <param name="option">オプション</param>
		/// <returns>成否</returns>
		public static bool InitializeByExternalWindow(IntPtr handle1, IntPtr handle2, int width, int height, EngineOption option)
		{
			if(core != null) return false;

			if(!Particular.Helper.CheckInitialize()) return false;

			core = Particular.Helper.CreateCore();
			if (core == null) return false;


			var graphicsType = option.GraphicsDevice;
			if(graphicsType == GraphicsDeviceType.Default)
			{
				graphicsType = Particular.Helper.GetDefaultDevice();
			}

			var coreOption = new swig.CoreOption();
			coreOption.GraphicsDevice = (swig.GraphicsDeviceType)graphicsType;
			coreOption.IsFullScreen = option.IsFullScreen;
			coreOption.IsReloadingEnabled = option.IsReloadingEnabled;

			var result = core.InitializeByExternalWindow(handle1, handle2, width, height, coreOption);

			if(result)
			{
				GC.Initialize();
				SetupMembers();

				if(option.AutoGeneratedLayer == AutoGeneratedLayerType.Layer2D)
				{
					var scene = new Scene();
					var layer = new Layer2D();
					scene.AddLayer(layer);
					ChangeScene(scene);
				}
				else if(option.AutoGeneratedLayer == AutoGeneratedLayerType.Layer3D)
				{
					var scene = new Scene();
					var layer = new Layer3D();
					scene.AddLayer(layer);
					ChangeScene(scene);
				}

				return true;
			}
			else
			{
				core.Release();
				core = null;
				return false;
			}
		}

		/// <summary>
		/// タイトルを設定する。
		/// </summary>
		public static string Title
		{
			set
			{
				if (core == null) return;
				core.SetTitle(value);
			}
		}

		/// <summary>
		/// イベントを実行し、進行可否を判断する。
		/// </summary>
		/// <returns>進行可能か?</returns>
		public static bool DoEvents()
		{
			if(core == null) return false;

			GC.Update();

			bool mes = core.DoEvents();

			if(Mouse != null)
			{
				Mouse.RefreshAllState();
			}

			transitionState.Proceed();
			if(!CurrentScene.IsAlive)
			{
				transitionState = new QuicklyChangingState(null);
			}

			return mes;
		}

		/// <summary>
		/// 更新処理を行う。
		/// </summary>
		public static void Update()
		{
			if(core == null) return;

			core.BeginDrawing();

			layerProfiler.Refresh();

			if(CurrentScene != null)
			{
				CurrentScene.Update();

				foreach(var item in CurrentScene.Layers)
				{
					layerProfiler.Record(item.Name, item.ObjectCount, item.TimeForUpdate);
				}
			}

			if(CurrentScene != null)
			{
				CurrentScene.Draw();
			}

			transitionState.Update();
			transitionState.Draw();

			core.Draw();

			core.EndDrawing();

		}

		/// <summary>
		/// 終了処理を行う。
		/// </summary>
		public static void Terminate()
		{
			if(core == null) return;

			if(CurrentScene != null)
			{
				CurrentScene.Dispose();
			}

			if(nextScene != null)
			{
				nextScene.Dispose();
			}


			CurrentScene = null;
			nextScene = null;

			GC.Terminate();

			core.Terminate();
			core.Release();
			core = null;

			Mouse = null;

			var refCount = swig.asd_core.GetGlobalReferenceCount__();

			if(refCount > 0)
			{
				Particular.Helper.ThrowUnreleasedInstanceException(refCount);
			}
		}

		/// <summary>
		/// マウスカーソルを作成する。
		/// </summary>
		/// <param name="path">画像のパス</param>
		/// <param name="hot">カーソルの相対座標</param>
		/// <returns>カーソル</returns>
		public static Cursor CreateCursor(string path, Vector2DI hot)
		{
			return GC.GenerateCursor(core.CreateCursor(path, hot), GC.GenerationType.Create);
		}

		/// <summary>
		/// マウスカーソルを設定する。
		/// </summary>
		/// <param name="cursor">カーソル</param>
		public static void SetCursor(Cursor cursor)
		{
			core.SetCursor(IG.GetCursor(cursor));
		}

		/// <summary>
		/// クリップボードの内容を取得、または設定する。
		/// </summary>
		public static string ClipboardString
		{
			get
			{
				return core.GetClipboardString();
			}
			set
			{
				core.SetClipboardString(value);
			}
		}

		/// <summary>
		/// フルスクリーンモードかどうか設定する。
		/// </summary>
		/// <remarks>
		/// 現状、DirectXのみ有効である。
		/// </remarks>
		public static bool IsFullscreenMode
		{
			set
			{
				if (core == null) return;
				core.SetIsFullscreenMode(value);
			}
		}

		/// <summary>
		/// 内部の参照数を取得する。
		/// </summary>
		/// <remarks>
		/// 内部オブジェクト間の参照の数を取得する。
		/// 正しく内部オブジェクトが破棄されているか調査するためのデバッグ用である。
		/// </remarks>
		public static int ReferenceCount
		{
			get
			{
				return swig.asd_core.GetGlobalReferenceCount__();
			}
		}

		/// <summary>
		/// 一番最初に追加された2Dレイヤーにオブジェクトを追加する。
		/// </summary>
		/// <param name="o">オブジェクト</param>
		/// <returns>成否</returns>
		public static bool AddObject2D(Object2D o)
		{
			Scene scene = null;

			if(CurrentScene != null)
			{
				scene = CurrentScene;
			}
			else if(nextScene != null)
			{
				scene = nextScene;
			}

			if(scene == null) return false;

			var layers = scene.Layers;

			foreach(var layer in layers)
			{
				if(layer.LayerType == LayerType.Layer2D)
				{
					var layer2d = (Layer2D)layer;
					layer2d.AddObject(o);
					return true;
				}
			}

			return false;
		}

		/// <summary>
		/// 一番最初に追加された2Dレイヤーにオブジェクトを削除する。
		/// </summary>
		/// <param name="o">オブジェクト</param>
		/// <returns>成否</returns>
		public static bool RemoveObject2D(Object2D o)
		{
			Scene scene = null;

			if(CurrentScene != null)
			{
				scene = CurrentScene;
			}
			else if(nextScene != null)
			{
				scene = nextScene;
			}

			if(scene == null) return false;

			var layers = scene.Layers;

			foreach(var layer in layers)
			{
				if(layer.LayerType == LayerType.Layer2D)
				{
					var layer2d = (Layer2D)layer;
					layer2d.RemoveObject(o);
					return true;
				}
			}

			return false;
		}

		/// <summary>
		/// 一番最初に追加された3Dレイヤーにオブジェクトを追加する。
		/// </summary>
		/// <param name="o">オブジェクト</param>
		/// <returns>成否</returns>
		public static bool AddObject3D(Object3D o)
		{
			Scene scene = null;

			if(CurrentScene != null)
			{
				scene = CurrentScene;
			}
			else if(nextScene != null)
			{
				scene = nextScene;
			}

			if(scene == null) return false;

			var layers = scene.Layers;

			foreach(var layer in layers)
			{
				if(layer.LayerType == LayerType.Layer3D)
				{
					var layer3d = (Layer3D)layer;
					layer3d.AddObject(o);
					return true;
				}
			}

			return false;
		}

		/// <summary>
		/// 一番最初に追加された3Dレイヤーにオブジェクトを削除する。
		/// </summary>
		/// <param name="o">オブジェクト</param>
		/// <returns>成否</returns>
		public static bool RemoveObject3D(Object3D o)
		{
			Scene scene = null;

			if(CurrentScene != null)
			{
				scene = CurrentScene;
			}
			else if(nextScene != null)
			{
				scene = nextScene;
			}

			if(scene == null) return false;

			var layers = scene.Layers;

			foreach(var layer in layers)
			{
				if(layer.LayerType == LayerType.Layer3D)
				{
					var layer3d = (Layer3D)layer;
					layer3d.RemoveObject(o);
					return true;
				}
			}

			return false;
		}

		/// <summary>
		/// 描画する対象となるシーンを変更する。
		/// </summary>
		/// <param name="scene">次のシーン</param>
		public static void ChangeScene(Scene scene)
		{
			transitionState = new QuicklyChangingState(scene);
			if(CurrentScene != null)
			{
				CurrentScene.RaiseOnChanging();
			}
			if(scene != null)
			{
				scene.RaiseOnRegistered();
			}
		}

		/// <summary>
		/// 描画する対象となるシーンを画面遷移効果ありで変更する。
		/// </summary>
		/// <param name="scene">次のシーン</param>
		/// <param name="transition">画面遷移効果</param>
		public static void ChangeSceneWithTransition(Scene scene, Transition transition)
		{
			transitionState = new FadingOutState(transition, scene);
			if(CurrentScene != null)
			{
				CurrentScene.RaiseOnChanging();
			}
			if(scene != null)
			{
				scene.RaiseOnRegistered();
			}
		}

		/// <summary>
		/// ウインドウを閉じる。
		/// </summary>
		public static void Close()
		{
			core.Close();
		}

		/// <summary>
		/// スクリーンショットを保存する。
		/// </summary>
		/// <param name="path">出力先</param>
		/// <remarks>
		/// Windowsの場合、pngとjpg形式の保存に対応している。他のOSではpng形式の保存に対応している。
		/// 形式の種類は出力先の拡張子で判断する。
		/// </remarks>
		public static void TakeScreenshot(string path)
		{
			core.TakeScreenshot(path);
		}

		/// <summary>
		/// スクリーンショットをgifアニメーションとして録画する。
		/// </summary>
		/// <param name="path">出力先</param>
		/// <param name="frame">録画フレーム数</param>
		/// <param name="frequency_rate">録画頻度(例えば、1だと1フレームに1回保存、0.5だと2フレームに1回保存)</param>
		/// <param name="scale">ウインドウサイズに対する画像サイズの拡大率(ウインドウサイズが320の場合、0.5を指定すると160の画像が出力される)</param>
		/// <remarks>
		/// 実行してから一定時間の間、録画を続ける。
		/// 録画が終了するまでにアプリケーションが終了された場合、終了した時点までの録画結果が出力される。
		/// </remarks>
		public static void CaptureScreenAsGifAnimation(string path, int frame, float frequency_rate, float scale)
		{
			core.CaptureScreenAsGifAnimation(path, frame, frequency_rate, scale);
		}

		/// <summary>
		/// 1フレームで経過した実時間(秒)を取得、または設定する。
		/// </summary>
		/// <remarks>
		/// 基本的に開発者は使用する取得のみで設定する必要はない。
		/// 何らかの理由で無理やり経過時間を指定する場合に使用する。
		/// </remarks>
		public static float DeltaTime
		{
			get
			{
				return core.GetDeltaTime();
			}
			set
			{
				core.SetDeltaTime(value);
			}
		}

		/// <summary>
		/// 現在のFPSを取得する。
		/// </summary>
		public static float CurrentFPS
		{
			get
			{
				return core.GetCurrentFPS();
			}
		}

		/// <summary>
		/// 目標FPSを取得、または設定する。
		/// </summary>
		public static int TargetFPS
		{
			get
			{
				return core.GetTargetFPS();
			}
			set
			{
				core.SetTargetFPS(value);
			}
		}

		/// <summary>
		/// 間を指定可能なオブジェクトの実時間あたりの進行速度を取得、または設定する。
		/// </summary>
		public static float TimeSpan
		{
			get
			{
				return core.GetTimeSpan();
			}
			set
			{
				core.SetTimeSpan(value);
			}
		}

		/// <summary>
		/// フレームレートの制御方法を取得、または設定する。
		/// </summary>
		public static FramerateMode FramerateMode
		{
			get
			{
				return (FramerateMode)core.GetFramerateMode();
			}
			set
			{
				core.SetFramerateMode((swig.FramerateMode)value);
			}
		}

		/// <summary>
		/// ウインドウズの場合、ウインドウハンドルを取得する。
		/// </summary>
		public static IntPtr WindowHandle
		{
			get
			{
				return core.GetWindowHandle();
			}
		}

		private static void SetupMembers()
		{
			CurrentScene = null;
			Logger = new Log(core.GetLogger());
			Keyboard = new Keyboard(core.GetKeyboard());

			if(core.GetMouse() != null)
			{
				Mouse = new Mouse(core.GetMouse());
			}

			if(core.GetFile() != null)
			{
				File = new File(core.GetFile());
			}

			if(core.GetJoystickContainer() != null)
			{
				JoystickContainer = new JoystickContainer(core.GetJoystickContainer());
			}

			Sound = new Sound(core.GetSound());
			Graphics = new Graphics(core.GetGraphics_Imp());
			ObjectSystemFactory = new asd.ObjectSystemFactory(core.GetObjectSystemFactory());
			Profiler = new Profiler(core.GetProfiler());
			AnimationSystem = new AnimationSystem(core.GetAnimationSyatem());

			layerProfiler = core.GetLayerProfiler();
		}

		/// <summary>
		/// ウィンドウのサイズを取得する。
		/// </summary>
		/// <returns>ウィンドウのサイズ</returns>
		public static Vector2DI WindowSize
		{
			get
			{
				return core.GetWindowSize();
			}
		}
	}
}
