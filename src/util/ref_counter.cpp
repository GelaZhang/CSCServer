
#include <assert.h>
#include "ref_counter.h"

using namespace Utility;
RefCounter::RefCounter()
{
	_ref = 0;
	_no_delete = false;
}

RefCounter::RefCounter(const RefCounter&)
{
	_ref = 0;
	_no_delete = false;
}

void RefCounter::AddRef()
{
#if defined(_WIN32)
	assert(::InterlockedExchangeAdd(static_cast<volatile LONG *>(&_ref), 0) >= 0);
	::InterlockedIncrement(static_cast<volatile LONG *>(&_ref));
#elif __GNUC__ >= 4
    int c = __sync_fetch_and_add(&_ref, 1);
    assert(c >= 0);
#else
    AutoMutex mutex(_mutex);
    assert(_ref >= 0);
    _ref ++;
#endif
}

void RefCounter::Release()
{
#if defined(_WIN32)
	assert(::InterlockedExchangeAdd((volatile LONG *)(&_ref), 0) > 0);
	if(::InterlockedDecrement(static_cast<volatile LONG *>(&_ref)) == 0 && !_no_delete)
	{
		m_bNoDelete = true;
		delete this;
	}
#elif __GNUC__ >= 4
    int c = __sync_fetch_and_sub(&_ref, 1);
    assert(c > 0);
    if(c == 1 && !_no_delete)
    {
    	_no_delete = true;
        delete this;
    }
#else
    AutoMutex mutex(_mutex);
    assert(_ref > 0);
    _ref --;
    if ( 0 == _ref && !_no_delete ) {
    	m_bNoDelete = true;
    	delete this;
    }
#endif
}

int RefCounter::GetRefCnt() const
{
#if defined(_WIN32)
	return InterlockedExchangeAdd(const_cast<volatile LONG*>(&m_lRef), 0);
#elif __GNUC__ >= 4
    return __sync_fetch_and_sub(const_cast<volatile int*>(&_ref), 0);
#else
    AutoMutex mutex(_mutex);
    return _ref;
#endif
}

void RefCounter::SetNoDelete(bool no_delete)
{
	_no_delete = no_delete;
}
