/* CS744--Assignment5(IIT Bombay)
* Author-- Satyam Behera
* Author-- Nitesh Singh
* Author-- Raghav Thakkar
*/

 
#include <stdio.h> 
#include <stdlib.h> 
#include <unistd.h> 
#include <string.h> 
#include <sys/types.h> 
#include <openssl/sha.h>
#include <sys/socket.h> 
#include <arpa/inet.h> 
#include <netinet/in.h>
#include <stdbool.h> 
#include <pthread.h>
#include <fcntl.h>
#include <math.h>
#include "rtpool.h"
#include "dbmng.h"
#include "KVcache.h"
#include "sych.h"
#include "xml.h"

 
#define MAXLINE 1024
bool key_check[65536];
int portnum;
int count=0;
char hexVal[8];
unsigned char result[SHA_DIGEST_LENGTH];
pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;
struct fingertable
{
	int key;
	char succ_hash[6];
	int succ_portno;
	int succ_ip;
};
struct fingertable ft[16];
struct peer
{
	int dec_hashkey;
	char hex_hashkey[7];
	int ip;
	int port;

};
char nameStr[10];
struct peer pr[8];
struct peer successor,predessor,current;
void findhash(char inp[],char hexVal[] )
{
	SHA1(inp, strlen(inp), result);
 	for(int i = 0; i < 3; i++)
   		sprintf((char*)&hexVal[i],"%02x", result[i]);
}

int hexadecimalToDecimal(char hexVal[])
{
	int base = 1; 
    int dec_val = 0;  
    for (int i=3; i>=0; i--) 
    {     
        if (hexVal[i]>='0' && hexVal[i]<='9') 
        { 
            dec_val += (hexVal[i] - 48)*base;  
            base = base * 16; 
        } 
  
        else if (hexVal[i]>='A' && hexVal[i]<='F')
        { 
            dec_val += (hexVal[i] - 55)*base; 
            base = base*16; 
        } 
	else if (hexVal[i]>='a' && hexVal[i]<='f')
	{
		dec_val += (hexVal[i] - 87)*base;
	}

    } 
      
    return dec_val; 
}


void swap(struct peer *xp, struct peer *yp)  
{  
   struct peer temp;
   temp.dec_hashkey = xp->dec_hashkey;
   strcpy(temp.hex_hashkey,xp->hex_hashkey);
   temp.ip = xp->ip;
   temp.port = xp->port;
   xp->dec_hashkey = yp->dec_hashkey;
   strcpy(xp->hex_hashkey,yp->hex_hashkey);
   xp->ip = yp->ip;
	xp->port = yp->port;
	yp->dec_hashkey=temp.dec_hashkey;
   strcpy(yp->hex_hashkey,temp.hex_hashkey);
   yp->ip = temp.ip;
	yp->port = temp.port;

}
void sort_peerlist()
{
	int i, j, min_idx; 
    for (i = 0; i < count-1; i++)  
    { 
        min_idx = i;  
        for (j = i+1; j < count; j++)  
        if (pr[j].dec_hashkey < pr[min_idx].dec_hashkey)  
            min_idx = j;  
        swap(&pr[min_idx], &pr[i]);
	}
}
int peer_info()
{
	count=0;
	FILE * peer_reader = fopen("peerinfo.txt","r");
	char buff[1024];
	char *dkey,*hkey,*i_p,*po;
	while (fgets(buff,sizeof(buff),peer_reader))
	{
		hkey = strtok(buff," ");
		dkey = strtok(NULL," ");
		i_p = strtok(NULL," ");
		po = strtok(NULL," ");
		pr[count].dec_hashkey = atoi(dkey);
		strcpy(pr[count].hex_hashkey,hkey);
		pr[count].ip = atoi(i_p);
		pr[count].port = atoi(po);
		count++;
	}
	//printf("%d\n",count);
	int key = current.dec_hashkey;
	sort_peerlist();
	/*add predessor and succesor in */
	int index;
	for (int i=0;i<count;i++)
	{
		if(key==pr[i].dec_hashkey)
		{
			index=i;
			break;
		}
	}

	if(count==1)
	{
		successor.dec_hashkey = key;
		successor.ip = pr[0].ip;
		successor.port = pr[0].port;
		strcpy(successor.hex_hashkey,hexVal);
		predessor.dec_hashkey = key;
		predessor.ip = pr[0].ip;
		predessor.port = pr[0].port;
		strcpy(predessor.hex_hashkey,hexVal);
		for(int i=0;i<65536;i++)
		{
			key_check[i] = true;
		}
	}
	else if(index==count-1)
	{
		successor.dec_hashkey = pr[0].dec_hashkey;
		successor.ip = pr[0].ip;
		successor.port = pr[0].port;
		strcpy(successor.hex_hashkey,pr[0].hex_hashkey);
		predessor.dec_hashkey = pr[index-1].dec_hashkey;
		predessor.ip = pr[index-1].ip;
		predessor.port = pr[index-1].port;
		strcpy(predessor.hex_hashkey,pr[index-1].hex_hashkey);
		for(int i=predessor.dec_hashkey+1; i<= current.dec_hashkey; i++)
		{
			key_check[i] = true;
		}
	}
	else if(index ==0)
	{
		successor.dec_hashkey = pr[index+1].dec_hashkey;
		successor.ip = pr[index+1].ip;
		successor.port = pr[index+1].port;
		strcpy(successor.hex_hashkey,pr[index+1].hex_hashkey);
		predessor.dec_hashkey = pr[count-1].dec_hashkey;
		predessor.ip = pr[count-1].ip;
		predessor.port = pr[count-1].port;
		strcpy(predessor.hex_hashkey,pr[count-1].hex_hashkey);
		for(int i=0;i<=current.dec_hashkey;i++)
		{
			key_check[i] = true;
		}
		for (int i = predessor.dec_hashkey+1; i < 65536 ; ++i)
		{
			key_check[i] = true;
		}
	}
	else
	{
		successor.dec_hashkey = pr[index+1].dec_hashkey;
		successor.ip = pr[index+1].ip;
		successor.port = pr[index+1].port;
		strcpy(successor.hex_hashkey,pr[index+1].hex_hashkey);
		predessor.dec_hashkey = pr[index-1].dec_hashkey;
		predessor.ip = pr[index-1].ip;
		predessor.port = pr[index-1].port;
		strcpy(predessor.hex_hashkey,pr[index-1].hex_hashkey);
		for (int i = predessor.dec_hashkey+1; i <= current.dec_hashkey; ++i)
		{
			key_check[i] = true;
		}
		
	}
	//debug code
/*
	for(int i=0;i<count;i++)
	{
		printf("%d\t",pr[i].port);
	}
	printf("\n");
*/
	//debug code end
	//printf("done\n");
	return index;
}

void fingertable_init(int n)
{
	int temp;
	for(int i=0;i<16;i++)
	{
		temp=0;
		ft[i].key = (n + (int)pow(2,i)) % 65536;
		for(int j=0;j<count;j++)
		{
			if(ft[i].key <= pr[j].dec_hashkey)
				{
					temp=j;
					break;
				}
		}
		ft[i].succ_portno = pr[temp].port;
		ft[i].succ_ip = pr[temp].ip;
		strcpy(ft[i].succ_hash,pr[temp].hex_hashkey);

	}
}

struct peer find_succssor(int id)
{
	struct peer temp;
	if((id <= successor.dec_hashkey && id > current.dec_hashkey)|| (id==successor.dec_hashkey)||(id > current.dec_hashkey && id > successor.dec_hashkey)||(id <= successor.dec_hashkey && id < current.dec_hashkey))
	{
		printf("test\n");
		return successor;
	}
	else
	{
		for(int i=15;i>=0;i--)
		{
			printf("%d\t%d\n",ft[i].key,ft[i].succ_portno );
			if(id > hexadecimalToDecimal(ft[i].succ_hash))
			{
				temp.dec_hashkey = hexadecimalToDecimal(ft[i].succ_hash);
				temp.ip = ft[i].succ_ip;
				temp.port = ft[i].succ_portno;
				strcpy(temp.hex_hashkey,ft[i].succ_hash);
				break;
			}
		}

		return temp;
	}
	
}

void update_flag_table(int id,int index)
{
	if(count > 2)
	{
		if(index >= 2)
		{
			for(int i=pr[index-2].dec_hashkey+1;i<=id;i++)
			{
				key_check[i] = false;

			}	
		}
		else if(index==1)
		{
			for(int i=0;i<=id;i++)
			{
				key_check[i] = false;
			}
			for (int i = pr[count-1].dec_hashkey; i < 65536; ++i)
			{
				key_check[i] = false;
			}
		}
		else if (index == 0)
		{
			for(int i=pr[count-2].dec_hashkey;i<=id;i++)
			{
				key_check[i] = false;
			}
		}
	}
	else
	{
		if(index == 1)
		{
			for (int i = current.dec_hashkey; i < 65536; ++i)
			{
				key_check[i] = false;
			}
			for(int i = 0; i<=id;i++)
			{
				key_check[i] = false;
			}
		}
		if(index == 0)
		{
			for(int i=current.dec_hashkey+1;i<=id;i++)
			{
				key_check[i] = false;
			}
		}
	}
	
	
}/*
void update_db(int port)
{
	char predfille[19];
	sprintf(predfille,"%d.csv",port);

	FILE *fp = fopen(nameStr,"r");

}*/

int main(int argc,char const *argv[]) { 



	intialisation(cache,256,4);
	void *periodic_thread_fun(void * vargp)
	{
		int turn =0;
		while(1)
		{
			//printf("s\n");
			turn++;
		sleep(3);
		peer_info();
		fingertable_init(current.dec_hashkey);
		//printf("%s\n",successor.hex_hashkey);
		printf("-------------------fingertable----------------------------------\n");
		printf("KEY\tSUCC\n");
		for(int i=0;i<16;i++)
		{
			printf("%d\t%d\n",ft[i].key,hexadecimalToDecimal(ft[i].succ_hash));
		}

		}
	}
	 
	 pthread_t periodic_thread;
	//ThreadPool Start
	struct incomingData
	{
		char msg[262146];
		int port;
	};
	int num_thread =5;
	int sockfd;
	struct sockaddr_in servaddr, cliaddr,predessor_servaddr,clientaddress;
	struct tpool *tm;
	tm = tpool_create(num_thread); 
	void myThreadFun(void *vargp)
	{
		struct incomingData *data_income = (struct incomingData *)vargp;
		char msg[262164];
		strcpy(msg,data_income->msg);
		//char *msg = data_income->msg;
		//printf("%s\n",msg);
		char * temp = strstr(msg,"JOIN");
		char * temp1 = strstr(msg,"LOOKUP");
		if(temp)
		{
			char buff[262164];
			char *dkey,*hkey,*po;
			strcpy(buff,msg);
			strtok(buff," ");
			hkey = strtok(NULL," ");
			dkey = strtok (NULL, " ");
			po = strtok(NULL," ");
			//printf("%s\n",hkey);
			successor.dec_hashkey = atoi(dkey);
			strcpy(successor.hex_hashkey,hkey);
			successor.port = atoi(po);
			int index_other = peer_info();
			fingertable_init(current.dec_hashkey);
			update_flag_table(predessor.dec_hashkey,index_other);
			/*
			for(int i=0;i<65536;i++)
			{
				if(!key_check[i])
					printf("%d ",i);
			}
			*/
			//update_db(predessor.port);

		}
		else if(temp1)
		{
			char buff[262164];
			char *type,*cid,*key,*value,*hexkey,*po;
			strcpy(buff,msg);
			strtok(buff," ");
			type = strtok(NULL," "); 
			cid = strtok(NULL," ");
			key = strtok(NULL," ");
			hexkey = strtok(NULL," ");
			po = strtok(NULL," ");
			if (strcmp("I",type)==0)
			{
				value = strtok(NULL," ");
			}
			char filename[10];
			char currentport[6];
			sprintf(currentport,"%d",current.port);
			strcpy(filename,currentport);
			strcat(filename,".csv");
			int id = atoi(cid);
			int port = atoi(po);
			char par[262146];
			int index = getSetId(key);
				
			if(key_check[id])
			{
				memset(&clientaddress, 0, sizeof(clientaddress));
				clientaddress.sin_family = AF_INET;
				clientaddress.sin_addr.s_addr = htonl(INADDR_ANY);
				clientaddress.sin_port = port;
				if(strcmp(type,"I")==0)
				{

					write_cachelock(index);
					writeEntry(cache,key,value,cid,filename);
					write_cacheunlock(index);
					pthread_mutex_lock(&lock);
					xmlmaker(key,value,"RI");
					int fd2 = open("server.xml",O_RDONLY);
        			read(fd2,par,262146);
					close(fd2);
					sendto(sockfd,(const char *)par,strlen(par),MSG_CONFIRM,(const struct sockaddr *)&clientaddress,sizeof(clientaddress));
					pthread_mutex_unlock(&lock);
					bzero(par,262146);
				}
				else if(strcmp(type,"D")==0)
				{
					write_cachelock(index);
					delete_cache(cache,key);
					write_cacheunlock(index);
					if(delete(filename,key))
                    {
						pthread_mutex_lock(&lock);
					    xmlmaker(key,value,"RI");
                    }
                    else{
						pthread_mutex_lock(&lock);
                        xmlmaker(NULL,NULL,"E");
                    }
					int fd2 = open("server.xml",O_RDONLY);
        			read(fd2,par,262146);
					close(fd2);
					sendto(sockfd,(const char *)par,strlen(par),MSG_CONFIRM,(const struct sockaddr *)&clientaddress,sizeof(clientaddress));
					pthread_mutex_unlock(&lock);
					bzero(par,262146);
				}
				else if(strcmp(type,"S")==0)
				{
					char result[262146];
					char * dbrslt=NULL;
					read_cachelock(index);
					int hitflag = search_cache(cache,key,result);
					read_cacheunlock(index);
					if(hitflag==0)
					{
						dbrslt = restoreFromFile(filename,key);
						if(dbrslt != NULL)
						{
							write_cachelock(index);
							writeEntry(cache,key,dbrslt,cid,filename);
							write_cacheunlock(index);
							pthread_mutex_lock(&lock);
							xmlmaker(key,dbrslt,"RS");
						}
						else
						{
							pthread_mutex_lock(&lock);
							xmlmaker(NULL,NULL,"E");
						}
					}
					else
					{
						printf("hit\n");//hit or not debug
						xmlmaker(key,result,"RS");
					}
					int fd2 = open("server.xml",O_RDONLY);
        			read(fd2,par,262146);
					close(fd2);
					sendto(sockfd,(const char *)par,strlen(par),MSG_CONFIRM,(const struct sockaddr *)&clientaddress,sizeof(clientaddress));
					pthread_mutex_unlock(&lock);
					bzero(par,262146);
				}
			}
			else
			{
				struct peer my_succ = find_succssor(id);
				memset(&clientaddress, 0, sizeof(clientaddress));
				clientaddress.sin_family = AF_INET;
				clientaddress.sin_addr.s_addr = INADDR_ANY;
				clientaddress.sin_port = my_succ.port;
				printf("succ of key %d\n",clientaddress.sin_port);
				sendto(sockfd,(const char *)msg,strlen(msg),MSG_CONFIRM,(const struct sockaddr *)&clientaddress,sizeof(clientaddress));
				printf("forwarding to %s\n",my_succ.hex_hashkey);
			}
			
		}
		else
		{
			struct data Data;
			char msg1[262146];
			strcpy(msg1,"LOOKUP ");
			char hex1key[6];
			char dec1key[6];
			char portua[6];
			int deckey;
			Data = xmlparser(msg);
			printf("after parsing\n");
			printf("key:%s\n",Data.key);
			printf("value:%s\n",Data.value);
			printf("type:%s\n",Data.type);
			findhash(Data.key,hex1key);
			deckey = hexadecimalToDecimal(hex1key);
			printf("key %d\n",deckey);
			sprintf(dec1key,"%d",deckey);
			sprintf(portua,"%d",data_income->port);
			strcat(msg1,Data.type);
			strcat(msg1," ");
			strcat(msg1,dec1key);
			strcat(msg1," ");
			strcat(msg1,Data.key);
			strcat(msg1," ");
			strcat(msg1,hex1key);
			strcat(msg1," ");
			strcat(msg1,portua);
			if(strcmp(Data.type,"I")==0)
			{
				strcat(msg1," ");
				strcat(msg1,Data.value);
			}
			strcpy(msg,msg1);

			char buff[262146];
			char *type,*cid,*key,*value,*hexkey,*po;
			strcpy(buff,msg);
			strtok(buff," ");
			type = strtok(NULL," "); 
			cid = strtok(NULL," ");
			key = strtok(NULL," ");
			hexkey = strtok(NULL," ");
			po = strtok(NULL," ");
			if (strcmp("I",type)==0)
			{
				value = strtok(NULL," ");
			}
			char filename[10];
			char currentport[6];
			sprintf(currentport,"%d",current.port);
			strcpy(filename,currentport);
			strcat(filename,".csv");
			int id = atoi(cid);
			int port = atoi(po);
			char par[262146];
			printf("%s\n",key);
			int index = getSetId(key);
			if(key_check[id])
			{
				memset(&clientaddress, 0, sizeof(clientaddress));
				clientaddress.sin_family = AF_INET;
				clientaddress.sin_addr.s_addr = htonl(INADDR_ANY);
				clientaddress.sin_port = (data_income->port);
				if(strcmp(type,"I")==0)
				{
					write_cachelock(index);
					writeEntry(cache,key,value,cid,filename);
					write_cacheunlock(index);
					pthread_mutex_lock(&lock);
					xmlmaker(key,value,"RI");
					int fd2 = open("server.xml",O_RDONLY);
        			read(fd2,par,262146);
					close(fd2);
					printf("%d\t%d\n",clientaddress.sin_port,clientaddress.sin_addr.s_addr);
					sendto(sockfd,(const char *)par,strlen(par),0,(const struct sockaddr *)&clientaddress,sizeof(clientaddress));
					pthread_mutex_unlock(&lock);
					bzero(par,262146);
				}
				else if(strcmp(type,"D")==0)
				{
					write_cachelock(index);
					delete_cache(cache,key);
					write_cacheunlock(index);
					if(delete(filename,key))
                    {
						pthread_mutex_lock(&lock);
					    xmlmaker(key,value,"RI");
                    }
                    else{
						pthread_mutex_lock(&lock);
                        xmlmaker(NULL,NULL,"E");
                    }
					int fd2 = open("server.xml",O_RDONLY);
        			read(fd2,par,262146);
					close(fd2);
					sendto(sockfd,(const char *)par,strlen(par),MSG_CONFIRM,(const struct sockaddr *)&clientaddress,sizeof(clientaddress));
					pthread_mutex_unlock(&lock);
					bzero(par,262146);
				}
				else if(strcmp(type,"S")==0)
				{
					char result[262146];
					char * dbrslt=NULL;
					read_cachelock(index);
					int hitflag = search_cache(cache,key,result);
					read_cacheunlock(index);
					if(hitflag==0)
					{
						dbrslt = restoreFromFile(filename,key);
						if(dbrslt != NULL)
						{
							write_cachelock(index);
							writeEntry(cache,key,dbrslt,cid,filename);
							write_cacheunlock(index);
							pthread_mutex_lock(&lock);
							xmlmaker(key,dbrslt,"RS");
						}
						else
						{
							pthread_mutex_lock(&lock);
							xmlmaker(NULL,NULL,"E");
						}
					}
					else
					{
						printf("hit\n");//hit or not debug
						xmlmaker(key,result,"RS");
					}
					int fd2 = open("server.xml",O_RDONLY);
        			read(fd2,par,262146);
					close(fd2);
					sendto(sockfd,(const char *)par,strlen(par),MSG_CONFIRM,(const struct sockaddr *)&clientaddress,sizeof(clientaddress));
					pthread_mutex_unlock(&lock);
					bzero(par,262146);
				}
			}
			else
			{
				struct peer my_succ = find_succssor(id);
				memset(&clientaddress, 0, sizeof(clientaddress));
				clientaddress.sin_family = AF_INET;
				clientaddress.sin_addr.s_addr = INADDR_ANY;
				clientaddress.sin_port = my_succ.port;
				printf("%d key succ\n",clientaddress.sin_port);
				sendto(sockfd,(const char *)msg,strlen(msg),MSG_CONFIRM,(const struct sockaddr *)&clientaddress,sizeof(clientaddress));
				printf("forwarding to %s\n",my_succ.hex_hashkey);
			}



			

			
		}
		
		

		
	}
	//ThreadPool end


	portnum = atoi(argv[1]);
	 
	char buffer[MAXLINE];
	char clientport[6];
	char clientaddr[64];
	char serverport[6];
	char serveraddr[164];   
	 
	if ( (sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0 ) { 
		perror("socket creation failed"); 
		exit(EXIT_FAILURE); 
	}
	
	memset(&servaddr, 0, sizeof(servaddr)); 
	memset(&cliaddr, 0, sizeof(cliaddr)); 
	 
	servaddr.sin_family = AF_INET; // IPv4 
	servaddr.sin_addr.s_addr = INADDR_ANY; 
	servaddr.sin_port = portnum;
	sprintf(serverport,"%d",servaddr.sin_port);
	sprintf(serveraddr,"%d",servaddr.sin_addr.s_addr);
	strcat(serveraddr,serverport);
	findhash(serveraddr,hexVal);
	printf("server is running on IP:%d and PORT:%d  and Hash:%s,%d\n",servaddr.sin_addr.s_addr,portnum,hexVal,hexadecimalToDecimal(hexVal)); 

	if ( bind(sockfd, (const struct sockaddr *)&servaddr, 
			sizeof(servaddr)) < 0 ) 
	{ 
		perror("bind failed"); 
		exit(EXIT_FAILURE); 
	}
	FILE *fb1=fopen("peerinfo.txt","a");
    fprintf(fb1,"%s %d %d %d\n",hexVal,hexadecimalToDecimal(hexVal),servaddr.sin_addr.s_addr,portnum);
	fclose(fb1);
	//fingertable_init();
	current.dec_hashkey = hexadecimalToDecimal(hexVal);
	current.ip = INADDR_ANY;
	current.port = portnum;

	 
		snprintf(nameStr, sizeof(nameStr), "%d", current.port);
		strcat(nameStr,".csv");

		FILE *fp = fopen(nameStr,"a");
		if(fp == NULL)
		{
			return -1;
		}
		fclose(fp);

	strcpy(current.hex_hashkey,hexVal);
	peer_info();
	fingertable_init(current.dec_hashkey);
	pthread_create (&periodic_thread,NULL,periodic_thread_fun,NULL);
	if(count!=1)
	{
		printf("joining\n");
		int len1;
		char join_message[1024]= "JOIN ";
		char port_dec[8];
		char port[8];
		memset(&predessor_servaddr, 0, sizeof(predessor_servaddr));
		predessor_servaddr.sin_family = AF_INET;
		predessor_servaddr.sin_addr.s_addr = INADDR_ANY;
		predessor_servaddr.sin_port = successor.port;
		
		strcat(join_message,current.hex_hashkey);
		strcat(join_message," ");
		sprintf(port_dec,"%d",current.dec_hashkey);
		strcat(join_message,port_dec);
		sprintf(port,"%d",current.port);
		strcat(join_message," ");
		strcat(join_message,port);

		printf("%d\n",predessor.port);
		//printf("%s\n",join_message);
		sendto(sockfd, (const char *)join_message, strlen(join_message), 
		MSG_CONFIRM, (const struct sockaddr *) &predessor_servaddr, 
			sizeof(predessor_servaddr));
	}


	char * hello = "successFul";
	struct incomingData dt;
    while(1)
    {
      int n;
	  printf("waitingfor recv\n");
	  socklen_t len = sizeof(struct sockaddr_in);
	  n = recvfrom(sockfd, (char *)buffer, MAXLINE, 
				MSG_WAITALL, ( struct sockaddr *) &cliaddr, 
				&len); 
	buffer[n] = '\0';
	printf("%s\n",buffer);
	strcpy(dt.msg,buffer);
	dt.port = cliaddr.sin_port;
	/*
	sendto(sockfd, (const char *)hello, strlen(hello), 
		MSG_CONFIRM, (const struct sockaddr *) &cliaddr, 
			len); */
	add_to_q(tm,myThreadFun,(void *)&dt);

    }
} 
