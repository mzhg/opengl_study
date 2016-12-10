
#ifndef _JET_UTIL_FILEUTIL_H
#define _JET_UTIL_FILEUTIL_H

namespace jet
{
	namespace util
	{
		class FileUtil
		{
		public:
			static void saveStringToFile(const char* filename, const char* content);

		private:
			FileUtil(FileUtil&) = delete;
			FileUtil();
			~FileUtil();
		};
	}
}

#endif

