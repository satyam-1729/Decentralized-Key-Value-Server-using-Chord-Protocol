#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

bool distribute_key(char *from,char* to,char* key)
{
    FILE* csv_reader = fopen(from,"r");
    char buffer[262146];
    char check[262146];
    char * arg=NULL;
    bool flag = false;
    char tempfilename[1024];
    strcpy(tempfilename,key);
    strcat(tempfilename,"tempD.csv");
    FILE* temp = fopen(tempfilename,"w");
    FILE* dest = fopen(to,"a");
    while(fgets(buffer,sizeof(buffer),csv_reader))
    {
        strcpy(check,buffer);
        strtok(buffer,",");
        strtok(NULL,",");
        char * id = strtok(NULL,",");
        if(atoi(id) > atoi(key))
        {
            fprintf(temp,"%s",check);
        }
        else
        {
            
            fprintf(dest, "%s", check);
        }
        
    }
    fclose(csv_reader);
    fclose(temp);
    remove(from);
    rename(tempfilename,from);
    return flag;
}

