#include <iostream>
#include "GPUFlocking.h"




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