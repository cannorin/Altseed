﻿using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace asd.Particular
{
	class SortedList
	{
		/// <summary>
		/// 値を取得する。
		/// </summary>
		/// <typeparam name="TKey"></typeparam>
		/// <typeparam name="TValue"></typeparam>
		/// <param name="d"></param>
		/// <param name="key"></param>
		/// <returns></returns>
		/// <remarks>
		/// Javaは型推論が弱いので、型を指定する。
		/// </remarks>
		public static TValue Get<TKey, TValue>(SortedList<TKey, TValue> d, TKey key)
		{
			return d[key];
		}

		public static void Set<TKey, TValue>(SortedList<TKey, TValue> d, TKey key, TValue value)
		{
			d[key] = value;
		}
	}
}
