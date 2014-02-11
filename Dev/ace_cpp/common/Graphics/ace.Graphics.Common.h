﻿
#pragma once

//----------------------------------------------------------------------------------
// Include
//----------------------------------------------------------------------------------
#include "../ace.common.Base.h"

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
namespace ace {
	//----------------------------------------------------------------------------------
	//
	//----------------------------------------------------------------------------------
	/**
	@brief	回転行列の計算順序
	*/
	enum eRotationOrder
	{
		ROTATION_ORDER_QUATERNION = 10,
		ROTATION_ORDER_XZY = 11,
		ROTATION_ORDER_XYZ = 12,
		ROTATION_ORDER_ZXY = 13,
		ROTATION_ORDER_ZYX = 14,
		ROTATION_ORDER_YXZ = 15,
		ROTATION_ORDER_YZX = 16,
		ROTATION_ORDER_AXIS = 18,
		ROTATION_ORDER_MAX = 0xfffffff
	};

	//----------------------------------------------------------------------------------
	//
	//----------------------------------------------------------------------------------
}