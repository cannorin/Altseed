﻿
//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
#include <asd.common.Base.h>
#include "asd.Engine.h"

#include "ObjectSystem/2D/asd.Layer2D.h"
#include "ObjectSystem/3D/asd.Layer3D.h"

#if _WIN32

#else
#include <dlfcn.h>
#endif

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
namespace asd
{
	//----------------------------------------------------------------------------------
	//
	//----------------------------------------------------------------------------------
	/**
	@brief	DLL情報読み取りクラス
	*/
	class DynamicLinkLibrary
	{
	private:
		mutable std::atomic<int32_t> m_reference;

#if _WIN32
		HMODULE	m_dll;
#else
		void*	m_dll;
#endif

	public:
		DynamicLinkLibrary()
			: m_reference(1)
		{
			m_dll = nullptr;
		}

		~DynamicLinkLibrary()
		{
			Reset();
		}

		void Reset()
		{
			if (m_dll != NULL)
			{
#if _WIN32
				::FreeLibrary(m_dll);
#else
				dlclose(m_dll);
#endif
				m_dll = NULL;
			}
		}

		/**
		@brief	DLLファイルを読み取る。
		@param	path	パス
		@return	成否
		*/
		bool Load(const char* path)
		{
			Reset();

#if _WIN32
			m_dll = ::LoadLibraryA(path);
#else
			m_dll = dlopen(path, RTLD_LAZY);
#endif
			if (m_dll == nullptr) return false;

			return true;
		}

		/**
		@brief	DLLから関数を読み取る。
		@param	name	関数名
		@return	関数ポインタ
		*/
		template<typename T>
		T GetProc(const char* name)
		{
#if _WIN32
			void* pProc = ::GetProcAddress(m_dll, name);
#else
			void* pProc = dlsym(m_dll, name);
#endif
			if (pProc == NULL)
			{
				return nullptr;
			}
			return (T)(pProc);
		}

		int AddRef()
		{
			std::atomic_fetch_add_explicit(&m_reference, 1, std::memory_order_consume);
			return m_reference;
		}

		int GetRef()
		{
			return m_reference;
		}

		int Release()
		{
			assert(m_reference > 0);
			bool destroy = std::atomic_fetch_sub_explicit(&m_reference, 1, std::memory_order_consume) == 1;
			if (destroy)
			{
				delete this;
				return 0;
			}

			return m_reference;
		}
	};

	struct DynamicLinkLibraryDeleter
	{
		void operator ()(DynamicLinkLibrary* p)
		{
			SafeRelease(p);
		}
	};

	//----------------------------------------------------------------------------------
	//
	//----------------------------------------------------------------------------------
	typedef int32_t(ASD_STDCALL *GetIntFunc)();

	//----------------------------------------------------------------------------------
	//
	//----------------------------------------------------------------------------------
	
	static std::shared_ptr <DynamicLinkLibrary>	g_dll = nullptr;
	static GetIntFunc g_GetGlobalRef = nullptr;
	ObjectSystemFactory* g_objectSystemFactory = nullptr;
	
	//----------------------------------------------------------------------------------
	//
	//----------------------------------------------------------------------------------
	static void ReleaseDLL()
	{
		if (g_dll != NULL)
		{
			if (g_dll->GetRef() == 1)
			{
				g_GetGlobalRef = nullptr;
				g_dll->Reset();
				g_dll.reset();
			}
			else
			{
				g_dll->Release();
			}
		}
	}

	static void ASD_STDCALL RemovedCore(Core* core)
	{
		ReleaseDLL();
	}

	bool Engine::HasDLL(const char* path)
	{
#if _WIN32
		auto dll = ::LoadLibraryA(path);
		if (dll != nullptr)
		{
			FreeLibrary(dll);
		}
		else
		{
			return false;
		}
#endif
		return true;
	}

	bool Engine::CheckDLL()
	{
#if _WIN32
		if (!HasDLL("D3DCOMPILER_43.dll"))
		{
			MessageBoxA(nullptr, "最新のDirectXEndUserRuntime？をインストールしてください。", "Error", MB_OK);
			return false;
		}
#endif
		return true;
	}

	//----------------------------------------------------------------------------------
	//
	//----------------------------------------------------------------------------------
	bool Engine::GenerateCore()
	{
		// DLLからコアを生成する。
		if (m_core == nullptr)
		{
			g_dll = std::shared_ptr<DynamicLinkLibrary>(new DynamicLinkLibrary(), DynamicLinkLibraryDeleter());

#if _WIN32
#if _DEBUG
			const char* path = "Altseed_core.Debug.dll";
#else
			const char* path = "Altseed_core.dll";
#endif
#elif __APPLE__
#if _DEBUG
			const char* path = "libAltseed_core.dylib";
#else
			const char* path = "libAltseed_core.dylib";
#endif
#else
#if _DEBUG
			const char* path = "libAltseed_core.so";
#else
			const char* path = "libAltseed_core.so";
#endif
#endif
			if (!g_dll->Load(path))
			{
				g_dll.reset();
				return false;
			}

			{
#if _WIN32
#if _WIN64
				const char* name = "CreateCore";
#else
				const char* name = "_CreateCore@0";
#endif
#else
				const char* name = "CreateCore";
#endif
				auto pProc = g_dll->GetProc<Core*(*)()>(name);
				if (pProc == NULL)
				{
					return false;
				}

				m_core = pProc();
				m_core->SetRemovedFunctionPpointer(RemovedCore);
				SafeAddRef(g_dll);
			}

			{
#if _WIN32
#if _WIN64
				const char* name = "GetGlobalReferenceCount__";
#else
				const char* name = "_GetGlobalReferenceCount__@0";
#endif
#else
				const char* name = "GetGlobalReferenceCount__";
#endif
				auto pProc = g_dll->GetProc<GetIntFunc>(name);
				if (pProc == NULL)
				{
					return false;
				}

				g_GetGlobalRef = pProc;
			}
		}

		return true;
	}


	//----------------------------------------------------------------------------------
	//
	//----------------------------------------------------------------------------------
	Core* Engine::m_core = nullptr;
	Keyboard* Engine::m_keyboard = nullptr;
	Mouse* Engine::m_mouse = nullptr;
	JoystickContainer* Engine::m_joystickContainer = nullptr;
	Log* Engine::m_logger = nullptr;
	Profiler* Engine::m_profiler = nullptr;
	LayerProfiler* Engine::m_layerProfiler = nullptr;
	Sound* Engine::m_sound = nullptr;
	Graphics* Engine::m_graphics = nullptr;
	ObjectSystemFactory* Engine::m_objectSystemFactory = nullptr;
	AnimationSystem* Engine::m_animationSyatem = nullptr;
	File* Engine::m_file = nullptr;

	std::shared_ptr<Scene>	Engine::m_currentScene;
	std::shared_ptr<Scene>	Engine::m_nextScene;
	std::shared_ptr<Scene>	Engine::m_previousScene;
	std::shared_ptr<Transition>	Engine::transition;

	//----------------------------------------------------------------------------------
	//
	//----------------------------------------------------------------------------------
	Engine::Engine()
	{
	}

	//----------------------------------------------------------------------------------
	//
	//----------------------------------------------------------------------------------
	Engine::~Engine()
	{
	}

	//----------------------------------------------------------------------------------
	//
	//----------------------------------------------------------------------------------
	bool Engine::Initialize(const achar* title, int32_t width, int32_t height, EngineOption option)
	{
		if (!CheckDLL()) return false;

		if (!GenerateCore()) return false;

		auto graphicsDeviceType = option.GraphicsDevice;
		if (graphicsDeviceType == GraphicsDeviceType::Default)
		{
#if _WIN32
			graphicsDeviceType = GraphicsDeviceType::DirectX11;
#else
			graphicsDeviceType = GraphicsDeviceType::OpenGL;
#endif
		}

		CoreOption coreOption;
		coreOption.GraphicsDevice = graphicsDeviceType;
		coreOption.IsFullScreen = option.IsFullScreen;
		coreOption.WindowPosition = option.WindowPosition;
		coreOption.IsReloadingEnabled = option.IsReloadingEnabled;

		bool init = m_core->Initialize(title, width, height, coreOption);
		if (init)
		{
			m_logger = m_core->GetLogger();
			m_profiler = m_core->GetProfiler();
			m_layerProfiler = m_core->GetLayerProfiler();
			m_objectSystemFactory = m_core->GetObjectSystemFactory();
			m_sound = m_core->GetSound();
			m_graphics = m_core->GetGraphics();
			m_animationSyatem = m_core->GetAnimationSyatem();
			m_file = m_core->GetFile();

			m_keyboard = m_core->GetKeyboard();
			m_mouse = m_core->GetMouse();
			m_joystickContainer = m_core->GetJoystickContainer();

			g_objectSystemFactory = m_objectSystemFactory;

			// 自動生成
			if (option.AutoGeneratedLayer == AutoGeneratedLayerType::Layer2D)
			{
				auto scene = std::make_shared<Scene>();
				auto layer = std::make_shared<Layer2D>();
				scene->AddLayer(layer);
				ChangeScene(scene);
			}
			else if (option.AutoGeneratedLayer == AutoGeneratedLayerType::Layer3D)
			{
				auto scene = std::make_shared<Scene>();
				auto layer = std::make_shared<Layer3D>();
				scene->AddLayer(layer);
				ChangeScene(scene);
			}
		}
		else
		{
			SafeRelease(m_core);
		}

		return init;
	}

	//----------------------------------------------------------------------------------
	//
	//----------------------------------------------------------------------------------
	bool Engine::InitializeByExternalWindow(void* handle1, void* handle2, int32_t width, int32_t height, EngineOption option)
	{
		if (!CheckDLL()) return false;

		if (!GenerateCore()) return false;

		auto graphicsDeviceType = option.GraphicsDevice;
		if (graphicsDeviceType == GraphicsDeviceType::Default)
		{
#if _WIN32
			graphicsDeviceType = GraphicsDeviceType::DirectX11;
#else
			graphicsDeviceType = GraphicsDeviceType::OpenGL;
#endif
		}

		CoreOption coreOption;
		coreOption.GraphicsDevice = graphicsDeviceType;
		coreOption.IsFullScreen = option.IsFullScreen;
		coreOption.WindowPosition = option.WindowPosition;
		coreOption.IsReloadingEnabled = option.IsReloadingEnabled;

		bool init = m_core->InitializeByExternalWindow(handle1, handle2, width, height, coreOption);
		if (init)
		{
			m_logger = m_core->GetLogger();
			m_profiler = m_core->GetProfiler();
			m_layerProfiler = m_core->GetLayerProfiler();
			m_objectSystemFactory = m_core->GetObjectSystemFactory();
			m_sound = m_core->GetSound();
			m_file = m_core->GetFile();
			m_graphics = m_core->GetGraphics();
			m_animationSyatem = m_core->GetAnimationSyatem();

			g_objectSystemFactory = m_objectSystemFactory;

			// 自動生成
			if (option.AutoGeneratedLayer == AutoGeneratedLayerType::Layer2D)
			{
				auto scene = std::make_shared<Scene>();
				auto layer = std::make_shared<Layer2D>();
				scene->AddLayer(layer);
				ChangeScene(scene);
			}
			else if (option.AutoGeneratedLayer == AutoGeneratedLayerType::Layer3D)
			{
				auto scene = std::make_shared<Scene>();
				auto layer = std::make_shared<Layer3D>();
				scene->AddLayer(layer);
				ChangeScene(scene);
			}
		}
		return init;
	}

	void Engine::SetTitle(const achar* title)
	{
		if (m_core == nullptr) return;
		m_core->SetTitle(title);
	}

	//----------------------------------------------------------------------------------
	//
	//----------------------------------------------------------------------------------
	bool Engine::DoEvents()
	{
		if (m_core == nullptr) return false;

		if (transition != nullptr)
		{
			if (transition->coreTransition->GetIsSceneChanged() && m_nextScene != nullptr)
			{
				if (m_currentScene != nullptr)
				{
					m_currentScene->CallChanging();
				}
				m_previousScene = m_currentScene;
				m_currentScene = m_nextScene;
				m_core->ChangeScene(m_nextScene->m_coreScene.get());
				m_nextScene->Start();
				m_nextScene = nullptr;
			}
			
			if (transition->coreTransition->GetIsFinished())
			{
				if (m_previousScene != nullptr)
				{
					m_previousScene->Dispose();
					m_previousScene = nullptr;
				}
				
				transition = nullptr;
				m_currentScene->CallTransitionFinished();
			}
		}
		else
		{
			if (m_nextScene != nullptr)
			{
				if (m_currentScene != nullptr)
				{
					m_currentScene->CallChanging();
					m_currentScene->Dispose();
				}
				
				m_currentScene = m_nextScene;
				m_core->ChangeScene(m_nextScene->m_coreScene.get());
				m_nextScene->Start();
				m_nextScene = nullptr;
			}
		}
		
		return m_core->DoEvents();
	}

	//----------------------------------------------------------------------------------
	//
	//----------------------------------------------------------------------------------
	void Engine::Update()
	{
		if (m_core == nullptr) return;

		m_core->BeginDrawing();

		m_layerProfiler->Refresh();

		if (m_currentScene != nullptr)
		{
			m_currentScene->Update();

			//*
			for (auto& l : m_currentScene->GetLayers())
			{
				m_layerProfiler->Record(
					l->GetName().c_str(),
					l->GetObjectCount(),
					l->GetTimeForUpdate());
			}
			//*/
		}

		if (transition != nullptr)
		{
			transition->OnUpdate();
		}
		
		if (m_currentScene != nullptr)
		{
			m_currentScene->Draw();
		}

		if (transition != nullptr)
		{
			std::shared_ptr<CoreScene> curScene;
			if (m_currentScene != nullptr)
			{
				curScene = m_currentScene->m_coreScene;
			}

			std::shared_ptr<CoreScene> prevScene;
			if (m_previousScene != nullptr)
			{
				prevScene = m_previousScene->m_coreScene;
			}

			if (transition->coreTransition->GetIsSceneChanged())
			{
				m_core->DrawSceneToWindowWithTransition(curScene.get(), prevScene.get(), transition->coreTransition.get());
			}
			else
			{
				m_core->DrawSceneToWindowWithTransition(nullptr, curScene.get(), transition->coreTransition.get());
			}
			
		}
		else
		{
			if (m_currentScene != nullptr)
			{
				m_core->DrawSceneToWindow(m_currentScene->m_coreScene.get());
			}
		}

		m_core->Draw();

		m_core->EndDrawing();
	}

	//----------------------------------------------------------------------------------
	//
	//----------------------------------------------------------------------------------
	void Engine::Terminate()
	{
		if (m_core == nullptr) return;

		if (m_currentScene != nullptr)
		{
			m_currentScene->Dispose();
		}

		if (m_nextScene != nullptr)
		{
			m_nextScene->Dispose();
		}

		if (m_previousScene != nullptr)
		{
			m_previousScene->Dispose();
		}

		m_currentScene.reset();
		m_nextScene.reset();
		m_previousScene.reset();
		transition.reset();

		m_core->Terminate();

		SafeRelease(m_core);
	}

	std::shared_ptr<Cursor> Engine::CreateCursor(const achar* path, Vector2DI hot)
	{
		auto c = m_core->CreateCursor(path, hot);
		return CreateSharedPtrWithReleaseDLL(c);
	}

	void Engine::SetCursor(std::shared_ptr<Cursor> cursor)
	{
		m_core->SetCursor(cursor.get());
	}

	const achar* Engine::GetClipboardString()
	{
		return m_core->GetClipboardString();
	}

	void Engine::SetClipboardString(const achar* s)
	{
		m_core->SetClipboardString(s);
	}

	void Engine::SetIsFullscreenMode(bool isFullscreenMode)
	{
		m_core->SetIsFullscreenMode(isFullscreenMode);
	}

	int32_t Engine::GetReferenceCount()
	{
		return g_GetGlobalRef();
	}

	bool Engine::AddObject2D(std::shared_ptr<Object2D> o)
	{
		ScenePtr scene;

		if (m_currentScene != nullptr)
		{
			scene = m_currentScene;
		}
		else if (m_nextScene != nullptr)
		{
			scene = m_nextScene;
		}
		
		if (scene == nullptr) return false;

		auto& layers = scene->GetLayers();
		
		for (auto& layer : layers)
		{
			if (layer->GetLayerType() == LayerType::Layer2D)
			{
				auto layer2d = (Layer2D*) layer.get();
				layer2d->AddObject(o);
				return true;
			}
		}

		return false;
	}

	bool Engine::RemoveObject2D(std::shared_ptr<Object2D> o)
	{
		ScenePtr scene;

		if (m_currentScene != nullptr)
		{
			scene = m_currentScene;
		}
		else if (m_nextScene != nullptr)
		{
			scene = m_nextScene;
		}

		if (scene == nullptr) return false;

		auto& layers = scene->GetLayers();

		for (auto& layer : layers)
		{
			if (layer->GetLayerType() == LayerType::Layer2D)
			{
				auto layer2d = (Layer2D*) layer.get();
				layer2d->RemoveObject(o);
				return true;
			}
		}

		return false;
	}

	bool Engine::AddObject3D(std::shared_ptr<Object3D> o)
	{
		ScenePtr scene;

		if (m_currentScene != nullptr)
		{
			scene = m_currentScene;
		}
		else if (m_nextScene != nullptr)
		{
			scene = m_nextScene;
		}

		if (scene == nullptr) return false;

		auto& layers = scene->GetLayers();

		for (auto& layer : layers)
		{
			if (layer->GetLayerType() == LayerType::Layer3D)
			{
				auto layer2d = (Layer3D*) layer.get();
				layer2d->AddObject(o);
				return true;
			}
		}

		return false;
	}

	bool Engine::RemoveObject3D(std::shared_ptr<Object3D> o)
	{
		ScenePtr scene;

		if (m_currentScene != nullptr)
		{
			scene = m_currentScene;
		}
		else if (m_nextScene != nullptr)
		{
			scene = m_nextScene;
		}

		if (scene == nullptr) return false;

		auto& layers = scene->GetLayers();

		for (auto& layer : layers)
		{
			if (layer->GetLayerType() == LayerType::Layer3D)
			{
				auto layer2d = (Layer3D*) layer.get();
				layer2d->RemoveObject(o);
				return true;
			}
		}

		return false;
	}

	//----------------------------------------------------------------------------------
	//
	//----------------------------------------------------------------------------------
	void Engine::ChangeScene(ScenePtr scene)
	{
		m_nextScene = scene;
	}

	void Engine::ChangeSceneWithTransition(std::shared_ptr<Scene> scene, const std::shared_ptr<Transition>& transition)
	{
		if (m_previousScene != nullptr)
		{
			m_previousScene->Dispose();
		}

		m_nextScene = scene;
		Engine::transition = transition;
	}

	//----------------------------------------------------------------------------------
	//
	//----------------------------------------------------------------------------------
	void Engine::TakeScreenshot(const achar* path)
	{
		m_core->TakeScreenshot(path);
	}

	void Engine::CaptureScreenAsGifAnimation(const achar* path, int32_t frame, float frequency_rate, float scale)
	{
		m_core->CaptureScreenAsGifAnimation(path, frame, frequency_rate, scale);
	}

	float Engine::GetDeltaTime()
	{
		return m_core->GetDeltaTime();
	}

	void Engine::SetDeltaTime(float deltaTime)
	{
		m_core->SetDeltaTime(deltaTime);
	}

	//----------------------------------------------------------------------------------
	//
	//----------------------------------------------------------------------------------
	float Engine::GetCurrentFPS()
	{
		return m_core->GetCurrentFPS();
	}

	//----------------------------------------------------------------------------------
	//
	//----------------------------------------------------------------------------------
	int32_t Engine::GetTargetFPS()
	{
		return m_core->GetTargetFPS();
	}

	//----------------------------------------------------------------------------------
	//
	//----------------------------------------------------------------------------------
	void Engine::SetTargetFPS(int32_t fps)
	{
		m_core->SetTargetFPS(fps);
	}

	float Engine::GetTimeSpan()
	{
		return m_core->GetTimeSpan();
	}

	void Engine::SetTimeSpan(float timeSpan)
	{
		m_core->SetTimeSpan(timeSpan);
	}

	FramerateMode Engine::GetFramerateMode()
	{
		return m_core->GetFramerateMode();
	}

	void Engine::SetFramerateMode(FramerateMode framerateMode)
	{
		m_core->SetFramerateMode(framerateMode);
	}

	void Engine::Close()
	{
		m_core->Close();
	}

	//----------------------------------------------------------------------------------
	//
	//----------------------------------------------------------------------------------
	void* Engine::GetWindowHandle()
	{
		return m_core->GetWindowHandle();
	}

	//----------------------------------------------------------------------------------
	//
	//----------------------------------------------------------------------------------
	Keyboard* Engine::GetKeyboard()
	{
		return m_keyboard;
	}

	//----------------------------------------------------------------------------------
	//
	//----------------------------------------------------------------------------------
	Mouse* Engine::GetMouse()
	{
		return m_mouse;
	}

	//----------------------------------------------------------------------------------
	//
	//----------------------------------------------------------------------------------
	Log* Engine::GetLogger()
	{
		return m_logger;
	}

	//----------------------------------------------------------------------------------
	//
	//----------------------------------------------------------------------------------
	Profiler* Engine::GetProfiler()
	{
		return m_profiler;
	}

	//----------------------------------------------------------------------------------
	//
	//----------------------------------------------------------------------------------
	JoystickContainer* Engine::GetJoystickContainer()
	{
		return m_joystickContainer;
	}

	//----------------------------------------------------------------------------------
	//
	//----------------------------------------------------------------------------------
	Graphics* Engine::GetGraphics()
	{
		return m_graphics;
	}

	//----------------------------------------------------------------------------------
	//
	//----------------------------------------------------------------------------------
	Sound* Engine::GetSound()
	{
		return m_sound;
	}

	//----------------------------------------------------------------------------------
	//
	//----------------------------------------------------------------------------------
	File* Engine::GetFile()
	{
		return m_file;
	}

	//----------------------------------------------------------------------------------
	//
	//----------------------------------------------------------------------------------
	AnimationSystem* Engine::GetAnimationSyatem()
	{
		return m_animationSyatem;
	}

	//----------------------------------------------------------------------------------
	//
	//----------------------------------------------------------------------------------
	Vector2DI Engine::GetWindowSize()
	{
		return m_core->GetWindowSize();
	}

	bool Engine::GetProfilerVisibility()
	{
		return m_core->GetProfilerVisibility();
	}

	void Engine::SetProfilerVisibility(bool visibility)
	{
		m_core->SetProfilerVisibility(visibility);
	}

	//----------------------------------------------------------------------------------
	//
	//----------------------------------------------------------------------------------
	int32_t GetGlobalReferenceCount()
	{
		if (g_GetGlobalRef == nullptr) return 0;
		auto ref = g_GetGlobalRef();
		return ref;
	}

	//----------------------------------------------------------------------------------
	//
	//----------------------------------------------------------------------------------
	void CheckDLL()
	{
		if (GetGlobalReferenceCount() == 0)
		{
			ReleaseDLL();
		}
	}

	//----------------------------------------------------------------------------------
	//
	//----------------------------------------------------------------------------------
}
