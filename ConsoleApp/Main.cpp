
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

// 将str字符以spl分割,存于dst中，并返回子字符串数量
int split(char dst[][80], char* str, const char* spl)
{
	int n = 0;
	char *result = NULL;
	result = strtok(str, spl);
	while (result != NULL)
	{
		strcpy(dst[n++], result);
		result = strtok(NULL, spl);
	}
	return n;
}

int main()
{
	Vec3 v = { 1, 2, 3 };
	changeValue(&v);

	std::cout << "v = (" << v.x << "," << v.y << "," << v.z << ")" << std::endl;

	char str[] = "what is you name?";
	char dst[10][80];
	int cnt = split(dst, str, " ");
	for (int i = 0; i < cnt; i++)
		puts(dst[i]);
	
	std::string string = ToString("123445");
	std::cout << string << std::endl;
	char cdir[255];
	char* path = _getcwd(cdir, 255);
	dir(path);
	return 0;
}
