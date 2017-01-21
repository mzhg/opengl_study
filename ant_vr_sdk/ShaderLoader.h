#pragma once

#include <string>

namespace jet
{
	namespace util
	{
		class ShaderLoader
		{
		public:
			ShaderLoader();
			~ShaderLoader();

			static bool load(std::string& out, const char* filename, unsigned count = 0, const char** search_paths = nullptr);
		};
	}
}

