﻿
//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
#include "asd.Texture2D_Imp_GL.h"
#include "../asd.Graphics_Imp_GL.h"

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
namespace asd {
	//----------------------------------------------------------------------------------
	//
	//----------------------------------------------------------------------------------
	Texture2D_Imp_GL::Texture2D_Imp_GL(Graphics* graphics)
		: Texture2D_Imp(graphics)
		, m_texture(0)
	{
	
	}

	//----------------------------------------------------------------------------------
	//
	//----------------------------------------------------------------------------------
	Texture2D_Imp_GL::Texture2D_Imp_GL(Graphics* graphics, GLuint texture, Vector2DI size, TextureFormat format)
		: Texture2D_Imp(graphics)
		, m_texture(texture)
	{
		m_format = format;
		m_size = size;
		m_resource.resize(size.X * size.Y * ImageHelper::GetPitch(m_format));
		
		auto g = (Graphics_Imp*) GetGraphics();
		g->IncVRAM(ImageHelper::GetVRAMSize(GetFormat(), GetSize().X, GetSize().Y));
	}

	//----------------------------------------------------------------------------------
	//
	//----------------------------------------------------------------------------------
	Texture2D_Imp_GL::~Texture2D_Imp_GL()
	{
		auto g = (Graphics_Imp*) GetGraphics();
		g->DecVRAM(ImageHelper::GetVRAMSize(GetFormat(), GetSize().X, GetSize().Y));

		glDeleteTextures(1, &m_texture);
	}

	//----------------------------------------------------------------------------------
	//
	//----------------------------------------------------------------------------------
	bool Texture2D_Imp_GL::GenerateTextureFromInternal(bool isSRGB)
	{
		auto g = (Graphics_Imp*) GetGraphics();

		GLuint texture = 0;
		glGenTextures(1, &texture);


		GLCheckError();
		if (glGetError() != GL_NO_ERROR) return false;

		glBindTexture(GL_TEXTURE_2D, texture);

		if (isSRGB)
		{
			glTexImage2D(
				GL_TEXTURE_2D,
				0,
				GL_SRGB8_ALPHA8,
				m_internalTextureWidth,
				m_internalTextureHeight,
				0,
				GL_RGBA,
				GL_UNSIGNED_BYTE,
				m_internalTextureData.data());
		}
		else
		{
			glTexImage2D(
				GL_TEXTURE_2D,
				0,
				GL_RGBA,
				m_internalTextureWidth,
				m_internalTextureHeight,
				0,
				GL_RGBA,
				GL_UNSIGNED_BYTE,
				m_internalTextureData.data());
		}
		

		glBindTexture(GL_TEXTURE_2D, 0);

		if (isSRGB)
		{
			m_format = TextureFormat::R8G8B8A8_UNORM_SRGB;
		}
		else
		{
			m_format = TextureFormat::R8G8B8A8_UNORM;
		}

		if (glGetError() != GL_NO_ERROR)
		{
			glDeleteTextures(1, &texture);
			return false;
		}
		
		m_texture = texture;
		m_size.X = m_internalTextureWidth;
		m_size.Y = m_internalTextureHeight;

		InternalUnload();

		g->IncVRAM(ImageHelper::GetVRAMSize(GetFormat(), GetSize().X, GetSize().Y));
		return true;
	}

	//----------------------------------------------------------------------------------
	//
	//----------------------------------------------------------------------------------
	Texture2D_Imp_GL* Texture2D_Imp_GL::Create(Graphics_Imp_GL* graphics, uint8_t* data, int32_t size, bool isSRGB)
	{
		if (size == 0) return nullptr;

		if (ImageHelper::IsPNG(data, size))
		{
			/* ロードしてみる */
			Texture2D_Imp_GL* texture = new Texture2D_Imp_GL(graphics);
			if (!texture->InternalLoad(data, size, true))
			{
				SafeRelease(texture);
				return nullptr;
			}

			if (!texture->GenerateTextureFromInternal(isSRGB))
			{
				SafeRelease(texture);
				return nullptr;
			}

			/* 必要ないので消す */
			texture->InternalUnload();

			return texture;
		}

		return nullptr;
	}

	//----------------------------------------------------------------------------------
	//
	//----------------------------------------------------------------------------------
	Texture2D_Imp_GL* Texture2D_Imp_GL::Create(Graphics_Imp_GL* graphics, int32_t width, int32_t height, TextureFormat format, void* data)
	{
		GLuint texture = 0;
		glGenTextures(1, &texture);

		if (glGetError() != GL_NO_ERROR) return nullptr;

		glBindTexture(GL_TEXTURE_2D, texture);

		int32_t intrenalFormat_;
		int32_t format_ = GL_RGBA;
		int32_t type;
		if (format == TextureFormat::R8G8B8A8_UNORM)
		{
			intrenalFormat_ = GL_RGBA;
			type = GL_UNSIGNED_BYTE;
		}
		else if(format == TextureFormat::R32G32B32A32_FLOAT)
		{
			intrenalFormat_ = GL_RGBA32F;
			type = GL_FLOAT;
		}
		else if (format == TextureFormat::R8G8B8A8_UNORM_SRGB)
		{
			intrenalFormat_ = GL_SRGB8_ALPHA8;
			type = GL_UNSIGNED_BYTE;
		}
		else if (format == TextureFormat::R16G16_FLOAT)
		{
			intrenalFormat_ = GL_RG16F;
			format_ = GL_RG;
			type = GL_FLOAT;
		}
		else if (format == TextureFormat::R8_UNORM)
		{
#ifdef __APPLE__
			intrenalFormat_ = GL_RED;
			format_ = GL_RED;
#else
			intrenalFormat_ = GL_LUMINANCE;
			format_ = GL_LUMINANCE;
#endif
			type = GL_UNSIGNED_BYTE;
		}
		else
		{
			return nullptr;
		}

		glTexImage2D(
			GL_TEXTURE_2D,
			0,
			intrenalFormat_,
			width,
			height,
			0,
			format_,
			type,
			data);

		glBindTexture(GL_TEXTURE_2D, 0);

		if (glGetError() != GL_NO_ERROR)
		{
			glDeleteTextures(1, &texture);
			return nullptr;
		}

		GLCheckError();

		return new Texture2D_Imp_GL(graphics, texture, Vector2DI(width, height), format);
	}

	//----------------------------------------------------------------------------------
	//
	//----------------------------------------------------------------------------------
	bool Texture2D_Imp_GL::Save(const achar* path)
	{
		auto g = (Graphics_Imp_GL*) GetGraphics();
		return g->SaveTexture(path, m_texture, GetSize());
	}

	//----------------------------------------------------------------------------------
	//
	//----------------------------------------------------------------------------------
	bool Texture2D_Imp_GL::Lock(TextureLockInfomation* info)
	{
		if (info == nullptr) return false;
		if (m_resource.size() == 0) return false;

		info->pixels = &(m_resource[0]);
		info->pitch = ImageHelper::GetPitch(m_format);
		info->size = m_size;
		return true;
	}

	//----------------------------------------------------------------------------------
	//
	//----------------------------------------------------------------------------------
	void Texture2D_Imp_GL::Unlock()
	{
		auto g = (Graphics_Imp_GL*) GetGraphics();

		glBindTexture(GL_TEXTURE_2D, m_texture);

		auto format = m_format;

		int32_t intrenalFormat_;
		int32_t format_ = GL_RGBA;
		int32_t type;
		if (format == TextureFormat::R8G8B8A8_UNORM)
		{
			intrenalFormat_ = GL_RGBA;
			type = GL_UNSIGNED_BYTE;
		}
		else if (format == TextureFormat::R32G32B32A32_FLOAT)
		{
			intrenalFormat_ = GL_RGBA32F;
			type = GL_FLOAT;
		}
		else if (format == TextureFormat::R8G8B8A8_UNORM_SRGB)
		{
			intrenalFormat_ = GL_SRGB8_ALPHA8;
			type = GL_UNSIGNED_BYTE;
		}
		else if (format == TextureFormat::R16G16_FLOAT)
		{
			intrenalFormat_ = GL_RG16F;
			format_ = GL_RG;
			type = GL_FLOAT;
		}
		else if (format == TextureFormat::R8_UNORM)
		{
#ifdef __APPLE__
			intrenalFormat_ = GL_RED;
			format_ = GL_RED;
#else
			intrenalFormat_ = GL_LUMINANCE;
			format_ = GL_LUMINANCE;
#endif
			type = GL_UNSIGNED_BYTE;
		}

		m_resource_rev.resize(m_size.X * m_size.Y * ImageHelper::GetPitch(m_format));

		// 上下を逆にする。
		auto pitch = ImageHelper::GetPitch(m_format);
		for (auto y = 0; y < m_size.Y; y++)
		{
			for (auto x = 0; x < m_size.X; x++)
			{
				for (auto p = 0; p < pitch; p++)
				{
					m_resource_rev[p + (x + y * m_size.X) * pitch] = m_resource[p + (x + (m_size.Y - y - 1) * m_size.X) * pitch];
				}
			}
		}

		glTexSubImage2D(
			GL_TEXTURE_2D,
			0,
			0,
			0,
			m_size.X,
			m_size.Y,
			format_,
			type,
			m_resource_rev.data());

		glBindTexture(GL_TEXTURE_2D, 0);
	}

	//----------------------------------------------------------------------------------
	//
	//----------------------------------------------------------------------------------
	void Texture2D_Imp_GL::Reload(void* data, int32_t size)
	{
		auto g = (Graphics_Imp*) GetGraphics();
		g->DecVRAM(ImageHelper::GetVRAMSize(GetFormat(), GetSize().X, GetSize().Y));

		glDeleteTextures(1, &m_texture);

		if (!InternalLoad(data, size, false))
		{
			InternalUnload();
			return;
		}

		GenerateTextureFromInternal(m_format == TextureFormat::R8G8B8A8_UNORM_SRGB);

		InternalUnload();
	}

	//----------------------------------------------------------------------------------
	//
	//----------------------------------------------------------------------------------
}
