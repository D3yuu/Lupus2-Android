#ifndef __INC_ETERLIB_MUTEX_H__
#define __INC_ETERLIB_MUTEX_H__

#ifdef __ANDROID__
#include "../android_compat/windows.h"
#endif

class Mutex
{
public:
	Mutex();
	~Mutex();

	void Lock();
	void Unlock();
	bool Trylock();

private:
	CRITICAL_SECTION lock;
};

#endif
