#include "gettickcount.h"

uint64_t GetTickCount()
{
//	return clock() * CLOCKS_PER_SEC;
//////////////////////////////////////////////////////////////////
	struct timeval val={0};
	gettimeofday(&val, 0);
//	std::cout << "tv_sec:" << val.tv_sec;
//	std::cout << " tv_usec:" << val.tv_usec;
//	std::cout << " tv_usec:" << val.tv_usec / 1000 << std::endl;
	return (val.tv_sec * 1000) + (val.tv_usec / 1000);
/////////////////////////////////////////////////////////////////
//	struct tms tm; 
//	return (dword)times(&tm); 
}                                                                                                                                 

