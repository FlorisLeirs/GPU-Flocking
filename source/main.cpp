#include "pch.h"
#include <iostream>
#include "GPUFlocking.h"

#if _DEBUG
// ReSharper disable once CppUnusedIncludeDirective
#if __has_include(<vld.h>)
#include <vld.h>
#endif
#endif



int main()
{
	unsigned int width = 1200;
	unsigned int height = 1000;

	GPUFlocking* pGpuFlocking = new GPUFlocking(width , height);
	pGpuFlocking->Run();

	delete pGpuFlocking;
	pGpuFlocking = nullptr;

	return 0;
}