/*************************************************************************
    > File Name: threadBaseClass.cpp
  > Author: Evans
  > Mail: 472794445@qq.com
  > Created Time: 2017年03月12日 星期日 20时54分02秒
  > Note: 
 ************************************************************************/

#include<iostream>
#include<string>
#include "threadBaseClass.h"
using namespace std;

threadBaseClass::threadBaseClass():is_running(0),p_priority(80),p_stacksize(32768)
{
	p_priority = 80;
	p_stacksize = 32768;
}
threadBaseClass::threadBaseClass(int priority,size_t stacksize):is_running(0),p_priority(priority),p_stacksize(stacksize)
{
}
threadBaseClass::~threadBaseClass()
{
	exit();	
}
int threadBaseClass::start()
{
	int ret = pthreadSpawn(&tid,p_priority,p_stacksize,(START_ROUTINE)thread_base_class_enter,1,this);
	if (ret == 0)
	{
		is_running = 1;
	}
	return ret;
}
int threadBaseClass::exit()
{
	int ret = 0;
	if (is_running && 0 == pthreadIdVerify(tid)) 
	{
		ret = pthreadCancel(tid);
		if (0 == ret)
		{
			is_running = 0;
		}
	}
	return ret;
}
int threadBaseClass::is_living()
{
	return is_running;
}
size_t threadBaseClass::getStackSize()
{
	return p_stacksize;
}
int threadBaseClass::getPriority()
{
	return p_priority;
}
pthread_t threadBaseClass::getThreadSelf()
{
	return tid;
}
int threadBaseClass::threadSuspend()
{
	return pthreadSuspend(tid);
}
int threadBaseClass::threadResume()
{
	return pthreadSuspend(tid);
}

void *thread_base_class_enter(void *arg)
{
	threadBaseClass *p = (threadBaseClass *)arg;
	if(p == NULL)
    {
		p->is_running = 0;
        return (void *)(-1);
    }

	p->run();

	p->is_running = 0;
	return NULL;
}
