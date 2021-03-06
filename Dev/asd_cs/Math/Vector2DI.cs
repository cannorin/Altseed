﻿using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace asd
{
	/// <summary>
	/// 2次元ベクトル(整数)
	/// </summary>
	[System.Runtime.InteropServices.StructLayout(System.Runtime.InteropServices.LayoutKind.Sequential)]
	public struct Vector2DI
	{
		/// <summary>
		/// X
		/// </summary>
		public int X;

		/// <summary>
		/// Y
		/// </summary>
		public int Y;

		/// <summary>
		/// コンストラクタ
		/// </summary>
		/// <param name="x">X</param>
		/// <param name="y">Y</param>
		public Vector2DI(int x, int y)
		{
			X = x;
			Y = y;
		}

		/// <summary>
		/// Vector2DF型に変換する。
		/// </summary>
		/// <returns>Vector2DF型</returns>
		public Vector2DF To2DF()
		{
			return new Vector2DF(X, Y);
		}

		public bool Equals(Vector2DI other)
		{
			return X == other.X && Y == other.Y;
		}


		public static bool operator ==(Vector2DI left, Vector2DI right)
		{
			return left.X == right.X && left.Y == right.Y;
		}
		public static bool operator !=(Vector2DI left, Vector2DI right)
		{
			return left.X != right.X || left.Y != right.Y;
		}
		public static Vector2DI operator +(Vector2DI left, Vector2DI right)
		{
			return new Vector2DI(left.X + right.X, left.Y + right.Y);
		}
		public static Vector2DI operator -(Vector2DI left, Vector2DI right)
		{
			return new Vector2DI(left.X - right.X, left.Y - right.Y);
		}
		public static Vector2DI operator -(Vector2DI op)
		{
			return new Vector2DI(-op.X, -op.Y);
		}
		public static Vector2DI operator *(Vector2DI op, int scolar)
		{
			return new Vector2DI(op.X * scolar, op.Y * scolar);
		}
		public static Vector2DI operator *(int scolar, Vector2DI op)
		{
			return new Vector2DI(scolar * op.X, scolar * op.Y);
		}
		public static Vector2DI operator *(Vector2DI left, Vector2DI right)
		{
			return new Vector2DI(left.X * right.X, left.Y * right.Y);
		}
		public static Vector2DI operator /(Vector2DI op, int scolar)
		{
			return new Vector2DI(op.X / scolar, op.Y / scolar);
		}
		public static Vector2DI operator /(Vector2DI left, Vector2DI right)
		{
			return new Vector2DI(left.X / right.X, left.Y / right.Y);
		}

		/// <summary>
		/// スカラーで除算する。
		/// </summary>
		/// <param name="v1">値1</param>
		/// <param name="v2">値2</param>
		/// <returns>v1/v2</returns>
		public static Vector2DI DivideByScalar(Vector2DI v1, float v2)
		{
			var ret = new Vector2DI();
			ret.X = (int)(v1.X / v2);
			ret.Y = (int)(v1.Y / v2);
			return ret;
		}
	}
}
