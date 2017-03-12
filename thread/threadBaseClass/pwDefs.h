#ifndef __PW_DEFS_H__
#define __PW_DEFS_H__
#ifdef DEBUG
#define PRINT(fmt,args...)	printf(fmt,##args)
#else
#define PRINT(fmt,args...)
#endif

#define NO_WAIT (0)
#define WAIT_FOREVER  (-1)

typedef void *(*FUNCPTR)(void * , void *, void *, void *, void *,void * , void *, void *, void *, void *);
typedef void *(*START_ROUTINE)(void *);

typedef struct 
{
	FUNCPTR entry;
	void *arg[10];
}FUNC_WRAPPER2;

typedef struct 
{
	int (*entry)(int , int , int,int , int , int,int , int , int,int);
	int parms[10];
}FUNC_WRAPPER;

#endif

