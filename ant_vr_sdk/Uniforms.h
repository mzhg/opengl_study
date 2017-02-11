#pragma once

#include <GL\glew.h>
#include <string>

template<typename T>
class UniformVar
{
public:
	
	virtual void set(const T* pData) = 0;
};

class Uniform1i : public UniformVar<GLint>
{
public:
	virtual void set(GLint x) = 0;
};


class UniformManager
{
public:
	/*
	template<typename T>
	UniformVar<T> getUniform(const std::string& name);
	*/
};