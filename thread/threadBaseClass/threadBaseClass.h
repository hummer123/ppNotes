/*************************************************************************
    > File Name: threadBaseClass.h
  > Author: Evans
  > Mail: 472794445@qq.com
  > Created Time: 2017年03月12日 星期日 20时37分10秒
  > Note: 
 ************************************************************************/

#ifndef _THREADBASECLASS_H
#define _THREADBASECLASS_H
#include "pwrapper.h"
/*禁止复制基类*/
class noncopyable
{
	protected:
	   noncopyable(){}
    private:
	   noncopyable(const noncopyable&);
	   noncopyable& operator =(const noncopyable&);
};

void *thread_base_class_enter(void *arg);

class threadBaseClass:public noncopyable{
	public:
		threadBaseClass();
		threadBaseClass(int priority,size_t stacksize);
		virtual ~threadBaseClass();
		virtual int run() = 0;
		int start();
		int exit();
		int is_living();
		size_t getStackSize();
		int getPriority();
		pthread_t getThreadSelf();
		int threadSuspend();
		int threadResume();
	private:
		friend void *thread_base_class_enter(void *arg);
		int is_running;
		int p_priority;
		size_t p_stacksize;
		pthread_t tid;
};

#endif
