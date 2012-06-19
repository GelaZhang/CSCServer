/* 
 * filename : mutex.h
 * author   : sundayman
 * date     : Jun 18, 2012
 * brief    : 
 */

#ifndef MUTEX_H_
#define MUTEX_H_


#include <cassert>
#ifndef WIN32
#include "pthread.h"
#endif
/**
*	\brief 简单的锁，封装了CriticalSection pthread_mutex_t
*           使用方法：
*           Mutex mutex;
*           CSimpleAutoLock autoLock(mutex);	这样就自动加解锁了，
*/
class Mutex
{
public:
	Mutex()
	{
#ifdef WIN32
		::InitializeCriticalSection(&_lock);
#else
		::pthread_mutex_init(&_mutex, NULL);
#endif
	}

	~Mutex()
	{
#ifdef WIN32
		::DeleteCriticalSection(&_lock);
#else
		::pthread_mutex_destroy(&_mutex);
#endif
	}

	Mutex(const Mutex& lock)
    {
#ifdef WIN32
        ::InitializeCriticalSection(&_lock);
#else
        ::pthread_mutex_init(&_mutex, NULL);
#endif
    }
	Mutex& operator = (const Mutex &lock)//使赋值无效
    {
        return *this;
    }
	void Lock()
	{
#ifdef WIN32
		::EnterCriticalSection(&_lock);
#else
		::pthread_mutex_lock(&_mutex);
#endif
	}

	void UnLock()
	{
#ifdef WIN32
		::LeaveCriticalSection(&_lock);
#else
		::pthread_mutex_unlock(&_mutex);
#endif
	}
#ifdef WIN32
	BOOL TestLock()
	{

		return ::TryEnterCriticalSection(&_lock);
	}
#else
	pthread_mutex_t *GetMutex()
	{
	    return &_mutex;
	}
#endif

private:
#ifdef WIN32
	CRITICAL_SECTION _lock;
#else
	pthread_mutex_t _mutex;
#endif //WIN32
};



class AutoMutex
{
	//这个类需要改进
	//1:兼容临界区
public:
	AutoMutex(Mutex &mutex):
		_mutex(mutex)
	{
		_mutex.Lock();
	}

	~AutoMutex()
	{
		_mutex.UnLock();
	}

private:

	/**
	 * \brief forbidden copy object
	 */
	AutoMutex(const AutoMutex&);
	AutoMutex& operator = (const AutoMutex&);
private:
	Mutex &_mutex;

};
#endif /* MUTEX_H_ */
