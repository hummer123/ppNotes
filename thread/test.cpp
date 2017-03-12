/*************************************************************************
    > File Name: test.cpp
  > Author: Evans
  > Mail: 472794445@qq.com
  > Created Time: 2017年03月11日 星期六 16时11分59秒
  > Note: 
 ************************************************************************/

#include<iostream>
#include<string>
#include<vector>
#include<set>
#include<map>
#include<algorithm>
#include <pthread.h>
#include <semaphore.h>
#include <sched.h>
#include <unistd.h>
#include <stdio.h>
#include <signal.h>
using namespace std;
void *class_thread_enter(void *agrv);

class Mythread{
	public:
		Mythread():is_running(0)
		{
			sem_init(&start_sem,0,0);
			pthread_create(&tid,NULL,class_thread_enter,this);
		}
		virtual ~Mythread()
		{
			sem_destroy(&start_sem);
			exit();
			is_running = 0;
			printf("base quit\n");
		}
	    virtual int run() = 0;
		int start()
		{
			sem_post(&start_sem);
		}
		int exit()
		{
			int ret = 0;
			/* 传递的pthread_kill的signal参数一般都是大于0的，这时系统默认或者自定义的都是有相应的处理程序。
			 * signal为0时，是一个被保留的信号，一般用这个保留的信号测试线程是否存在.
			 * pthread_kill()的返回值： 调用成功返回0， ESRCH 线程不存在， EINVAL 信号不合法
			 *
			*/
			if (0 == pthread_kill(tid,0))  
			{
				ret = pthread_cancel(tid);
			}
			return ret;
		}

	private:
		friend void *class_thread_enter(void *agrv);
		int is_running;
		pthread_t tid;
		sem_t start_sem;
};
void *class_thread_enter(void *agrv)
{
    Mythread *p = (Mythread *)agrv;
    if(p == NULL)
    {
       return (void *)(-1);
    }
    sem_wait(&(p->start_sem));
    p->is_running = 1;
    p->run();
    return NULL;
}
class Mythread_test:public Mythread{
	public:
	~Mythread_test()
	{
		printf("test quit\n");
	}
	public:
		int run()
		{
			while(1)
			{
				printf("this is a test\n");
				sleep(1);
			}
		}
};
int main()
{
	Mythread_test a;
	int b;
	a.start();

	scanf("%d",&b);

	return 0;
}
