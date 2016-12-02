#include "Numeric.h"

#include <sstream>
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
	}
}
