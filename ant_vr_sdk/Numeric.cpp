#include "Numeric.h"
// #include "Util.h"
#include <sstream>

#include <string.h>
#if defined(_DEBUG) || defined(DEBUG)
#include <assert.h>
#else
#define assert(x)
#endif

namespace jet
{
	namespace util
	{

		int Numeric::parseInt(const char* str)
		{
			std::stringstream ss(str);
			int num;
			ss >> num;
			return num;
		}

		std::string Numeric::toString(int i)
		{
			std::stringstream ss;
			ss << i;
			return ss.str();
		}

		Numeric::Numeric()
		{
		}


		Numeric::~Numeric()
		{
		}

		extern "C"
		{
			void ToLowercase(char* pDst, const char* pIn, int length = -1)
			{
				if (length < 0)
				{
					length = static_cast<int>(Numeric::min(strlen(pIn), strlen(pDst)));
				}

				const char* const pEnd = pIn + length;

				while (pIn < pEnd)
				{
					if (*pIn >= 'A' && *pIn <= 'Z')
					{
						*pDst = *pIn + 32;
					}
					else
					{
						*pDst = *pIn;
					}

					pIn++;
					pDst++;
				}
			}

			void ToUppercase(char* pDst, const char* pIn, int length = -1)
			{
				if (length < 0)
				{
					length = static_cast<int>(Numeric::min(strlen(pIn), strlen(pDst)));
				}

				const char* const pEnd = pIn + length;

				while (pIn < pEnd)
				{
					if (*pIn >= 'a' && *pIn <= 'z')
					{
						*pDst = *pIn - 32;
					}
					else
					{
						*pDst = *pIn;
					}

					pIn++;
					pDst++;
				}
			}

			void debugString(const char* p)
			{
#if defined(_DEBUG) || defined(DEBUG)
				const char* pStart = p;
				int len = 0;

				while (*p)
				{
					p++;
					len++;

					assert(len);
				}
#endif
			}
		}


	}
}
