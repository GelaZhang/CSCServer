#ifndef __REFERENCE_COUNTER_H__
#define __REFERENCE_COUNTER_H__

/**
*	\file ref_counter.h
*	\brief  提供引用计数操作，需要使用引用计数的类直接继承本类，即可具备引用计数操作
*			亦可聚合本类的对象达到引用计数的目的，推荐使用继承
*			使用CSmartPtr可以达到自动对引用计数进行管理的效果
*			使用方法:1、新定义的类（NewClass）继承至RefCounter，这里新类拥有引用计数功能。
*			2、定义智能类：typedef SmartPtr<NewClass> NewClassPtr;
*			3、到此：就可随意使用CNew的指针了，如：NewClassPtr newPtr = new NewClass;
*	\remark 引用计数使用注意：
*			引用计数使用时，可能出现循环引用的情况（如： A引用B， B又引用A）。
*			出现这种情况时双方都释放不了，出现了死锁的状况，故使用引用计数的对象系统
*			应当是设计良好的树形结构或线性结构，而不应该出现闭环的状况。这是设计的职责。
*/

#include <string>
#ifdef _WIN32
#include <Windows.h>
#endif //_WIN32

#include "mutex.h"

namespace Utility
{
	class RefCounter
	{
	public:
		RefCounter();
		/**
		*	\brief 拷贝构造函数，不拷贝引用计数，引用计数从零开始
		*/
		RefCounter(const RefCounter&);
		virtual ~RefCounter()
		{
		}
		/**
		*	\brief 赋值操作不改变引用计数
		*/
		RefCounter& operator= (const RefCounter&)
		{
			return *this;
		}

		/**
		*	\brief
		*/
		virtual void AddRef();
		/**
		*	\brief
		*/
		virtual void Release();
		/**
		*	\brief
		*/
		virtual int GetRefCnt() const ;
		/**
		*	\brief 设置当引用计数减少到0时的操作方式：
		*		false表示delete掉自己，true表示不delete
		*		对象构造时默认为false,即计数减少到0时delete自己
		*/
		virtual void SetNoDelete(bool);

	protected:
#if defined(_WIN32)
    LONG _ref;
#elif __GNUC__ >= 4
    volatile int _ref;
#else
    int _ref;
    Mutex _mutex;
#endif
		bool _no_delete;

	};

	/**
	*	\brief 任何继承至RefCounter的类，可以使用该类达到智能指针的效果
	*/
	template<typename T>
	class SmartPtr
	{
	public:
		SmartPtr(T* p = NULL)
		{
			m_p = p;
			if ( m_p )
			{
				m_p->AddRef();
			}
		}
		SmartPtr(const SmartPtr<T>& r)
		{
			m_p = r.m_p;
			if ( m_p )
			{
				m_p->AddRef();
			}
		}
		~SmartPtr()
		{
			if ( m_p )
			{
				m_p->Release();
			}
		}
		SmartPtr& operator = (const SmartPtr<T> &r)
		{
			if ( r.m_p != m_p )
			{
				if ( m_p )
				{
					m_p->Release();
				}

				m_p = r.m_p;
				if ( m_p )
				{
					m_p->AddRef();
				}

			}
			return *this;
		}
		SmartPtr& operator = (T* p)
		{
			if ( m_p != p )
			{
				if ( m_p )
				{
					m_p->Release();
				}

				if ( p )
				{
					p->AddRef();
				}
				m_p = p;
			}
			return *this;
		}

		T* get() const
		{
			return m_p;
		}

		/**
		*	\brief 注意：该函数可能抛异常。当指针为空时抛异常
		*			暂时找不到在此处不抛异常的方案。
		*/
		T* operator ->() const
		{
			assert( m_p != NULL );
			if ( !m_p )
			{
				throwNullException();
			}
			return m_p;

		}

		/**
		*	\brief 注意：该函数可能抛异常。当指针为空时抛异常
		*			暂时找不到在此处不抛异常的方案。
		*/
		T& operator *() const
		{
			assert( m_p != NULL );

			if ( !m_p )
			{
				throwNullException();
			}
			return *(m_p);
		}
		/**
		*	\brief 达到使用if(x)判断指针是否为空的效果（x为CSmarPtr对象）
		*/
		operator bool() const
		{
			return m_p ? true : false;
		}

		/**
		*	\brief 实现动态类型转换
		*/
		template<typename Y>
		static SmartPtr dynamicCast( const SmartPtr<Y> &r)
		{
			return SmartPtr<T>(dynamic_cast<T*>(r.m_p));
		}

		/**
		*	\brief 实现将同一继承体系的指针类型转换成该智能指针对象
		*/
		template<typename Y>
		static SmartPtr dynamicCast( Y* p )
		{
			return SmartPtr<T>(dynamic_cast<T*>(p));
		}

	protected:
		void throwNullException() const
		{
			std::string strErrMsg =  "SmartPtr using null pointer: ";
			strErrMsg += __FILE__;
			throw  strErrMsg;
		}
	public:
		T* m_p;
	};
}

#endif//!__REFERENCE_COUNTER_H__
