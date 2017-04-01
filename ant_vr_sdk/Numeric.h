#pragma once

#include <stdint.h>
#include <string>
#include <set>
#include <vector>

namespace jet
{
	namespace util
	{
		class Numeric
		{
		public:
			Numeric();
			~Numeric();

			static const unsigned ONE_MB = 1024 * 1024;

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
			static bool isEqual(const T (&a)[Size], const T (&b)[Size])
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

			// The type must implement the 'operator bool ()' function.
			template<typename T>
			static unsigned arraySize(T* a)
			{
				if (a == nullptr)
					return 0;

				T* c = a;
				unsigned count = 0;
				while (*c)
				{
					count++;
					c++;
				}

				return count;
			}

			template<typename T>
			static int indexOf(const std::vector<T>& array, const T value)
			{
				for (int i = 0; i < array.size(); i++)
				{
					if (array[i] == value)
					{
						return i;
					}
				}

				return -1;
			}

			template<typename T>
			static bool remove(std::vector<T>& array, const T value)
			{
				int index = indexOf(array, value);
				if (index >= 0)
				{
					array.erase(array.begin() + index);
					return true;
				}

				return false;
			}

			template<typename T>
			static int indexOfRef(const std::vector<T>& array, const T& value)
			{
				for (size_t i = 0; i < array.size(); i++)
				{
//					const T& left = array[i];
					if (array[i] == value)
					{
						return static_cast<int>(i);
					}
				}

				return -1;
			}

			template<typename T>
			static bool removeRef(std::vector<T>& array, const T& value)
			{
				int index = indexOfRef(array, value);
				if (index >= 0)
				{
					array.erase(array.begin() + index);
					return true;
				}

				return false;
			}

			template<typename T>
			static float ratio(T a, T b)
			{
				return static_cast<float>(a) / static_cast<float>(b);
			}

		private:
			Numeric(Numeric&) = delete;
		};
	}
}


