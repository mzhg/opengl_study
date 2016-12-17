
#ifndef _JET_UTIL_FILEUTIL_H
#define _JET_UTIL_FILEUTIL_H
#include <string>

namespace jet
{
	namespace util
	{
		class FileUtil
		{
		public:
			static void saveStringToFile(const char* filename, const char* content);
			static std::string findFilePath(const char* filename, unsigned int count = 0, const char** search_paths = nullptr);
		private:
			FileUtil(FileUtil&) = delete;
			FileUtil();
			~FileUtil();
		};
	}
}

#endif

