﻿

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
#include "asd.Vector2DI.h"
#include "asd.Vector2DF.h"

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
namespace asd
{
	//----------------------------------------------------------------------------------
	//
	//----------------------------------------------------------------------------------
	Vector2DI::Vector2DI()
		: X(0)
		, Y(0)
	{

	}

	//----------------------------------------------------------------------------------
	//
	//----------------------------------------------------------------------------------
	Vector2DI::Vector2DI(int32_t x, int32_t y)
		: X(x)
		, Y(y)
	{

	}

	//----------------------------------------------------------------------------------
	//
	//----------------------------------------------------------------------------------
	bool Vector2DI::operator==(const Vector2DI& right)
	{
		return X == right.X && Y == right.Y;
	}

	//----------------------------------------------------------------------------------
	//
	//----------------------------------------------------------------------------------
	bool Vector2DI::operator!=(const Vector2DI& right)
	{
		return X != right.X || Y != right.Y;
	}

	//----------------------------------------------------------------------------------
	//
	//----------------------------------------------------------------------------------
	Vector2DI Vector2DI::operator-()
	{
		return Vector2DI(-X, -Y);
	}

	//----------------------------------------------------------------------------------
	//
	//----------------------------------------------------------------------------------
	Vector2DI Vector2DI::operator+(const Vector2DI& right)
	{
		return Vector2DI(X + right.X, Y + right.Y);
	}

	//----------------------------------------------------------------------------------
	//
	//----------------------------------------------------------------------------------
	Vector2DI Vector2DI::operator-(const Vector2DI& right)
	{
		return Vector2DI(X - right.X, Y - right.Y);
	}

	//----------------------------------------------------------------------------------
	//
	//----------------------------------------------------------------------------------
	Vector2DI Vector2DI::operator*(const Vector2DI& right)
	{
		return Vector2DI(X * right.X, Y * right.Y);
	}

	//----------------------------------------------------------------------------------
	//
	//----------------------------------------------------------------------------------
	Vector2DI Vector2DI::operator/(const Vector2DI& right)
	{
		return Vector2DI(X / right.X, Y / right.Y);
	}

	//----------------------------------------------------------------------------------
	//
	//----------------------------------------------------------------------------------
	Vector2DI Vector2DI::operator*(int32_t right)
	{
		return Vector2DI(X * right, Y * right);
	}

	//----------------------------------------------------------------------------------
	//
	//----------------------------------------------------------------------------------
	Vector2DI Vector2DI::operator/(int32_t right)
	{
		return Vector2DI(X / right, Y / right);
	}

	//----------------------------------------------------------------------------------
	//
	//----------------------------------------------------------------------------------
	Vector2DI& Vector2DI::operator+=(const Vector2DI& right)
	{
		X += right.X;
		Y += right.Y;
		return *this;
	}

	//----------------------------------------------------------------------------------
	//
	//----------------------------------------------------------------------------------
	Vector2DI& Vector2DI::operator-=(const Vector2DI& right)
	{
		X -= right.X;
		Y -= right.Y;
		return *this;
	}

	//----------------------------------------------------------------------------------
	//
	//----------------------------------------------------------------------------------
	Vector2DI& Vector2DI::operator*=(const Vector2DI& right)
	{
		X *= right.X;
		Y *= right.Y;
		return *this;
	}

	//----------------------------------------------------------------------------------
	//
	//----------------------------------------------------------------------------------
	Vector2DI& Vector2DI::operator/=(const Vector2DI& right)
	{
		X /= right.X;
		Y /= right.Y;
		return *this;
	}

	//----------------------------------------------------------------------------------
	//
	//----------------------------------------------------------------------------------
	Vector2DI& Vector2DI::operator*=(int32_t right)
	{
		X *= right;
		Y *= right;
		return *this;
	}

	//----------------------------------------------------------------------------------
	//
	//----------------------------------------------------------------------------------
	Vector2DI& Vector2DI::operator/=(int32_t right)
	{
		X /= right;
		Y /= right;
		return *this;
	}

	Vector2DF Vector2DI::To2DF() const
	{
		return Vector2DF(X, Y);
	}

	//----------------------------------------------------------------------------------
	//
	//----------------------------------------------------------------------------------
}
//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------