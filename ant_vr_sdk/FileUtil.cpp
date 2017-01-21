#include "FileUtil.h"
#include <fstream>
#include <stdio.h>
#include <io.h>
#include <direct.h>
#include "AssetLoader.h"
#include <sstream>

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

		static bool CheckFileExsit(const char* filename)
		{
			FILE *fp = NULL;
			if ((fopen_s(&fp, filename, "r") == 0) || (fp != NULL))
			{
				fclose(fp);
				return true;
			}
			else
			{
				return false;
			}
		}

		static std::string SplitFilename(const std::string& str)
		{
//			std::cout << "Splitting: " << str << '\n';
			std::size_t found = str.find_last_of("/\\");
//			std::cout << " path: " << str.substr(0, found) << '\n';
//			std::cout << " file: " << str.substr(found + 1) << '\n';
			return str.substr(found + 1);
		}

		static std::string SplitFilepath(const std::string& str)
		{
			//			std::cout << "Splitting: " << str << '\n';
			std::size_t found = str.find_last_of("/\\");
			//			std::cout << " path: " << str.substr(0, found) << '\n';
			//			std::cout << " file: " << str.substr(found + 1) << '\n';
			return str.substr(0, found);
		}

		static std::string FindFilePath(const char* filename, const char* folder, bool& founded)
		{
			std::string filepath = folder;
			filepath += '/';
			filepath += filename;
			if (CheckFileExsit(filepath.c_str()))
			{
				founded = true;
				return filepath;
			}

			intptr_t hFile = 0;
			struct _finddata_t fileInfo;
			std::string pathName;

			// \\* 代表要遍历所有的类型
			if ((hFile = _findfirst(pathName.assign(folder).append("\\*").c_str(), &fileInfo)) == -1) 
			{
				founded = false;
				return std::string();
			}

//			std::string filename_str = SplitFilename(filename);

			do
			{
				//判断文件的属性是文件夹还是文件
//				std::cout << fileInfo.name << (fileInfo.attrib&_A_SUBDIR ? "[folder]" : "[file]") << std::endl;
				if (fileInfo.attrib&_A_SUBDIR)
				{
					if (fileInfo.name[0] != '.')
					{
						std::string nextFolder = folder;
						nextFolder += "\\";
						nextFolder += fileInfo.name;
						std::string result = FindFilePath(filename, nextFolder.c_str(), founded);
						if (founded)
						{
							return result;
						}
					}
				}
				/*
				else if (filename_str == fileInfo.name)
				{
					return pathName.substr(0, pathName.length() - 1) + filename_str;
//					return pathName;
				}
				*/
			} while (_findnext(hFile, &fileInfo) == 0);
			_findclose(hFile);

			founded = false;
			return std::string();
		}

		std::string FileUtil::findFilePath(const char* filename, unsigned int count, const char** search_paths)
		{
			if (CheckFileExsit(filename))
				return filename;

			bool found = false;
			char cdir[255];
			char* path = _getcwd(cdir, 255);
			std::string path_str = path;
			path_str += filename;
			if (CheckFileExsit(path_str.c_str()))
				return path_str;

			std::string result = FindFilePath(filename, path, found);
			if (found)
			{
				return result;
			}

			for (int i = 0; i < count; i++)
			{
				std::string search_path = search_paths[i];
				search_path += '/';
//				search_path += filename;
				result = FindFilePath(filename, search_path.c_str(), found);
				if (found)
				{
					return result;
				}
			}

			for (int i = 0; i < count; i++)
			{
				std::string search_path = path;
				search_path += '/';
				search_path += search_paths[i];
				search_path += '/';
//				search_path += filename;
//				if (CheckFileExsit(search_path.c_str()))
//					return search_path;
				result = FindFilePath(filename, search_path.c_str(), found);
				if (found)
				{
					return result;
				}
			}

			return std::string();
		}

		FileUtil::FileUtil()
		{
		}


		FileUtil::~FileUtil()
		{
		}

		bool LineReader::initFromFile(const char* filename)
		{
			std::string filepath = FileUtil::findFilePath(filename);
			if (!filepath.empty())
			{
				int32_t legnth = 0;
				char* pData = AssetLoaderRead(filepath.c_str(), legnth);

				return initFromMemory(static_cast<unsigned>(legnth), pData, true);
			}

			return false;
		}

		bool LineReader::initFromMemory(unsigned length, const char* pData, bool owed)
		{
			if (length)
			{
				m_pData = pData;
				mOwed = owed;
				m_Length = length;
				mInited = true;
				m_pCursor = m_pData;
				m_pLineHead = m_pData;
			}

			return mInited;
		}

		const char*  LineReader::readLine(unsigned& length)
		{
			if (!mInited)
			{
				length = 0;
				return nullptr;
			}

			const char* pEnd = m_pData + m_Length;

			m_pLineHead = m_pCursor;
			while (m_pLineHead < pEnd && *m_pLineHead == '\n')
			{
				m_pLineHead++;
			}

			if (m_pLineHead == pEnd)
			{
				length = 0;
				return nullptr;
			}

			m_pCursor = m_pLineHead;
			while (m_pCursor < pEnd && *m_pCursor != '\n')
			{
				m_pCursor++;
			}

			length = m_pCursor - m_pLineHead;
			return m_pLineHead;
		}

		LineReader::LineReader() :mInited(false), mOwed(false), m_Length(0), m_pCursor(0), m_pData(0), m_pLineHead(0)
		{
		}

		LineReader::~LineReader()
		{
			if (mOwed)
			{
				AssetLoaderFree((char*)m_pData);
			}
		}

		CommentFilter::CommentFilter() :m_Reader(), m_bInComment(false){}

		bool CommentFilter::initFromFile(const char* filename)
		{
			return m_Reader.initFromFile(filename);
		}

		bool CommentFilter::initFromMemory(unsigned length, const char* pData, bool owed)
		{
			return m_Reader.initFromMemory(length, pData, owed);
		}

		static const char* singleComment = "//";
		static const char* pairCommentLeft = "/*";
		static const char* pairCommentRight = "*/";

		const char*  CommentFilter::readLine(unsigned& length)
		{
			unsigned legnth;
			const char* pLine = m_Reader.readLine(legnth);
			if (!pLine)
			{
				return nullptr;
			}

			m_LineBuffer.clear();
			while (parseLine(legnth, pLine))
			{
				pLine = m_Reader.readLine(legnth);
				if (!pLine)
				{
					return m_LineBuffer.c_str();
				}
			}

			return m_LineBuffer.c_str();
		}

		bool CommentFilter::parseLine(unsigned length, const char* pLine)
		{
			std::size_t fromIndex = 0;
			std::size_t singleIndex;
			std::size_t pairStart;

			std::string line(pLine, pLine + length);
#define LINE_END std::string::npos
			while (true)
			{
				if (m_bInComment)
				{
					std::size_t pairEnd = line.find(pairCommentRight);
					if (pairEnd != LINE_END)
					{  // find the "*/"
						fromIndex = pairEnd + 2;
						m_bInComment = false;
						continue;
					}
					else
					{
						// No find the "*/" continue parsing.
						return true;
					}
				}
				else
				{
					singleIndex = line.find(singleComment, fromIndex);
					pairStart = line.find(pairCommentLeft, fromIndex);

					if (singleIndex != LINE_END && pairStart != LINE_END)
					{
						if (singleIndex < pairStart)
							pairStart = LINE_END;
						else
							singleIndex = LINE_END;
					}

					if (singleIndex != LINE_END)
					{
//						lineBuffer.append(line.substring(fromIndex, singleIndex));
						m_LineBuffer += line.substr(fromIndex, singleIndex);
						return false;
					}

					if (pairStart != LINE_END)
					{
						int length = 2;
//						lineBuffer.append(line.substring(fromIndex, pairStart));
						m_LineBuffer += line.substr(fromIndex, pairStart);
						std::size_t pairEnd = line.find(pairCommentRight, pairStart + length);
						if (pairEnd != LINE_END)
						{
							fromIndex = pairEnd + length;
							continue;
						}
						else
						{
							m_bInComment = true;
							return true;  // we need a new line.
						}
					}

					// singleIndex and the pairStart are both -1. This is a pure string.
//					lineBuffer.append(line.substring(fromIndex));
					m_LineBuffer += line.substr(fromIndex);
					return false;
				}
			}

#undef LINE_END
		}

		void FileUtil::loadText(std::string& out, const char* filename, bool igoreComment, std::function<void(std::string& line)>  filter)
		{
			if (!igoreComment && !filter)
			{
				int32_t legnth = 0;
				char* pData = AssetLoaderRead(filename, legnth);
				if (pData)
				{
					out.append(pData, legnth);
					AssetLoaderFree(pData);
				}
			}
			else
			{
				std::stringstream ss;
				LineReader* pReader;
				if (igoreComment)
				{
					pReader = new CommentFilter();
				}
				else
				{
					pReader = new LineReader();
				}

				pReader->initFromFile(filename);
				const char* line;
				unsigned line_length;
				std::string line_wrapper;
				while ((line = pReader->readLine(line_length)))
				{
					line_wrapper.clear();
					line_wrapper.append(line, line_length);
					if (filter)
					{
						filter(line_wrapper);	
					}

					ss << line_wrapper;
				}

				delete pReader;
				out += ss.str();
			}
		}
	}
}