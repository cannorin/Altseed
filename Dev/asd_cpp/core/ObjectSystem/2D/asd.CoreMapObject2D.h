﻿#pragma once
#include "../common/Math/asd.Vector2DF.h"
#include "../common/Graphics/asd.Color.h"
#include "asd.CoreChip2D.h"
#include "asd.CoreObject2D.h"

namespace asd
{

	class CoreMapObject2D
		: public CoreObject2D
	{
	public:
		CoreMapObject2D() {}
		virtual ~CoreMapObject2D() {}

		/**
		@brief	このオブジェクトの原点位置を取得する。この位置が、描画する際の描画・拡縮・回転の中心となる。
		*/
		virtual Vector2DF GetCenterPosition() const = 0;

		/**
		@brief	このオブジェクトの描画優先度を取得します。
		*/
		virtual int GetDrawingPriority() const = 0;

		/**
		@brief	このオブジェクトの原点位置を設定する。この位置が、描画する際の描画・拡縮・回転の中心となる。
		*/
		virtual void SetCenterPosition(Vector2DF position) = 0;

		/**
		@brief	このオブジェクトの描画優先度を設定する。
		*/
		virtual void SetDrawingPriority(int priority) = 0;

		/**
		@brief	このオブジェクトに描画チップを追加する。
		*/
		virtual bool AddChip(CoreChip2D* chip) = 0;

		/**
		@brief	このオブジェクトから描画チップを削除する。
		*/
		virtual bool RemoveChip(CoreChip2D* chip) = 0;

		/**
		@brief	このオブジェクトに追加されている描画チップを全て削除する。
		*/
		virtual void Clear() = 0;

	};
}