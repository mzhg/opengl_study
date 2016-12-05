#pragma once

#include <string>
namespace jet
{
	namespace util
	{
		class Numeric
		{
		public:
			Numeric();
			~Numeric();

			static int parseInt(const char* str);
			static std::string toString(int i);
			
			template<typename T>
			static T max(T a, T b) { return a > b ? a : b; }
			template<typename T>
			static T min(T a, T b) { return a < b ? a : b; }

			/** Divides two integers and rounds up */
			template<typename T>
			static int divideAndRoundUp(int dividend, int divisor)
			{
				return (dividend + divisor - 1) / divisor;
			}

			template<typename T>
			static int indexOf(int length,const T* array, T value)
			{
				for (int i = 0; i < length; i++)
				{
					if (array[i] == value)
					{
						return i;
					}
				}

				return -1;
			}

		private:
			Numeric(Numeric&) = delete;
		};
	}
}


