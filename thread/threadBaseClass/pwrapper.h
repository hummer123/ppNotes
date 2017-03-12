#ifndef __PWRAPPER_H__
#define __PWRAPPER_H__
#ifdef __cplusplus
extern "C"{
#endif
#include <pthread.h>
#include <fcntl.h>           /* For O_* constants */
#include <sys/stat.h>        /* For mode constants */
#include <mqueue.h>
#include <semaphore.h>
#include "pwDefs.h"

#define MQ_PRIO_NORMAL  (0)
#define MQ_PRIO_URGENT  (1)

#define SEM_EMPTY (0)
#define SEM_FULL  (1)

void setPthreadName(char *name);

int pthreadGetPriorityScope(int *minPriority ,int *maxPriority);

int pthreadSpawn(pthread_t *ptid , int priority,size_t stacksize,void *(*funcptr)(void *),unsigned args,...);

int pthread_fifo_spawn(pthread_t *ptid , int priority,size_t stacksize,void *funcptr,unsigned args,...);


pthread_t pthreadSelf();

int pthreadIdVerify(pthread_t tid);
int pthreadSuspend(pthread_t tid);
int pthreadResume(pthread_t tid);
int pthreadCancel(pthread_t tid);

int pthreadMutexInit(pthread_mutex_t *mutex);
int pthreadMutexLock(pthread_mutex_t *mutex ,int wait_ms);
int pthreadMutexUnlock(pthread_mutex_t *mutex);
int pthreadMutexDestroy(pthread_mutex_t *mutex);

mqd_t mqOpen(const char *name , int oflag , ...);
int mqSend(mqd_t mqdes, const char *msq_ptr,size_t msg_len,int wait_ms,unsigned msg_prio);
ssize_t mqReceive(mqd_t mqdes,char *msg_ptr,size_t msg_len,int wait_ms,unsigned *msg_prio);
int mqGetattr(mqd_t mqdes,struct mq_attr *mqstat);
int mqSetattr(mqd_t mqdes,const struct mq_attr *mqstat,struct mq_attr *omqstat);
int mqClose(mqd_t mqdes);
int mqUnlink(const char *name);

int semInit(sem_t *sem,unsigned value);
int semWait(sem_t *sem,int wait_ms);
int semPost(sem_t *sem);
int semDestroy(sem_t *sem);

#ifdef __cplusplus
}
#endif

#endif
