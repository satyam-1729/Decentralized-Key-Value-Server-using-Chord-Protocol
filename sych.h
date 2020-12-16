#include <pthread.h>
#include <stdbool.h>

bool writer_db_present[256] = {false};
bool writer_cache_present[256] = {false};
static int reader_db_count[256] , writer_db_waiting[256];
pthread_mutex_t dblock[256];
pthread_cond_t reader_can_enter_db[256];
pthread_cond_t writer_can_enter_db[256];

pthread_mutex_t cachelock[256];
pthread_cond_t reader_can_enter_cache[256];
pthread_cond_t writer_can_enter_cache[256];
static int reader_cache_count[256],writer_cache_waiting[256];

void sync_init()
{
	for (int i = 0; i < 256; ++i)
	{
		//writer_db_present[i] = false;
		pthread_mutex_init(&(dblock[i]),0);
		pthread_mutex_init(&(cachelock[i]),0);
		pthread_cond_init(&(reader_can_enter_db[i]),0);
		pthread_cond_init(&(writer_can_enter_db[i]),0);
		pthread_cond_init(&(reader_can_enter_cache[i]),0);
		pthread_cond_init(&(writer_can_enter_cache[i]),0);
	}
}

void read_dblock(int i)
{
    pthread_mutex_lock(&dblock[i]);
    while(writer_db_present[i] || writer_db_waiting[i] > 0)
    {
        pthread_cond_wait(&reader_can_enter_db[i],&dblock[i]);
    }
    reader_db_count[i]++;
    pthread_mutex_unlock(&dblock[i]);
}
void read_dbunlock(int i)
{
    pthread_mutex_lock(&dblock[i]);
    reader_db_count[i]--;
    if(reader_db_count[i]==0)
    {
        pthread_cond_signal(&writer_can_enter_db[i]);
    }
    pthread_mutex_unlock(&dblock[i]);
}
void write_dblock(int i)
{
    pthread_mutex_lock(&dblock[i]);
    writer_db_waiting[i]++;
    while(reader_db_count[i] > 0 || writer_db_present[i])
    {
        pthread_cond_wait(&writer_can_enter_db[i],&dblock[i]);
    }
    writer_db_waiting[i]--;
    writer_db_present[i] = true;
    pthread_mutex_unlock(&dblock[i]);
}
void write_dbunlock(int i)
{
    pthread_mutex_lock(&dblock[i]);
    writer_db_present[i] = false;
    if(writer_db_waiting[i]==0)
        pthread_cond_signal(&reader_can_enter_db[i]);
    else
        pthread_cond_signal(&writer_can_enter_db[i]);
    pthread_mutex_unlock(&dblock[i]);
}



void read_cachelock(int i)
{
    pthread_mutex_lock(&cachelock[i]);
    while(writer_cache_present[i] || writer_cache_waiting[i] > 0)
    {
        pthread_cond_wait(&reader_can_enter_cache[i],&cachelock[i]);
    }
    reader_cache_count[i]++;
    pthread_mutex_unlock(&cachelock[i]);
}
void read_cacheunlock(int i)
{
    pthread_mutex_lock(&cachelock[i]);
    reader_cache_count[i]--;
    if(reader_cache_count[i]==0)
    {
        pthread_cond_signal(&writer_can_enter_cache[i]);
    }
    pthread_mutex_unlock(&cachelock[i]);
}
void write_cachelock(int i)
{
    pthread_mutex_lock(&cachelock[i]);
    writer_cache_waiting[i]++;
    while(reader_cache_count[i] > 0 || writer_cache_present[i])
    {
        pthread_cond_wait(&writer_can_enter_cache[i],&cachelock[i]);
    }
    writer_cache_waiting[i]--;
    writer_cache_present[i] = true;
    pthread_mutex_unlock(&cachelock[i]);
}
void write_cacheunlock(int i)
{
    pthread_mutex_lock(&cachelock[i]);
    writer_cache_present[i] = false;
    if(writer_cache_waiting[i]==0)
        pthread_cond_signal(&reader_can_enter_cache[i]);
    else
        pthread_cond_signal(&writer_can_enter_cache[i]);
    pthread_mutex_unlock(&cachelock[i]);
}
