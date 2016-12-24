#ifndef _COUNTER_H_
#define _COUNTER_H_

/*
功能:计数器
调用者：TestSSE.cpp
*/

#include <time.h>  
#include <stdio.h>  

#ifdef WIN32  || WIN64

#include <windows.h>  
#define timing_t double  
_LARGE_INTEGER g_startTime, g_stopTime;
double g_dqFreq;

inline void startTiming()
{
	_LARGE_INTEGER f;
	QueryPerformanceFrequency(&f);
	g_dqFreq = (double)f.QuadPart;

	QueryPerformanceCounter(&g_startTime);
}

// unit: ms  
inline timing_t stopTiming()
{
	QueryPerformanceCounter(&g_stopTime);
	return ((double)(g_stopTime.QuadPart - g_startTime.QuadPart) / g_dqFreq * 1000);
}

inline timing_t stopWithPrintTiming()
{
	timing_t timing;
	QueryPerformanceCounter(&g_stopTime);
	timing = ((double)(g_stopTime.QuadPart - g_startTime.QuadPart) / g_dqFreq * 1000);
	printf("Elapsed Timing(ms) : %.3lf\n\n", timing);

	return timing;
}

#else  
#include <unistd.h>  
typedef unsigned long long int64;
#define timing_t int64  
#if defined(__i386__)  
inline int64 GetCycleCount() {
	int64 result;
	__asm__ __volatile__("rdtsc" : "=A" (result));
	return result;
}
#elif defined(__x86_64__)  
inline int64 GetCycleCount()
{
	int64 hi, lo;
	__asm__ __volatile__("rdtsc" : "=a"(lo), "=d"(hi));
	return ((int64)lo) | (((int64)hi) << 32);
}
#endif  

int64 ticks_start, ticks_end;

inline void startTiming()
{
	ticks_start = GetCycleCount();
}

// unit: cycles  
inline int64 stopTiming()
{
	ticks_end = GetCycleCount();
	return (ticks_end - ticks_start);
}

inline int64 stopWithPrintTiming()
{
	int64 timing;
	ticks_end = GetCycleCount();
	timing = (ticks_end - ticks_start);
	printf("----------Elapsed Timing(Cycles) : %llu\n", timing);
	printf("----------------------------------------\n");

	return timing;
}
#endif  

// unit: ms  
inline void wait(int ms)
{
#ifdef WIN32  
	Sleep(ms);
#else  
	usleep(ms * 1000);
#endif  
}

#endif