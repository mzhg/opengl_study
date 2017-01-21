#include "ShaderLoader.h"
#include "FileUtil.h"
#include "Numeric.h"
#include <set>
#include <vector>

namespace jet
{
	namespace util
	{
		static std::string GetIncludeFilename(const std::string& includeLine)
		{
			std::size_t start = Numeric::min(includeLine.find('"'), includeLine.find('<'));
			std::size_t end = Numeric::max(Numeric::min(includeLine.find('"'), std::string::npos), Numeric::min(includeLine.find('<'), std::string::npos));
			
			return includeLine.substr(start + 1, end);
		}

		static bool InternalLoadShaderFile(std::string& out, const char* filename, unsigned count, const char** search_paths, std::set<std::string>& processing)
		{
			std::string filepath = FileUtil::findFilePath(filename, count, search_paths);
			if (!filepath.empty())
			{
				if (processing.find(filepath) == processing.end())
				{
					/*
					std::vector<std::string> subIncludesFiles;
					std::vector<std::string> includeTags;
					std::string _out;
					*/

					auto lineFilter([=, &processing](std::string& line) 
					{
						const std::string tag = "#include";
						std::size_t index = line.find(tag);
						if (index != std::string::npos)
						{
							std::string fname = GetIncludeFilename(line.substr(index + tag.length()));
							line.clear();
							InternalLoadShaderFile(line, fname.c_str(), count, search_paths, processing);
						}
					});

					FileUtil::loadText(out, filepath.c_str(), true, lineFilter);
					processing.insert(filepath);
				}
				else
				{
//					out.clear();
				}

				return true;
			}

			return false;
		}

		bool ShaderLoader::load(std::string& out, const char* filename, unsigned count, const char** search_paths)
		{
			std::set<std::string> processing;
			return InternalLoadShaderFile(out, filename, count, search_paths, processing);
		}

		ShaderLoader::ShaderLoader()
		{

		}

		ShaderLoader::~ShaderLoader()
		{

		}
	}
}
