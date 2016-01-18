﻿#pragma once

#include "asd.Object2D.h"
#include "asd.DrawnObject2D.h"
#include "../../Shape/asd.Shape.h"

namespace asd
{
	/**
	@brief	図形の描画を扱うクラス。
	*/
	class GeometryObject2D : public DrawnObject2D
	{
	private:
		std::shared_ptr<CoreGeometryObject2D> m_coreObject;
		std::shared_ptr<Texture2D> texturePtr;
		std::shared_ptr<Shape> shapePtr;

		CoreObject2D* GetCoreObject() const;
		CoreDrawnObject2D* GetCoreDrawnObject() const override;

	public:
		typedef std::shared_ptr<CoreGeometryObject2D> Ptr;

		GeometryObject2D();
		virtual ~GeometryObject2D();

		/**
		@brief	描画に使用する図形を取得する。
		@return 図形
		*/
		std::shared_ptr<Shape> GetShape() const ;

		/**
		@brief	描画に使用する図形を設定する。
		@param	shape	図形
		*/
		void SetShape(std::shared_ptr<Shape> shape);

		/**
		@brief	このオブジェクトのブレンディング モードを取得する。
		@return	ブレンディングモード
		*/
		AlphaBlendMode GetAlphaBlendMode() const;

		/**
		@brief	このオブジェクトのブレンディング モードを設定する。
		@param	alphaBlend	ブレンディングモード
		*/
		void SetAlphaBlendMode(AlphaBlendMode alphaBlend);

		/**
		@brief	このオブジェクトを描画する際の中心座標を設定する。
		@param	centerPosition	描画する際の中心座標
		*/
		void SetCenterPosition(Vector2DF centerPosition);

		/**
		@brief	このオブジェクトを描画する際の中心座標を取得する。
		@return	描画する際の中心座標
		*/
		Vector2DF GetCenterPosition() const;

		/**
		@brief	このオブジェクトを描画する際のテクスチャフィルタの種類を設定する。
		@param	textureFilterType	描画する際のテクスチャフィルタの種類
		*/
		void SetTextureFilterType(TextureFilterType textureFilterType);

		/**
		@brief	このオブジェクトを描画する際のテクスチャフィルタの種類を取得する。
		@return	描画する際のテクスチャフィルタの種類
		*/
		TextureFilterType GetTextureFilterType() const;

		/**
		@brief	このオブジェクトを描画に使用するエフェクトを設定する。
		@param	texture	テクスチャ
		*/
		void SetTexture(std::shared_ptr<Texture2D> texture);

		/**
		@brief	このオブジェクトを描画に使用するエフェクトを取得する。
		@return	テクスチャ
		*/
		std::shared_ptr<Texture2D> GetTexture() const;
	};
}