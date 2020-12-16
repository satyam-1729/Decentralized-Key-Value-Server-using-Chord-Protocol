
#include <stdbool.h>
char value1[1024];

void dumpToFile(char* filename,char * key , char * value , char * dec_hashValue)
{
    FILE* csv_reader = fopen(filename,"r");
    char buffer[262146];
    char check[262146];
    char * arg=NULL;
    char tempfilename[1024];
    strcpy(tempfilename,key);
    strcat(tempfilename,"tempI.csv");
    FILE* temp = fopen(tempfilename,"w");
    int flag=0;
    while(fgets(buffer,sizeof(buffer),csv_reader))
    {
        char *temp1 = strstr(buffer,key);
        if(!temp1)
        {
            fprintf(temp,"%s",buffer);
        }
        else
        {
            strcpy(check,buffer);
            char * part = strtok_r(check," ,",&arg);
            if(strcmp(part,key)==0)
            {
                //printf("it's equal\n");
          fprintf(temp,"%s , %s, %s \n",key,value,dec_hashValue);
          flag=1;
          }
          else
          {
             fprintf(temp,"%s",buffer); 
          }
            
        }
        
    }
    if(flag==0)
        fprintf(temp,"%s , %s,%s \n",key,value,dec_hashValue);
    fclose(csv_reader);
    fclose(temp);
    remove(filename);
    rename(tempfilename,filename);

}
char* restoreFromFile(char* filename,char * key)
{
    char check[262146];
    char * arg=NULL;
    FILE * csv_reader = fopen(filename,"r");
    char buffer[262146];
    while(fgets(buffer,sizeof(buffer),csv_reader))
    {
        char *temp = strstr(buffer,key);
        if(temp){
            strcpy(check,buffer);
            char * part = strtok_r(check," ,",&arg);
            if(strcmp(part,key)==0)
            { 
        char *start = strchr(temp,',');
        start++;
        strcpy(value1,start);
        return value1;
            }
        }
    }
    return NULL;

}

bool delete(char *filename,char* key)
{
    FILE* csv_reader = fopen(filename,"r");
    char buffer[262146];
    char check[262146];
    char * arg=NULL;
    bool flag = false;
    char tempfilename[1024];
    strcpy(tempfilename,key);
    strcat(tempfilename,"tempD.csv");
    FILE* temp = fopen(tempfilename,"w");
    while(fgets(buffer,sizeof(buffer),csv_reader))
    {
        char *temp1 = strstr(buffer,key);
        if(!temp1)
        {
            fprintf(temp,"%s",buffer);
        }
        else
        {
            strcpy(check,buffer);
            char* part = strtok_r(check," ,",&arg);
            if(strcmp(part,key)!=0)
            {
              fprintf(temp,"%s",buffer);  
            }
            else
            {
                flag= true;
            }
            
        }
        
    }
    fclose(csv_reader);
    fclose(temp);
    remove(filename);
    rename(tempfilename,filename);
    return flag;
}
