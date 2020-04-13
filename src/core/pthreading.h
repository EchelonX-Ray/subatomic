#ifndef _insertion_core_pthreading_h
#define _insertion_core_pthreading_h

#define _POSIX_C_SOURCE 199506L
#include <pthread.h>
#include "./../lib/wincomp/wincomp.h"

struct MTK_Thread_Return {
};

struct MTK_Thread_Param {
	struct MTK_WinBase *window;
	pthread_mutex_t *lock;
	pthread_mutex_t lock_val;
	struct MTK_Thread_Return ret_val;
};

void* thread0_entry_blink_cursor(void *thread_param);

#endif
