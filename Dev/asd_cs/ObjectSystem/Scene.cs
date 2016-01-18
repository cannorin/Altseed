﻿using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace asd
{
	/// <summary>
	/// レイヤーの更新と描画を管理するシーン機能を提供するクラス。
	/// </summary>
	public class Scene : IReleasable
	{
		/// <summary>
		/// コンストラクタ
		/// </summary>
		public Scene()
		{
			CoreInstance = Engine.ObjectSystemFactory.CreateScene();

			var p = CoreInstance.GetPtr();

			if (GC.Scenes.Contains(p))
			{
				Particular.Helper.ThrowException("");
			}

			GC.Scenes.AddObject(p, this);

			layersToDraw_ = new List<Layer>();
			layersToUpdate_ = new List<Layer>();
			componentManager_ = new ComponentManager<Scene, SceneComponent>(this);
			
			IsAlive = true;
		}

		#region GC対策
		~Scene()
		{
			ForceToRelease();
		}

		public bool IsReleased
		{
			get
			{
				return CoreInstance == null;
			}
		}

		public void ForceToRelease()
		{
			lock (this)
			{
				if (CoreInstance == null) return;
				GC.Collector.AddObject(CoreInstance);
				CoreInstance = null;
			}
			Particular.GC.SuppressFinalize(this);
		}
		#endregion


		public bool IsAlive { get; set; }

		/// <summary>
		/// 描画先がHDRかどうか、取得、または設定する。
		/// </summary>
		public bool HDRMode
		{
			get { return CoreInstance.GetHDRMode(); }
			set { CoreInstance.SetHDRMode(value); }
		}

		/// <summary>
		/// 全てのレイヤーとポストエフェクトが描画され終わった画面をテクスチャとして取得する。
		/// </summary>
		/// <returns>画面</returns>
		/// <remarks>テクスチャの内容はシーンが描画されるたびに変わる。主にシーン遷移の際に使用する。</remarks>
		public RenderTexture2D EffectedScreen
		{
			get
			{
				return GC.GenerateRenderTexture2D(CoreInstance.GetBaseTarget(), GC.GenerationType.Get);
			}
		}

		/// <summary>
		/// 所属しているレイヤーを取得する。
		/// </summary>
		public IEnumerable<Layer> Layers { get { return layersToUpdate_; } }


		/// <summary>
		/// 指定したレイヤーをこのシーンに追加する。
		/// </summary>
		/// <param name="layer">追加されるレイヤー</param>
		public void AddLayer(Layer layer)
		{
			if (executing)
			{
				addingLayer.AddLast(layer);
				return;
			}

			if (layer.Scene != null)
			{
				throw new ArgumentException("指定したレイヤーは、既に別のシーンに所属しています。", "layer");
			}
			layersToDraw_.Add(layer);
			layersToUpdate_.Add(layer);
			CoreInstance.AddLayer(layer.CoreLayer);
			layer.Scene = this;
			layer.RaiseOnAdded();
		}

		/// <summary>
		/// 指定したレイヤーをこのシーンから削除する。
		/// </summary>
		/// <param name="layer">削除されるレイヤー</param>
		public void RemoveLayer(Layer layer)
		{
			if (executing)
			{
				removingLayer.AddLast(layer);
				return;
			}

			layersToDraw_.Remove(layer);
			layersToUpdate_.Remove(layer);
			CoreInstance.RemoveLayer(layer.CoreLayer);
			layer.RaiseOnRemoved();
			layer.Scene = null;
		}

		/// <summary>
		/// 指定したコンポーネントをこのシーンに追加する。
		/// </summary>
		/// <param name="component">追加するコンポーネント</param>
		/// <param name="key">コンポーネントに関連付けるキー</param>
		public void AddComponent(SceneComponent component, string key)
		{
			componentManager_.Add(component, key);
		}

		/// <summary>
		/// 指定したキーを持つコンポーネントを取得する。
		/// </summary>
		/// <param name="key">取得するコンポーネントのキー</param>
		/// <returns>コンポーネント</returns>
		public SceneComponent GetComponent(string key)
		{
			return componentManager_.Get(key);
		}

		/// <summary>
		/// 指定したコンポーネントをこのシーンから削除する。
		/// </summary>
		/// <param name="key">削除するコンポーネントを示すキー</param>
		public bool RemoveComponent(string key)
		{
			return componentManager_.Remove(key);
		}

		#region イベントハンドラ
		/// <summary>
		/// オーバーライドして、このシーンを初期化処理を記述できる。
		/// </summary>
		protected virtual void OnRegistered()
		{
		}

		/// <summary>
		/// オーバーライドして、最初のシーン更新時に実行する処理を記述する。
		/// </summary>
		protected virtual void OnStartUpdating()
		{
		}

		/// <summary>
		/// オーバーライドして、トランジション終了時に実行する処理を記述する。(※DoEvents関数内で実行される。)
		/// </summary>
		protected virtual void OnTransitionFinished()
		{
		}

		protected virtual void OnChanging()
		{
		}

		/// <summary>
		/// オーバーライドして、このシーンから別のシーンに切り替わる際に実行される処理を記述する。
		/// </summary>
		protected virtual void OnStopUpdating()
		{
		}

		protected virtual void OnUnregistered()
		{
		}

		/// <summary>
		/// オーバーライドして、このシーンが無条件に破棄される際に実行される処理を記述する。
		/// </summary>
		protected virtual void OnDispose()
		{
		}

		/// <summary>
		/// オーバーライドして、Updateの直前に実行する処理を記述する。
		/// </summary>
		protected virtual void OnUpdating()
		{
		}

		/// <summary>
		/// オーバーライドして、Updateの直後に実行する処理を記述する。
		/// </summary>
		protected virtual void OnUpdated()
		{
		}

		internal void RaiseOnRegistered()
		{
			OnRegistered();
		}

		internal void RaiseOnStartUpdating()
		{
			OnStartUpdating();
		}

		internal void RaiseOnTransitionFinished()
		{
			OnTransitionFinished();
		}

		internal void RaiseOnChanging()
		{
			OnChanging();
		}

		internal void RaiseOnStopUpdating()
		{
			OnStopUpdating();
		}

		internal void RaiseOnUnregistered()
		{
			OnUnregistered();
		}

		public void Dispose()
		{
			foreach (var item in layersToUpdate_)
			{
				if (item.IsAlive)
				{
					item.Dispose();
				}
			}
			IsAlive = false;
			OnDispose();
			ForceToRelease();
		}

		internal void Update()
		{
			var beVanished = new List<Layer>();

			executing = true;

			layersToUpdate_.Sort((x, y) => x.UpdatePriority - y.UpdatePriority);

			OnUpdating();

			foreach(var item in layersToUpdate_)
			{
				item.BeginUpdating();
			}

			foreach(var item in layersToUpdate_)
			{
				item.Update();
				if(!item.IsAlive)
				{
					beVanished.Add(item);
				}
			}

			foreach(var item in layersToUpdate_)
			{
				item.EndUpdating();
			}

			componentManager_.Update();

			OnUpdated();

			executing = false;

			foreach(var item in beVanished)
			{
				RemoveLayer(item);
				item.Dispose();
			}

			CommitChanges();
		}

		void CommitChanges()
		{
			foreach(var layer in addingLayer)
			{
				AddLayer(layer);
			}

			foreach(var layer in removingLayer)
			{
				RemoveLayer(layer);
			}

			addingLayer.Clear();
			removingLayer.Clear();
		}

		internal void Draw()
		{
			executing = true;

			Particular.Helper.SortLayersToDraw(layersToDraw_);

			foreach(var item in layersToDraw_)
			{
				item.DrawAdditionally();
			}

			CoreInstance.BeginDrawing();

			foreach(var item in layersToDraw_)
			{
				item.BeginDrawing();
			}

			foreach(var item in layersToDraw_)
			{
				item.Draw();
			}

			foreach(var item in layersToDraw_)
			{
				item.EndDrawing();
			}

			CoreInstance.EndDrawing();

			executing = false;

			CommitChanges();
		}
		#endregion


		internal unsafe swig.CoreScene CoreInstance { get; private set; }

		private ComponentManager<Scene, SceneComponent> componentManager_ { get; set; }

		private LinkedList<Layer> addingLayer = new LinkedList<Layer>();
		private LinkedList<Layer> removingLayer = new LinkedList<Layer>();
		private List<Layer> layersToDraw_;
		private List<Layer> layersToUpdate_;
		private bool executing = false;
	}
}
