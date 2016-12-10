#include "FileUtil.h"
#include <fstream>

namespace jet
{
	namespace util
	{
		void FileUtil::saveStringToFile(const char* filename, const char* content)
		{
			std::ofstream fout(filename);

			fout << content;
			fout.flush();
			fout.close();
		}

		FileUtil::FileUtil()
		{
		}


		FileUtil::~FileUtil()
		{
		}
	}
}