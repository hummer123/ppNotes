/*
**posixWrapper.c - A wrapper for posix thread ,message queue , semaphore library.
**
**3/12/2017 by huguodong
**
*/

#include <stdarg.h>
#include <time.h>
#include <sys/types.h>
#include <limits.h>
#include <sched.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <sys/time.h>
#include <signal.h>
#include <pthread.h>
#include <mqueue.h>
#include <semaphore.h>
#include <sys/ioctl.h>
#include <sys/prctl.h>
#include "pwDefs.h"


#define PRINTA(fmt, args...) //printf(fmt,##args)
#define PRINT_ERR(x)	printf x
#define MAX_ARGS_NUM   10

void setPthreadName(char *name)
{
	if(name != NULL)
	{
		(void)prctl(15,(unsigned long)name);
	}
}


static void *threadWrapper2(void *arg)
{
	FUNC_WRAPPER2 func = *((FUNC_WRAPPER2 *)arg);
	free((char *)arg);

	(*(func.entry))(func.arg[0],func.arg[1],func.arg[2],func.arg[3],func.arg[4],
					func.arg[5],func.arg[6],func.arg[7],func.arg[8],func.arg[9]);
	return NULL;
}

static void *threadWrapper(void *arg)
{
	FUNC_WRAPPER func = *((FUNC_WRAPPER *)arg);
	free((char *)arg);
	(*(func.entry))(func.parms[0],func.parms[1],func.parms[2],func.parms[3],func.parms[4],
					func.parms[5],func.parms[6],func.parms[7],func.parms[8],func.parms[9]);
	return NULL;
}

static void getTimespec(int wait_ms,struct timespec *tp)
{
	time_t sec,t;
	long long nsec;

	sec = 0;
	if (wait_ms == NO_WAIT)
	{
		nsec = 0;
	}
	else
	{
		nsec = wait_ms * 1000000LL;
	}

	if (clock_gettime(CLOCK_REALTIME,tp) == -1)
	{
		PRINT("getTimespec:clock_gettime call fail , error %d(%s)\n",errno,strerror(errno));
		tp->tv_sec = time(NULL) + 1;
		tp->tv_nsec = 0;
	}
	else
	{
		t = time(NULL) + 1;
		if ((int)(tp->tv_sec - t) > 30)
		{
			tp->tv_sec = t;
			tp->tv_nsec = 0;
		}
	}
	nsec += tp->tv_nsec;
	PRINT("getTimespec: current time sec = %ld, time = %ld , nsec = %ld , total nsec = %lld\n",
		tp->tv_sec,time(NULL) + 1 , tp->tv_nsec , nsec);

	if (nsec >= 1000000000)
	{
		sec = nsec / 1000000000;
		nsec = nsec % 1000000000;
	}
	tp->tv_sec += sec;
	tp->tv_nsec = nsec;
}

int pthreadGetPriorityScope(int *minPriority,int *maxPriority)
{
	if (minPriority != NULL)
	{
		(*minPriority) = sched_get_priority_min(SCHED_RR);
		if (*minPriority == -1)
		{
			return  -1;
		}
	}

	if (maxPriority != NULL)
	{
		(*maxPriority) = sched_get_priority_max(SCHED_RR);
		if (*maxPriority == -1)
		{
			return  -1;
		}
	}
	return 0;
}

static int setPthreadAttr(pthread_attr_t *attr,int priority,size_t stacksize, int inheritsched)
{
	int rval;
	struct sched_param params;
	int maxPriority,minPriority;
	if ((inheritsched != SCHED_FIFO) && (inheritsched !=  SCHED_RR) && (inheritsched != SCHED_OTHER))
	{
		return -1;
	}
	rval = pthread_attr_init(attr);
	if (rval != 0)
	{
		return rval;
	}
	//对R2 及 Raptor 平台，使用的是NPTL线程库，需要设置下线程属性，线程优先级才能生效

#if defined(R2_PLAT) || defined(R7_HI3516A)
	rval = pthread_attr_setinheritsched(attr,PTHREAD_EXPLICIT_SCHED);
	if (rval != 0)
	{
		pthread_attr_destroy(attr);
		return rval;
	}

	rval = pthread_attr_setscope(attr,PTHREAD_SCOPE_SYSTEM);
	if (rval != 0)
	{
		if (rval == ENOTSUP)
		{
			PRINT("The system does not support the %s scope , using %s\n",
				"PTHREAD_SCOPE_SYSTEM","PTHREAD_SCOPE_PROCESS");
			rval = pthread_attr_setscope(attr,PTHREAD_SCOPE_PROCESS);
		}

		if (rval)
		{
			pthread_attr_destroy(attr);
			return rval;
		}
	}
#endif
	
	rval = pthread_attr_setschedpolicy(attr,inheritsched);
	if (rval != 0)
	{
		pthread_attr_destroy(attr);
		return rval;
	}
	//设置分离线程 自己释放资源
	rval = pthread_attr_setdetachstate(attr,PTHREAD_CREATE_DETACHED);
	if (rval != 0)
	{
		pthread_attr_destroy(attr);
		return rval;
	}
	
	rval = pthreadGetPriorityScope(&minPriority, &maxPriority);
	if ( rval != 0)
	{
		pthread_attr_destroy(attr);
		return rval;
	}

	if (priority < minPriority)
	{
		params.sched_priority = maxPriority;
	}
	else
	{
		params.sched_priority = priority;
	}
	rval = pthread_attr_setschedparam(attr,&params);
	if (rval != 0)
	{
		pthread_attr_destroy(attr);
		return rval;
	}
#if defined(RAPTOR) || defined(R2_PLAT) || defined(AM_S2) || defined(R6_S2LM) || defined(R7_HI3516A)
	if (stacksize < 2*1024*1024)
	{
		stacksize = 2*1024*1024;
	}
#else
	if (stacksize < PTHREAD_STACK_MIN)
	{
		stacksize = PTHREAD_STACK_MIN;
	}
#endif	
	rval = pthread_attr_setstacksize(attr,stacksize);
	if (rval != 0)
	{
		pthread_attr_destroy(attr);
		return rval;
	}
	return 0;
}

int pthreadSpawn(pthread_t *ptid,int priority,size_t stacksize,
				void *(*funcptr)(void *,void *,void *,void *,void *,void *,void *,void *,void *,void *),
				unsigned args, ...)
{
	int i,rval;
	void *arg[MAX_ARGS_NUM] = {0};
	START_ROUTINE start;
	pthread_t tid,*raw;
	pthread_attr_t attr;
	va_list ap;

	FUNC_WRAPPER2 *func = NULL;
	if (funcptr == NULL || args > MAX_ARGS_NUM)
	{
		return EINVAL;
	}

	va_start(ap,args);

	for(i = 0 ; i < args ; i++)
	{
		arg[i] = va_arg(ap,void *);
		PRINTA("pthreadSpawn: arg[%d] = %d\n",i,(int)arg[i]);
	}
	va_end(ap);

	rval = setPthreadAttr( &attr,  priority,  stacksize , SCHED_RR);
	if (rval != 0)
	{
		return rval;
	}
	if (ptid != NULL)
	{
		raw = ptid;
	}
	else
	{
		raw = &tid;
	}

	if (args <= 1)
	{
		rval = pthread_create(raw,&attr,(START_ROUTINE)funcptr,arg[0]);
	}
	else
	{
		func = (FUNC_WRAPPER2 *)malloc(sizeof(FUNC_WRAPPER2));
		if (func == NULL)
		{
			(void)pthread_attr_destroy(&attr);
			return ENOMEM;
		}
		start = threadWrapper2;
		memset((char *)func,0,sizeof(FUNC_WRAPPER2));
		func->entry = funcptr;
		for (i = 0 ; i < args ; i++)
		{
			func->arg[i] = arg[i];
		}
		rval = pthread_create(raw, &attr ,start , (void *)func);

		if (rval != 0)
		{
			free((char *)func);
		}
	}

	pthread_attr_destroy(&attr);
	return rval;
}
int pthread_fifo_spawn(pthread_t *ptid,int priority,size_t stacksize,
				void *(*funcptr)(void *,void *,void *,void *,void *,void *,void *,void *,void *,void *),
				unsigned args, ...)
{
	int i,rval;
	void *arg[MAX_ARGS_NUM] = {0};
	START_ROUTINE start;
	pthread_t tid,*raw;
	pthread_attr_t attr;
	va_list ap;

	FUNC_WRAPPER2 *func = NULL;
	if (funcptr == NULL || args > MAX_ARGS_NUM)
	{
		return EINVAL;
	}

	va_start(ap,args);

	for(i = 0 ; i < args ; i++)
	{
		arg[i] = va_arg(ap,void *);
		PRINTA("pthreadSpawn: arg[%d] = %d\n",i,(int)arg[i]);
	}
	va_end(ap);

	rval = setPthreadAttr( &attr,  priority,  stacksize , SCHED_FIFO);
	if (rval != 0)
	{
		return rval;
	}
	if (ptid != NULL)
	{
		raw = ptid;
	}
	else
	{
		raw = &tid;
	}

	if (args <= 1)
	{
		rval = pthread_create(raw,&attr,(START_ROUTINE)funcptr,arg[0]);
	}
	else
	{
		func = (FUNC_WRAPPER2 *)malloc(sizeof(FUNC_WRAPPER2));
		if (func == NULL)
		{
			(void)pthread_attr_destroy(&attr);
			return ENOMEM;
		}
		start = threadWrapper2;
		memset((char *)func,0,sizeof(FUNC_WRAPPER2));
		func->entry = funcptr;
		for (i = 0 ; i < args ; i++)
		{
			func->arg[i] = arg[i];
		}
		rval = pthread_create(raw, &attr ,start , (void *)func);

		if (rval != 0)
		{
			free((char *)func);
		}
	}

	pthread_attr_destroy(&attr);
	return rval;
}

pthread_t pthreadSelf()
{
	return pthread_self();
}

int pthreadIdVerify(pthread_t tid)
{
	int rval;
	rval = pthread_kill(tid,0);
	if (rval == 0)
	{
		return 0;
	}
	else
	{
		return -1;
	}
}

int pthreadSuspend(pthread_t tid)
{
	int rval = 0;
	rval = pthread_kill(tid,SIGSTOP);

	if(rval != 0)
	{
		return -1;
	}
	return 0;
}

int pthreadResume(pthread_t tid)
{
	return pthread_kill(tid,SIGCONT);
}

int pthreadCancel(pthread_t tid)
{
	return pthread_cancel(tid);
}


/******************************Mutex***************************************************/

int pthreadMutexInit(pthread_mutex_t *mutex)
{
	return pthread_mutex_init(mutex,NULL);
}

int pthreadMutexLock(pthread_mutex_t *mutex ,int wait_ms)
{
	int rval;
	struct timespec timeout;
	if (wait_ms == NO_WAIT)
	{
		rval = pthread_mutex_trylock(mutex);
	}
	else if (wait_ms == WAIT_FOREVER)
	{
		rval = pthread_mutex_lock(mutex);
	}
	else
	{
		getTimespec( wait_ms, &timeout);
		rval = pthread_mutex_timedlock(mutex,&timeout);
	}
	return rval;
}
int pthreadMutexUnlock(pthread_mutex_t *mutex)
{
	return pthread_mutex_unlock(mutex);
}

int pthreadMutexDestroy(pthread_mutex_t *mutex)
{
	return pthread_mutex_destroy(mutex);
}


/*****************************************message queue ***********************************************/

mqd_t mqOpen(const char *name , int oflag , va_list args)
{
	return mq_open(name,oflag,args);
}

int mqSend(mqd_t mqdes, const char *msq_ptr,size_t msg_len,int wait_ms, unsigned msg_prio)
{
	int rval;
	struct timespec timeout;
	if (wait_ms == WAIT_FOREVER)
	{
		rval = mq_send(mqdes,msq_ptr,msg_len,msg_prio);
	}
	else
	{
		getTimespec(wait_ms, &timeout);
		rval = mq_timedsend(mqdes,msq_ptr,msg_len,msg_prio,&timeout);
	}
	return rval;
}
ssize_t mqReceive(mqd_t mqdes,char *msg_ptr,size_t msg_len,int wait_ms,unsigned *msg_prio)
{
	ssize_t rval;
	struct timespec timeout;
	if (wait_ms == WAIT_FOREVER)
	{
		rval = mq_receive(mqdes,msg_ptr,msg_len,msg_prio);
	}
	else
	{
		getTimespec(wait_ms, &timeout);
		rval = mq_timedreceive(mqdes,msg_ptr,msg_len,msg_prio,&timeout);
	}
	return rval;
}
int mqGetattr(mqd_t mqdes,struct mq_attr *mqstat)
{
	return mq_getattr(mqdes,mqstat);
}
int mqSetattr(mqd_t mqdes,const struct mq_attr *mqstat,struct mq_attr *omqstat)
{
	return mq_setattr(mqdes,mqstat,omqstat);
}
int mqClose(mqd_t mqdes)
{
	return mq_close(mqdes);
}
int mqUnlink(const char *name)
{
	return mq_unlink(name);
}

int semInit(sem_t *sem,unsigned value)
{
	return sem_init(sem,0,value);
}

sem_t *semOpen(const char *name,int oflag,...)
{
	sem_t *rval;
	va_list args;

	va_start(args,oflag);
	rval = sem_open(name,oflag,args);
	va_end(args);

	return rval;
}

int semWait(sem_t *sem,int wait_ms)
{
	int rval;
	struct timespec timeout;
	if (wait_ms == NO_WAIT)
	{
		rval = sem_trywait(sem);
	}
	else if (wait_ms == WAIT_FOREVER)
	{
		rval = sem_wait(sem);
	}
	else
	{
		getTimespec(wait_ms,&timeout);
		rval = sem_timedwait(sem,&timeout);
	}
	return rval;
}
int semPost(sem_t *sem)
{
	return sem_post(sem);
}
int semDestroy(sem_t *sem)
{
	return sem_destroy(sem);
}

int semUnlink(const char *name)
{
	return sem_unlink(name);
}
