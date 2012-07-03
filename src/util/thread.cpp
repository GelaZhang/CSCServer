// **********************************************************************
//
// Copyright (c) 2003-2010 ZeroC, Inc. All rights reserved.
//
// This copy of Ice is licensed to you under the terms described in the
// ICE_LICENSE file included in this distribution.
//
// **********************************************************************

#include "thread.h"

#include <time.h>
#include <climits>
#include <exception>
#include <iostream>
#ifndef _WIN32
    #include <sys/time.h>
    #include <sys/resource.h>
#endif

using namespace std;

#ifdef _WIN32

Utility::ThreadControl::ThreadControl() :
    _handle(0),
    _id(GetCurrentThreadId())
{
}

Utility::ThreadControl::ThreadControl(HANDLE handle, Utility::ThreadControl::ID id) :
    _handle(handle),
    _id(id)
{
}

bool
Utility::ThreadControl::operator==(const ThreadControl& rhs) const
{
    return _id == rhs._id;
}

bool
Utility::ThreadControl::operator!=(const ThreadControl& rhs) const
{
    return _id != rhs._id;
}

void
Utility::ThreadControl::join()
{
    if(_handle == 0)
    {
        assert(0);
        return;
    }

    DWORD rc = WaitForSingleObject(_handle, INFINITE);
    if(rc != WAIT_OBJECT_0)
    {
        assert(0);
        return;
    }
    
    detach();
}

void
Utility::ThreadControl::detach()
{
    if(_handle == 0)
    {
        assert(0);
        return;
    }
    
    if(CloseHandle(_handle) == 0)
    {
        assert(0);
        return;
    }
}

Utility::ThreadControl::ID
Utility::ThreadControl::id() const
{
    return _id;
}

void
Utility::ThreadControl::sleep(size_t timeout)
{
    Sleep(timeout);
}

void
Utility::ThreadControl::yield()
{
    //
    // A value of zero causes the thread to relinquish the remainder
    // of its time slice to any other thread of equal priority that is
    // ready to run.
    //
    Sleep(0);
}

Utility::Thread::Thread() :
    _started(false),
    _running(false),
    _handle(0),
    _id(0)
{
}

Utility::Thread::Thread(const string& name) :
    _name(name),
    _started(false),
    _running(false),
    _handle(0),
    _id(0)
{
}

Utility::Thread::~Thread()
{
}

static unsigned int
WINAPI startHook(void* arg)
{
    // Ensure that the thread doesn't go away until run() has
    // completed.
    //
    Utility::ThreadPtr thread;

    try
    {
        Utility::Thread* rawThread = static_cast<Utility::Thread*>(arg);

        //
        // Ensure that the thread doesn't go away until run() has
        // completed.
        //
        thread = rawThread;

#ifdef _WIN32
        //
        // Initialize the random number generator in each thread on
        // Windows (the rand() seed is thread specific).
        //
        unsigned int seed = time(0);
        srand(seed ^ thread->getThreadControl().id());
#endif

        //
        // See the comment in Utility::Thread::start() for details.
        //
        rawThread->Release();
        thread->run();
    }
    catch(...)
    {
        if(!thread->name().empty())
        {
            cerr << thread->name() << " terminating" << endl;
        }
#if defined(_MSC_VER) && (_MSC_VER < 1300)
        terminate();
#else
	std::terminate();
#endif
    }

    thread->_done();
   
    return 0;
}

#include <process.h>

Utility::ThreadControl
Utility::Thread::start(size_t stackSize)
{
    return start(stackSize, THREAD_PRIORITY_NORMAL);
}

Utility::ThreadControl
Utility::Thread::start(size_t stackSize, int priority)
{
    //
    // Keep this alive for the duration of start
    //
    Utility::ThreadPtr keepMe = this;

    AutoMutex lock(_stateMutex);

    if(_started)
    {
        assert(0);
        return ThreadControl(_handle, _id);
    }

    //
    // It's necessary to increment the reference count since
    // pthread_create won't necessarily call the thread function until
    // later. If the user does (new MyThread)->start() then the thread
    // object could be deleted before the thread object takes
    // ownership. It's also necessary to increment the reference count
    // prior to calling pthread_create since the thread itself calls
    // __decRef().
    //
    AddRef();
    
    unsigned int id;
    _handle = 
        reinterpret_cast<HANDLE>(
            _beginthreadex(0, 
                            static_cast<unsigned int>(stackSize), 
                            startHook, this, CREATE_SUSPENDED, &id));
    _id = id;

    if(_handle == 0)
    {
        Release();
        throw exception("create thread fail");
    }
    if(SetThreadPriority(_handle, priority) == 0)
    {
        Release();
        throw exception("SetThreadPriority fail");
    }
    if(ResumeThread(_handle) == -1)
    {
        Release();
        throw exception("ResumeThread fail");
    }

    _started = true;
    _running = true;
    
    return ThreadControl(_handle, _id);
}

Utility::ThreadControl
Utility::Thread::getThreadControl() const
{
    AutoMutex lock(_stateMutex);
    if(!_started)
    {
        throw exception("thread have not started");
    }
    return ThreadControl(_handle, _id);
}

bool
Utility::Thread::operator==(const Thread& rhs) const
{
    return this == &rhs;
}

bool
Utility::Thread::operator!=(const Thread& rhs) const
{
    return this != &rhs;
}

bool
Utility::Thread::operator<(const Thread& rhs) const
{
    return this < &rhs;
}

bool
Utility::Thread::isAlive() const
{
    AutoMutex lock(_stateMutex);
    return _running;
}

void
Utility::Thread::_done()
{
    AutoMutex lock(_stateMutex);
    _running = false;
}

const string&
Utility::Thread::name() const
{
    return _name;
}

#else

Utility::ThreadControl::ThreadControl(pthread_t thread) :
    _thread(thread),
    _detachable(true)
{
}

Utility::ThreadControl::ThreadControl() :
    _thread(pthread_self()),
    _detachable(false)
{
}

bool
Utility::ThreadControl::operator==(const ThreadControl& rhs) const
{
    return pthread_equal(_thread, rhs._thread) != 0;
}

bool
Utility::ThreadControl::operator!=(const ThreadControl& rhs) const
{
    return !operator==(rhs);
}

void
Utility::ThreadControl::join()
{
    if(!_detachable)
    {
        assert(0);
        return;
    }

    void* ignore = 0;
    int rc = pthread_join(_thread, &ignore);
    if(rc != 0)
    {
        assert(0);
    }
}

void
Utility::ThreadControl::detach()
{
    if(!_detachable)
    {
    	assert(0);
    	return;
    }

    int rc = pthread_detach(_thread);
    if(rc != 0)
    {
    	assert(0);
    }
}

Utility::ThreadControl::ID
Utility::ThreadControl::id() const
{
    return _thread;
}

void
Utility::ThreadControl::sleep(size_t timeout)
{
    struct timespec ts;
    ts.tv_sec = timeout / 1000;
    ts.tv_nsec = (timeout % 1000 ) * 1000 * 1000L;
    nanosleep(&ts, 0);
}

void
Utility::ThreadControl::yield()
{
    sched_yield();
}

Utility::Thread::Thread() :
    _started(false),
    _running(false)
{
}

Utility::Thread::Thread(const string& name) :
    _name(name),
    _started(false),
    _running(false)
{
}

Utility::Thread::~Thread()
{
}

extern "C" 
{
static void*
startHook(void* arg)
{
    //
    // Ensure that the thread doesn't go away until run() has
    // completed.
    //
    Utility::ThreadPtr thread;

    try
    {
        Utility::Thread* rawThread = static_cast<Utility::Thread*>(arg);

        thread = rawThread;

        //
        // See the comment in Utility::Thread::start() for details.
        //
        rawThread->Release();
        thread->run();
    }
    catch(...)
    {
        if(!thread->name().empty())
        {
            cerr << thread->name() << " terminating" << endl;
        }
        std::terminate();
    }

    thread->_done();
    
    return 0;
}
}


Utility::ThreadControl
Utility::Thread::start(size_t stackSize)
{
    return start(stackSize, false, 0);
}

Utility::ThreadControl
Utility::Thread::start(size_t stackSize, int priority)
{
    return start(stackSize, true, priority);
}
Utility::ThreadControl
Utility::Thread::start(size_t stackSize, bool realtimeScheduling, int priority)
{
    //
    // Keep this alive for the duration of start
    //
    Utility::ThreadPtr keepMe = this;

    AutoMutex lock(_stateMutex);

    if(_started)
    {
    	assert(0);
        throw exception();
    }

    //
    // It's necessary to increment the reference count since
    // pthread_create won't necessarily call the thread function until
    // later. If the user does (new MyThread)->start() then the thread
    // object could be deleted before the thread object takes
    // ownership. It's also necessary to increment the reference count
    // prior to calling pthread_create since the thread itself calls
    // __decRef().
    //
    AddRef();

    pthread_attr_t attr;
    int rc = pthread_attr_init(&attr);
    if(rc != 0)
    {
    	Release();
        pthread_attr_destroy(&attr);
        assert(0);
        throw exception();
    }
    if(stackSize > 0)
    {
        if(stackSize < PTHREAD_STACK_MIN)
        {
            stackSize = PTHREAD_STACK_MIN;
        }
#ifdef __APPLE__
        if(stackSize % 4096 > 0)
        {
            stackSize = stackSize / 4096 * 4096 + 4096;
        }
#endif
        rc = pthread_attr_setstacksize(&attr, stackSize);
        if(rc != 0)
        {
            Release();
            pthread_attr_destroy(&attr);
            assert(0);
            throw exception();

        }
    }

    if(realtimeScheduling)
    {
        rc = pthread_attr_setschedpolicy(&attr, SCHED_RR);
        if(rc != 0)
        {
        	Release();
            assert(0);
            throw exception();

        }
        sched_param param;
        param.sched_priority = priority;
        rc = pthread_attr_setschedparam(&attr, &param);
        if(rc != 0)
        {
            Release();
            pthread_attr_destroy(&attr);
            assert(0);
            throw exception();

        }
        pthread_attr_setinheritsched(&attr, PTHREAD_EXPLICIT_SCHED);
    }
    rc = pthread_create(&_thread, &attr, startHook, this);
    pthread_attr_destroy(&attr);
    if(rc != 0)
    {
        Release();
        assert(0);
        throw exception();

    }

    _started = true;
    _running = true;
    return ThreadControl(_thread);
}

Utility::ThreadControl
Utility::Thread::getThreadControl() const
{
	AutoMutex lock(_stateMutex);
    if(!_started)
    {
        assert(0);
        throw exception();

    }
    return ThreadControl(_thread);
}

bool
Utility::Thread::operator==(const Thread& rhs) const
{
    return this == &rhs;
}

bool
Utility::Thread::operator!=(const Thread& rhs) const
{
    return this != &rhs;
}

bool
Utility::Thread::operator<(const Thread& rhs) const
{
    return this < &rhs;
}

bool
Utility::Thread::isAlive() const
{
    AutoMutex lock(_stateMutex);
    return _running;
}

void
Utility::Thread::_done()
{
    AutoMutex lock(_stateMutex);
    _running = false;
}

const string&
Utility::Thread::name() const
{
    return _name;
}

#endif
