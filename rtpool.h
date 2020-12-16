#include <stddef.h>
typedef void (*func)(void *arg);
struct tpool_fun
{
	func fun;
	void *args;
	struct tpool_fun *next;
};

struct tpool
{
	struct tpool_fun *first;
	struct tpool_fun *last;
	pthread_mutex_t  mutex;
    pthread_cond_t   cond;
    pthread_cond_t   run_cond;
    int fun_count;
    int thread_count;
    int finish;
};

struct tpool_fun * tpool_fun_create(func fun, void *args)
{
	struct tpool_fun *f;
	if(fun == 0)
	{
		return 0;
	}
	f = malloc(sizeof(*f));
	f->fun = fun;
	f->args = args;
	f->next = 0;

	return f;
}

void tpool_fun_destroy(struct tpool_fun *f)
{
	if (f == 0)
		return;
	free(f);
}

struct tpool_fun * remove_from_q(struct tpool *t)
{
	if (t == 0)
	{
		return 0;
	}

	struct tpool_fun *f;
	f = t->first;
	if (f == 0)
	{
		return 0;
	}
	if(f->next == 0)
	{
		t->first = 0;
		t->last = 0;
	}
	else
	{
		t->first = f->next;
	}
	return f;
}

int add_to_q(struct tpool *pool, func f, void *args)
{
	struct tpool_fun *fun;
	if(pool == 0)
		return -1;

	fun = tpool_fun_create(f, args);
	if (fun == 0)
	{
		return -1;
	}

	pthread_mutex_lock(&(pool->mutex));
	if(pool->first == 0)
	{
		pool->first = fun;
		pool->last = pool->first; 
	}
	else
	{
		pool->last->next = fun;
		pool->last = fun;
	}
	pthread_cond_broadcast(&(pool->cond));
	pthread_mutex_unlock(&(pool->mutex));

	return 1;
}

void * tpool_worker(void* args)
{
	struct tpool *pool = args;
	struct tpool_fun *fun;

	while(1)
	{
		pthread_mutex_lock(&(pool->mutex));
		if(pool->finish)
			break;

		if(pool->first == 0)
			pthread_cond_wait(&(pool->cond),&(pool->mutex));

		fun = remove_from_q(pool);
		pool->fun_count++;
		pthread_mutex_unlock(&(pool->mutex));

		if(fun != 0)
		{
			fun->fun(fun->args);
			tpool_fun_destroy(fun);
		}

		pthread_mutex_lock(&(pool->mutex));
		pool->fun_count--;
		if(!pool->finish && pool->fun_count==0 && pool->first == 0)
		{
			pthread_cond_signal(&(pool->run_cond));
		}
		pthread_mutex_unlock(&(pool->mutex));
	}
	pool->thread_count--;
	pthread_cond_signal(&(pool->run_cond));
	pthread_mutex_unlock(&(pool->mutex));
	return 0;
}

struct tpool *tpool_create(int num)
{
	struct tpool *pool;
	pthread_t thread;
	if (num==0)
	{
		num=2;
	}
	pool = calloc(1,sizeof(*pool));
	pool->thread_count = num;
	pthread_mutex_init(&(pool->mutex),0);
	pthread_cond_init(&(pool->cond),0);
	pthread_cond_init(&(pool->run_cond),0);
	pool->first = 0;
	pool->last = 0;

	for (int i = 0; i < num; ++i)
	{
		pthread_create(&thread,0,tpool_worker,pool);
		pthread_detach(thread);
	}

	return pool;
}

void tpool_wait(struct tpool *pool)
{
	if(pool == 0)
		return;

	pthread_mutex_lock(&(pool->mutex));
	while(1)
	{
		if((!pool->finish && pool->fun_count!=0) || (pool->finish && pool->thread_count == 0))
		{
			pthread_cond_wait(&(pool->run_cond),&(pool->mutex));
		}
		else
		{
			break;
		}

	}
	pthread_mutex_unlock(&(pool->mutex));
}

void tpool_delete(struct tpool *pool)
{
	if(pool == 0)
		return;
	struct tpool_fun *t1;
	struct tpool_fun *t2;
	pthread_mutex_lock(&(pool->mutex));
	t1 = pool->first;
	while(t1 != 0)
	{
		t2 = t1->next;
		tpool_fun_destroy(t1);
		t1 = t2;
	}
	pool->finish = 1;
	pthread_cond_broadcast(&(pool->cond));
	pthread_mutex_unlock(&(pool->mutex));
	tpool_wait(pool);
	pthread_mutex_destroy(&(pool->mutex));
	pthread_cond_destroy(&(pool->cond));
	pthread_cond_destroy(&(pool->run_cond));
	free(pool);
}
//7977489422