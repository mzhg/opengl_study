
#include <iostream>


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

/*
int main()
{
	Vec3 v = { 1, 2, 3 };
	changeValue(&v);

	std::cout << "v = (" << v.x << "," << v.y << "," << v.z << ")" << std::endl;
	return 0;
}
*/