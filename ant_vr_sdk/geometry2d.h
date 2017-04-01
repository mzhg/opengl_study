#pragma once
#include "Numeric.h"

namespace jet
{
	namespace util
	{
		template<typename Type>
		struct Rectangle2D
		{
			Type X, Y;
			Type Width, Height;

			Rectangle2D():
				X(0), Y(0),
				Width(0), Height(0)
			{}

			Rectangle2D(Type x, Type y, Type w, Type h):
				X(x), Y(y), Width(w), Height(h){}
		};

		typedef Rectangle2D<int> Rectangle2i;
		typedef Rectangle2D<float> Rectangle2f;
		typedef Rectangle2D<unsigned int> Rectangle2ui;

		template<typename Type>
		bool operator == (const Rectangle2D<Type>& a, const Rectangle2D<Type>& b)
		{
			return a.X == b.X && a.Y == b.Y && a.Width == b.Width && a.Height == b.Height;
		}

		template<typename Type>
		bool operator != (const Rectangle2D<Type>& a, const Rectangle2D<Type>& b)
		{
			return a.X != b.X || a.Y != b.Y || a.Width != b.Width || a.Height != b.Height;
		}

		template<typename Type>
		struct Range
		{
			Range(Type min = (Type)0, Type max = (Type)1) : Min(min), Max(max){}
			Type Min, Max;

			Type length() const { return Max - Min; }

			bool operator==(const Range<Type>& b) const
			{
				return Min == b.Min && Max == b.Max;
			}
		};

		typedef Range<int32_t> Rangei;
		typedef Range<uint32_t> Rangeui;
		typedef Range<float> Rangef;
		typedef Range<double> Ranged;

		template<typename Type>
		bool operator == (const Range<Type>& a, const Range<Type>& b)
		{
			return a.Min == b.Min && a.Max == b.Max;
		}

		template<typename Type>
		bool operator != (const Range<Type>& a, const Range<Type>& b)
		{
			return a.Min != b.Min || a.Max != b.Max;
		}

		template<typename Type>
		Range<Type> operator + (const Range<Type>& a, Type b)
		{
			Range<Type> result;
			result.Min = a.Min + b;
			result.Max = a.Max + b;
			return result;
		}

		template<typename Type>
		Range<Type> operator + (Type b, const Range<Type>& a)
		{
			Range<Type> result;
			result.Min = a.Min + b;
			result.Max = a.Max + b;
			return result;
		}

		template<typename Type>
		Range<Type> operator - (const Range<Type>& a, Type b)
		{
			Range<Type> result;
			result.Min = a.Min - b;
			result.Max = a.Max - b;
			return result;
		}

		template<typename Type>
		Range<Type>& operator += (Range<Type>& a, Type b)
		{
			a.Min = a.Min + b;
			a.Max = a.Max + b;
			return a;
		}

		template<typename Type>
		Range<Type>& operator -= (Range<Type>& a, Type b)
		{
			a.Min = a.Min - b;
			a.Max = a.Max - b;
			return a;
		}

		template<typename Type>
		Range<Type> and(const Range<Type>& a, const Range<Type>& b)
		{
			Range<Type> out;
			out.Min = Numeric::min<Type>(a.Min, b.Min);
			out.Max = Numeric::max<Type>(a.Max, b.Max);

			return out;
		}

		template<typename Type>
		struct Size
		{
			Type Width, Height;
		};

		typedef Size<int32_t> Sizei;
	}
}