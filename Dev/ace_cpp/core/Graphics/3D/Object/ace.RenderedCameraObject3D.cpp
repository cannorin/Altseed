﻿
#include "ace.RenderedCameraObject3D.h"
#include "../../ace.Graphics_Imp.h"

#include "../../Resource/ace.RenderTexture2D_Imp.h"
#include "../../Resource/ace.DepthBuffer_Imp.h"

#include "../../2D/ace.PostEffectRenderer.h"

namespace ace
{

	RenderedCameraObject3DProxy::RenderedCameraObject3DProxy(Graphics* graphics)
	{
		deviceType = graphics->GetGraphicsDeviceType();
		m_postEffectRenderer = PostEffectRenderer::Create(graphics);

		m_renderTarget_FR[0] = nullptr;
		m_renderTarget_FR[1] = nullptr;

		Up = Vector3DF(0, 1, 0);
	}

	RenderedCameraObject3DProxy::~RenderedCameraObject3DProxy()
	{
		SafeRelease(m_renderTargetDiffuseColor_RT);
		SafeRelease(m_renderTargetSpecularColor_Smoothness_RT);
		SafeRelease(m_renderTargetNormalDepth_RT);
		SafeRelease(m_renderTargetAO_MatID_RT);

		SafeRelease(m_renderTargetShadow_RT);

		SafeRelease(m_renderTargetSSAO_RT);
		SafeRelease(m_renderTargetSSAO_temp_RT);

		SafeRelease(m_renderTarget_FR[0]);
		SafeRelease(m_renderTarget_FR[1]);
		SafeRelease(m_depthBuffer_RT);

		SafeRelease(m_postEffectRenderer);

		SafeRelease(m_renderTargetEnvironment);
	}

	void RenderedCameraObject3DProxy::SetWindowSize(Graphics* graphics, Vector2DI windowSize)
	{
		auto g = (Graphics_Imp*) graphics;

		if (WindowSize != windowSize)
		{
			SafeRelease(m_renderTargetDiffuseColor_RT);
			SafeRelease(m_renderTargetSpecularColor_Smoothness_RT);
			SafeRelease(m_renderTargetNormalDepth_RT);
			SafeRelease(m_renderTargetAO_MatID_RT);

			SafeRelease(m_renderTargetShadow_RT);

			SafeRelease(m_renderTargetSSAO_RT);
			SafeRelease(m_renderTargetSSAO_temp_RT);

			SafeRelease(m_renderTarget_FR[0]);
			SafeRelease(m_renderTarget_FR[1]);
			SafeRelease(m_depthBuffer_RT);

			SafeRelease(m_renderTargetEnvironment);

			m_renderTargetDiffuseColor_RT = g->CreateRenderTexture2D_Imp(windowSize.X, windowSize.Y, TextureFormat::R8G8B8A8_UNORM);
			m_renderTargetSpecularColor_Smoothness_RT = g->CreateRenderTexture2D_Imp(windowSize.X, windowSize.Y, TextureFormat::R8G8B8A8_UNORM);
			m_renderTargetNormalDepth_RT = g->CreateRenderTexture2D_Imp(windowSize.X, windowSize.Y, TextureFormat::R32G32B32A32_FLOAT);
			m_renderTargetAO_MatID_RT = g->CreateRenderTexture2D_Imp(windowSize.X, windowSize.Y, TextureFormat::R8G8B8A8_UNORM);

			m_renderTargetSSAO_RT = g->CreateRenderTexture2D_Imp(windowSize.X, windowSize.Y, TextureFormat::R8G8B8A8_UNORM);
			m_renderTargetSSAO_temp_RT = g->CreateRenderTexture2D_Imp(windowSize.X, windowSize.Y, TextureFormat::R8G8B8A8_UNORM);

			m_renderTargetShadow_RT = g->CreateRenderTexture2D_Imp(windowSize.X, windowSize.Y, TextureFormat::R8G8B8A8_UNORM);

			m_renderTarget_FR[0] = g->CreateRenderTexture2D_Imp(windowSize.X, windowSize.Y, TextureFormat::R8G8B8A8_UNORM);
			m_renderTarget_FR[1] = g->CreateRenderTexture2D_Imp(windowSize.X, windowSize.Y, TextureFormat::R8G8B8A8_UNORM);
			m_depthBuffer_RT = g->CreateDepthBuffer_Imp(windowSize.X, windowSize.Y);

			m_renderTargetEnvironment = g->CreateRenderTexture2D_Imp(windowSize.X, windowSize.Y, TextureFormat::R8G8B8A8_UNORM);
		}

		WindowSize = windowSize;
	}

	void RenderedCameraObject3DProxy::OnUpdateAsync()
	{
		if (deviceType == GraphicsDeviceType::DirectX11)
		{
			ProjectionMatrix.SetPerspectiveFovRH(
				FOV / 180.0f * 3.141592f,
				(float) WindowSize.X / (float) WindowSize.Y,
				ZNear,
				ZFar);
		}
		else
		{
			ProjectionMatrix.SetPerspectiveFovRH_OpenGL(
				FOV / 180.0f * 3.141592f,
				(float) WindowSize.X / (float) WindowSize.Y,
				ZNear,
				ZFar);
		}

		auto pos = GetGlobalPosition();
		CameraMatrix.SetLookAtRH(
			pos,
			Focus,
			Vector3DF(0, 1, 0));

		
		auto invCameraMat = (CameraMatrix).GetInverted();

		auto fov = FOV / 180.0f * 3.141592f;
		auto aspect = (float)WindowSize.X / (float)WindowSize.Y;

		float yScale = 1 / tanf(fov / 2);
		float xScale = yScale / aspect;

		ReconstructInfo1 = Vector3DF(ZNear * ZFar, ZFar - ZNear, -ZFar);
		//ReconstructInfo1 = Vector3DF(cP->ZFar - cP->ZNear, cP->ZNear, 0.0f);
		ReconstructInfo2 = Vector4DF(1.0f / xScale, 1.0f / yScale, 0.0f, 0.0f);

		Vector3DF zero;
		zero = CameraMatrix.Transform3D(zero);
		UpDir = CameraMatrix.Transform3D(Vector3DF(0, 1, 0)) - zero;
		RightDir = CameraMatrix.Transform3D(Vector3DF(1, 0, 0)) - zero;
		FrontDir = CameraMatrix.Transform3D(Vector3DF(0, 0, 1)) - zero;		
	}

	RenderTexture2D_Imp* RenderedCameraObject3DProxy::GetRenderTarget()
	{
		return m_renderTarget_FR[postEffectCount % 2];
	}

	RenderTexture2D_Imp* RenderedCameraObject3DProxy::GetAffectedRenderTarget()
	{
		return m_renderTarget_FR[0];
	}

	void RenderedCameraObject3DProxy::ApplyPostEffects(RenderingCommandHelper* helper)
	{
		for (auto& c : m_postEffectCommands_RT)
		{
			m_postEffectRenderer->ExportRenderingCommands(helper, c);
		}
	}

	RenderedCameraObject3D::RenderedCameraObject3D(Graphics* graphics)
		: RenderedObject3D(graphics)
	{
		m_values.size = Vector2DI();
		m_values.fov = 0.0f;
		m_values.zfar = 0.0f;
		m_values.znear = 0.0f;
		m_values.postEffectCount = 0;

		proxy = new RenderedCameraObject3DProxy(graphics);
	}

	RenderedCameraObject3D::~RenderedCameraObject3D()
	{
		SafeRelease(proxy);
	}

	void RenderedCameraObject3D::Flip()
	{
		RenderedObject3D::Flip();

		proxy->ZNear = m_values.znear;
		proxy->ZFar = m_values.zfar;
		proxy->FOV = m_values.fov;
		proxy->Focus = m_values.focus;
		proxy->SetWindowSize(GetGraphics(), m_values.size);

		proxy->postEffectCount = m_values.postEffectCount;

		proxy->m_postEffectCommands_RT = m_postEffectCommands;
	}

	void RenderedCameraObject3D::SetFocus(const Vector3DF& focus)
	{
		m_values.focus = focus;
	}

	void RenderedCameraObject3D::SetWindowSize(const Vector2DI& size)
	{
		m_values.size = size;
	}

	void RenderedCameraObject3D::SetFieldOfView(float fov)
	{
		m_values.fov = fov;
	}

	void RenderedCameraObject3D::SetZFar(float zfar)
	{
		m_values.zfar = zfar;
	}

	void RenderedCameraObject3D::SetZNear(float znear)
	{
		m_values.znear = znear;
	}

	void RenderedCameraObject3D::StartAddingPostEffect(int32_t postEffectCount)
	{
		m_values.postEffectCount = postEffectCount;
		m_postEffectCommands.clear();
	}

	void RenderedCameraObject3D::AddPostEffectCommand(std::shared_ptr<Material2DCommand> command)
	{
		m_postEffectCommands.push_back(command);
	}

	RenderTexture2D* RenderedCameraObject3D::GetDstForPostEffect(int32_t count)
	{
		if (m_values.postEffectCount % 2 == 0)
		{
			if (count % 2 == 1)
			{
				return proxy->m_renderTarget_FR[0];
			}
			else
			{
				return proxy->m_renderTarget_FR[1];
			}
		}
		else
		{
			if (count % 2 == 1)
			{
				return proxy->m_renderTarget_FR[1];
			}
			else
			{
				return proxy->m_renderTarget_FR[0];
			}
		}

		return nullptr;
	}

	RenderTexture2D* RenderedCameraObject3D::GetSrcForPostEffect(int32_t count)
	{
		if (m_values.postEffectCount % 2 == 0)
		{
			if (count % 2 == 1)
			{
				return proxy->m_renderTarget_FR[1];
			}
			else
			{
				return proxy->m_renderTarget_FR[0];
			}
		}
		else
		{
			if (count % 2 == 1)
			{
				return proxy->m_renderTarget_FR[0];
			}
			else
			{
				return proxy->m_renderTarget_FR[1];
			}
		}

		return nullptr;
	}
}