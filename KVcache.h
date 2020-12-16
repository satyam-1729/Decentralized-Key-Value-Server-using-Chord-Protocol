#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdbool.h>
#include <string.h>


struct KVblock
{
    char key[256];
    char value[262146];
    bool ref;
    bool valid;
};

struct KVcache
{
    int freeblock;
    struct KVblock *set; 
};
struct KVcache cache[256];
static int clock1[256];
char char_buf[300];

int SET_SIZE;
int CACHE_SIZE;
int getSetId(char* key)
{
    return (strlen(key)-1)%CACHE_SIZE;
}
int getfileindex(char* key)
{
 return (strlen(key)-1)%25;
}
void intialisation(struct KVcache *cache,int c,int s)
{
    CACHE_SIZE=c;
    SET_SIZE=s;
    for(int i=0;i<CACHE_SIZE;i++)
    {
        cache[i].freeblock=SET_SIZE;
        cache[i].set = (struct KVblock*) malloc(SET_SIZE*sizeof(struct KVblock));
    }
}

void writeEntry(struct KVcache * cache,char* key, char* value,char* dec_hashvalue,char* filename)
{
    int index = getSetId(key);
    int fileindex = getfileindex(key);
    int pointer = clock1[index];
    bool flag_present = false;
    for(int i=0;i<SET_SIZE;i++)
    {
        if(strcmp(cache[index].set[i].key,key)==0 && cache[index].set[i].valid)
        {
            strcpy(cache[index].set[i].value,value);
            cache[index].set[i].valid = true;
            cache[index].set[i].ref = true;
            return;
        }
    }

    if(cache[index].freeblock!=0)
    {
        strcpy(cache[index].set[pointer].key,key);
        strcpy(cache[index].set[pointer].value,value);
        cache[index].set[pointer].valid = true;
        cache[index].set[pointer].ref = false;
        pointer = (pointer+1)%SET_SIZE;
        cache[index].freeblock--;
        clock1[index] = pointer;

    }
    else
    {
        while(1)
        {
            if(!cache[index].set[pointer].ref)
            {
               strcpy(cache[index].set[pointer].key,key);
               strcpy(cache[index].set[pointer].value,value);
               cache[index].set[pointer].valid = true;
               cache[index].set[pointer].ref = false;
               pointer = (pointer+1)%SET_SIZE;
               clock1[index] = pointer;
               break;  
            }
            else
            {
                cache[index].set[pointer].ref=false;
                pointer = (pointer+1)%SET_SIZE;
            }
            

        }
    }
    
   dumpToFile(filename,key,value,dec_hashvalue); 
}


void delete_cache(struct KVcache* cache,char* key)
{
    int index = getSetId(key);
    int pointer = clock1[index];
    bool delete_flag=false;
    int delete_index;
    for(int i =0;i<SET_SIZE;i++)
    {
        if(strcmp(cache[index].set[i].key,key)==0 && cache[index].set[i].valid)
        {
            delete_flag = true;
            delete_index=i;
            break;
        }
    }
    if(delete_flag)
    {
        while(delete_index != pointer)
        {
            strcpy(cache[index].set[delete_index].key,cache[index].set[(delete_index+1)%SET_SIZE].key);
            strcpy(cache[index].set[delete_index].value,cache[index].set[(delete_index+1)%SET_SIZE].value);
            cache[index].set[delete_index].ref = cache[index].set[(delete_index+1)%SET_SIZE].ref ;
            cache[index].set[delete_index].valid=cache[index].set[(delete_index+1)%SET_SIZE].valid;
            delete_index = (delete_index+1)%SET_SIZE;
        }
        if(cache[index].freeblock==0)
        {
        cache[index].set[pointer].valid = false;
        cache[index].freeblock++;
        }
        else
        {
            pointer = (pointer-1)%SET_SIZE;
            cache[index].set[pointer].valid = false;
            cache[index].freeblock++;
        }

        clock1[index]  = pointer;
    }
}

int search_cache(struct KVcache* cache,char* key,char* arr)
{
    int index = getSetId(key);
    for(int i=0;i<SET_SIZE;i++)
    {
        if(strcmp(cache[index].set[i].key,key)==0 && cache[index].set[i].valid)
        {
            strcpy(arr,cache[index].set[i].value);
            return 1;
        }
    }
    return 0;
}

void toXML(struct KVcache * cache)
{
FILE *fb1=fopen("cache.xml","w");
fprintf(fb1,"<?xml version=\"1.0\" encoding=\"utf-8\"?>\n");
fprintf(fb1,"<KVCache>\n");
for(int index =0 ;index<CACHE_SIZE;index++)
{
bool count =true;
for(int i =0 ;i<SET_SIZE;i++)
{
	if(cache[index].set[i].valid)
	{
        if(count)
            fprintf(fb1,"<Set Id = \"%d\">\n",index);
        count = false;

		if(cache[index].set[i].ref)
			fprintf(fb1,"<CacheEntry isReferenced =\"true\" isvalid =\"true\">\n");
		else
			fprintf(fb1,"<CacheEntry isReferenced =\"false\" isvalid =\"true\">\n");
	fprintf(fb1,"<Key> %s </Key>\n",cache[index].set[i].key);
    fprintf(fb1,"<Value> %s </Value>\n",cache[index].set[i].value);
	fprintf(fb1,"</CacheEntry>\n");

	}
}
if(!count)
    fprintf(fb1,"</Set>\n");
}
fprintf(fb1,"</KVCache>\n");
fclose(fb1);


}


