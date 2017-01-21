
#ifndef _JET_UTIL_FILEUTIL_H
#define _JET_UTIL_FILEUTIL_H
#include <string>
#include <functional>

namespace jet
{
	namespace util
	{
		class FileUtil
		{
		public:
			static void saveStringToFile(const char* filename, const char* content);
			static std::string findFilePath(const char* filename, unsigned int count = 0, const char** search_paths = nullptr);
			static void loadText(std::string& out, const char* filename, bool igoreComment = false, std::function<void(std::string& line)> filter = nullptr/*[](std::string& line){}*/);
		private:
			FileUtil(FileUtil&) = delete;
			FileUtil();
			~FileUtil();
		};

		class LineReader
		{
		public:
			LineReader();

			bool initFromFile(const char* filename);
			bool initFromMemory(unsigned length, const char* pData, bool owed = false);

			virtual const char*  readLine(unsigned& length);
			~LineReader();

		private:
			bool mInited;
			bool mOwed;
			const char* m_pData;
			const char* m_pCursor;
			const char* m_pLineHead;
			unsigned m_Length;
		};

		class CommentFilter : public LineReader
		{
		public:
			CommentFilter();

			bool initFromFile(const char* filename);
			bool initFromMemory(unsigned length, const char* pData, bool owed = false);

			const char*  readLine(unsigned& length) override;

			virtual ~CommentFilter(){}

		private:

			bool parseLine(unsigned length, const char* pLine);

		private:
			LineReader m_Reader;
			std::string m_LineBuffer;
			bool m_bInComment;
		};
	}
}

#endif

