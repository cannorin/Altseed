﻿#pragma once

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
#include <asd.common.Base.h>
#include <Math/asd.Vector4DF.h>
#include <Math/asd.Matrix44.h>

#include "../asd.Graphics_Imp.h"
#include "../asd.DeviceObject.h"

#include <cstddef>

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
namespace asd {
	//----------------------------------------------------------------------------------
	//
	//----------------------------------------------------------------------------------
	/**
		@brief	シェーダー
	*/
	class NativeShader_Imp
		: public DeviceObject
	{
	public:

		class BindingTexture
		{
		public:
			std::shared_ptr<Texture>	TexturePtr;
			TextureWrapType				WrapType;
			TextureFilterType			FilterType;
			std::string					Name;
		};

	protected:
		uint8_t*			m_vertexConstantBuffer;
		uint8_t*			m_pixelConstantBuffer;
		Texture*			m_textureSlots[Graphics_Imp::MaxTextureCount];
		std::string			m_textureNames[Graphics_Imp::MaxTextureCount];
		TextureWrapType		textureWrapTypes[Graphics_Imp::MaxTextureCount];
		TextureFilterType	textureFilterTypes[Graphics_Imp::MaxTextureCount];

		std::map<int32_t, BindingTexture> bindingTextures;

		int32_t GetBufferSize(ConstantBufferFormat type, int32_t count);

		/**
			@note	キャッシュ用キー
		*/
		astring		m_key;

		static uint32_t CalcHash(const char* text);

	public:
		NativeShader_Imp(Graphics* graphics);
		virtual ~NativeShader_Imp();

		void SetFloat(const char* name, const float& value);

		void SetVector2DF(const char* name, const Vector2DF& value);

		void SetVector3DF(const char* name, const Vector3DF& value);

		void SetVector4DF(const char* name, const Vector4DF& value);

		void SetMatrix44(const char* name, const Matrix44& value);

		void SetFloat(int32_t id, const float& value);

		void SetVector2DF(int32_t id, const Vector2DF& value);

		void SetVector3DF(int32_t id, const Vector3DF& value);

		void SetVector4DF(int32_t id, const Vector4DF& value);

		void SetVector4DFArray(int32_t id, Vector4DF* value, int32_t count);

		void SetMatrix44(int32_t id, const Matrix44& value);

		void SetMatrix44Array(int32_t id, Matrix44* value, int32_t count);

		virtual int32_t GetConstantBufferID(const char* name) = 0;

		virtual int32_t GetTextureID(const char* name) = 0;

		virtual void SetConstantBuffer(const char* name, const void* data, int32_t size) = 0;

		virtual void SetConstantBuffer(int32_t id, const void* data, int32_t size) = 0;

		virtual void SetTexture(const char* name, Texture* texture, TextureFilterType filterType, TextureWrapType wrapType) = 0;

		virtual void SetTexture(int32_t id, Texture* texture, TextureFilterType filterType, TextureWrapType wrapType) = 0;

		virtual void AssignConstantBuffer() = 0;

		const achar* GetKey() { return m_key.c_str(); }
		void SetKey(const achar* key) { m_key = key; }

		template<typename T>
		T& GetVertexConstantBuffer()
		{
			assert(m_vertexConstantBuffer != nullptr);
			return *((T*) m_vertexConstantBuffer);
		}

		uint8_t* GetVertexConstantBuffer()
		{
			return m_vertexConstantBuffer;
		}

		template<typename T>
		T& GetPixelConstantBuffer()
		{
			assert(m_pixelConstantBuffer != nullptr);
			return *((T*) m_pixelConstantBuffer);
		}

		uint8_t* GetPixelConstantBuffer()
		{
			return m_pixelConstantBuffer;
		}

		void SetTexture(const char* name, Texture* texture, TextureFilterType filterType, TextureWrapType wrapType, int32_t index);

		bool GetTexture(char*& name, Texture*& texture, TextureFilterType& filterType, TextureWrapType& wrapType, int32_t index);

		std::map<int32_t, BindingTexture>& GetBindingTextures() { return bindingTextures; }

		/**
			@brief	ShaderConstantValueの配列から定数を設定する。
			@param	constantValues	配列の先頭ポインタ
			@param	constantValueCount	配列の個数
		*/
		void SetConstantValues(ShaderConstantValue* constantValues, int32_t constantValueCount);
	};

	//----------------------------------------------------------------------------------
	//
	//----------------------------------------------------------------------------------
}
