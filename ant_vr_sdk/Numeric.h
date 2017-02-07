#pragma once

#include <string>
#include <set>
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
			static T divideAndRoundUp(T dividend, T divisor)
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

			template<typename T>
			static bool isEqual(const std::set<T>& a, const std::set<T>& b)
			{
				if (a.size() != b.size()) return false;

				auto a_it = a.begin();
				auto b_it = b.begin();

				while (a_it != a.end())
				{
					if (*a_it != *b_it)
					{
						return false;
					}

					a_it++;
					b_it++;
				}

				return true;
			}

			template<typename T, unsigned int Size>
			static bool isEqual(const T a[Size], const T b[Size])
			{
				for (unsigned i = 0; i < Size; i++)
				{
					if (a[i] != b[i])
					{
						return false;
					}
				}

				return true;
			}

		private:
			Numeric(Numeric&) = delete;
		};
	}
}


