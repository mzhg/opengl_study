#include "FileUtil.h"
#include <fstream>
#include <stdio.h>
#include <io.h>
#include <direct.h>

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
				return true;
			else
				return false;
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
	}
}