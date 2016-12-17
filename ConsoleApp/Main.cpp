
#include <iostream>
#include <string>
#include <io.h>
#include <direct.h>

struct  Vec3
{
	float x;
	float y;
	float z;
};

void changeValue(Vec3* v)
{
	(*v).x = 100;
}

std::string ToString(const char* str)
{
	return str;
}

void dir(std::string path)
{
	
	intptr_t hFile = 0;
	struct _finddata_t fileInfo;
	std::string pathName, exdName;

	// \\* 代表要遍历所有的类型
	if ((hFile = _findfirst(pathName.assign(path).append("\\*").c_str(), &fileInfo)) == -1) {
		return;
	}
	do
	{
		//判断文件的属性是文件夹还是文件
		std::cout << fileInfo.name << (fileInfo.attrib&_A_SUBDIR ? "[folder]" : "[file]") << std::endl;
	} while (_findnext(hFile, &fileInfo) == 0);
	_findclose(hFile);
	return;
	

	intptr_t Handle;
	struct _finddata_t FileInfo;
	if ((Handle = _findfirst("D:\\*.txt", &FileInfo)) == -1L)
		printf("没有找到匹配的项目\n");
	else
	{
		printf("%s\n", FileInfo.name);
		while (_findnext(Handle, &FileInfo) == 0)
			printf("%s\n", FileInfo.name);
		_findclose(Handle);
	}
}


int main()
{
	Vec3 v = { 1, 2, 3 };
	changeValue(&v);

	std::cout << "v = (" << v.x << "," << v.y << "," << v.z << ")" << std::endl;


	
	std::string string = ToString("123445");
	std::cout << string << std::endl;
	char cdir[255];
	char* path = _getcwd(cdir, 255);
	dir(path);
	return 0;
}
