﻿
#pragma once

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
#include <asd.common.Base.h>
#include <Math/asd.Vector2DI.h>
#include "../asd.ReferenceObject.h"

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
namespace asd {
//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
/**
	@brief	メインウインドウを操作するクラス
*/
class Window
	: public IReference
{
protected:
	Window(){}
	virtual ~Window(){}
public:

	/**
		@brief	イベント処理を行う。
		@return	ウインドウが存続可能か
	*/
	virtual bool DoEvent() = 0;

	/**
		@brief	タイトルを設定する。
		@param	title	[in]	タイトル
	*/
	virtual void SetTitle( const achar* title ) = 0;

	virtual Vector2DI GetPrimaryMonitorPosition() = 0;

	virtual Vector2DI GetPrimaryMonitorSize() = 0;

	virtual Vector2DI GetWindowPosition() = 0;

	virtual void SetWindowPosition(Vector2DI position) = 0;

	virtual void ShowWindow() = 0;

	/**
		@brief	ウインドウを閉じる。
	*/
	virtual void Close() = 0;

	/**
		@brief	画面サイズを取得する。
		@return	画面サイズ
	*/
	virtual Vector2DI GetSize() const = 0;

	virtual void SetSize(Vector2DI size) = 0;

	/**
		@brief	ウインドウズの場合、ウインドウハンドルを取得する。
		@return	ウインドウハンドル
	*/
	virtual void* GetWindowHandle() const = 0;
};

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
}