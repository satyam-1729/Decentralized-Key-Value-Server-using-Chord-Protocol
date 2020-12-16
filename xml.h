
struct data {
    char key[256];
    char value[1024];
	char type[4];
};
void xmlmaker(char * key,char* value,char* type)
{
	if(strcmp(type,"I")==0)
	{
    FILE *fb1=fopen("client.xml","w");
	fprintf(fb1,"<?xml version=\"1.0\" encoding=\"utf-8\"?>\n");
    fprintf(fb1,"<KVMessage type=\"putreq\">\n");
    fprintf(fb1,"<Key> %s </Key>\n",key);
    fprintf(fb1,"<Value> %s </Value>\n",value);
    fprintf(fb1,"</KVMessage>\n");
    fclose(fb1);
	}
	else if(strcmp(type,"D")==0)
	{
	FILE *fb1=fopen("client.xml","w");
	fprintf(fb1,"<?xml version=\"1.0\" encoding=\"utf-8\"?>\n");
    fprintf(fb1,"<KVMessage type=\"delreq\">\n");
    fprintf(fb1,"<Key> %s </Key>\n",key);
    fprintf(fb1,"</KVMessage>\n");
    fclose(fb1);

	}
	else if(strcmp(type,"S")==0)
	{
	FILE *fb1=fopen("client.xml","w");
	fprintf(fb1,"<?xml version=\"1.0\" encoding=\"utf-8\"?>\n");
    fprintf(fb1,"<KVMessage type=\"getreq\">\n");
    fprintf(fb1,"<Key> %s </Key>\n",key);
    fprintf(fb1,"</KVMessage>\n");
    fclose(fb1);

	}
	else if(strcmp(type,"RI")==0)
	{
	FILE *fb1=fopen("server.xml","w");
	fprintf(fb1,"<?xml version=\"1.0\" encoding=\"utf-8\"?>\n");
    fprintf(fb1,"<KVMessage type=\"resp\">\n");
    fprintf(fb1,"<Message>Success </Message>");
    fprintf(fb1,"</KVMessage>\n");
    fclose(fb1);

	}
	else if(strcmp(type,"RS")==0)
	{
	FILE *fb1=fopen("server.xml","w");
	fprintf(fb1,"<?xml version=\"1.0\" encoding=\"utf-8\"?>\n");
    fprintf(fb1,"<KVMessage type=\"resp\">\n");
    fprintf(fb1,"<Key>%s </Key>\n",key);
    fprintf(fb1,"<Value>%s </Value>\n",value);
    fprintf(fb1,"</KVMessage>\n");
    fclose(fb1);
	}
	else if(strcmp(type,"E")==0)
	{
	 FILE *fb1=fopen("server.xml","w");
	 fprintf(fb1,"<?xml version=\"1.0\" encoding=\"utf-8\"?>\n");
     fprintf(fb1,"<KVMessage type=\"resp\">\n");
	 fprintf(fb1,"<Message>Error:Key not Found </Message>\n");
	 fprintf(fb1,"</KVMessage>\n");
	 fclose(fb1);

	}
	
}

struct data xmlparser(char * buffer)
{
	struct data Data;
	char key[345];
	char value[1024];
    char temp_buffer[1024];
	strcpy(temp_buffer,buffer);
	char *temp = strstr(temp_buffer,"putreq");
	if(temp)
	{
		temp = strstr(temp_buffer,"<Key>");
		char *start = strchr(temp,'>');
		start++;
		char* end = strchr(start,'<');
		end--;
		*end = '\0';
		strcpy(key,start+1);
		strcpy(temp_buffer,buffer);
		temp = strstr(temp_buffer,"<Value>");
		start = strchr(temp,'>');
		start++;
		end = strchr(start,'<');
		*end = '\0';
		strcpy(Data.key,start+1);//+1 beacuse of space
		strcpy(value,start+1);
		strcpy(Data.key,key);
		strcpy(Data.value,value);
		strcpy(Data.type,"I");
		return Data;

	}
	else
	{
		temp = strstr(temp_buffer,"getreq");
		if(temp)
			strcpy(Data.type,"S");
		else
			strcpy(Data.type,"D");
		temp = strstr(temp_buffer,"<Key>");
		char *start = strchr(temp,'>');
		start++;
		char* end = strchr(start,'<');
		end--;
		*end = '\0';
		strcpy(Data.key,start+1);//+1 beacuse of space
		strcpy(Data.value,"\0");
		return Data;
		

	}
	
}

void clientParser(char* response,char* pr_resp)
{
	char* temp = strstr(response,"<Value>");
	if(temp)
	{
		char *start = strchr(temp,'>');
		start++;
		char* end = strchr(start,'<');
		end--;
		*end = '\0';
		strcpy(pr_resp,start);
		return;
	}
	temp = strstr(response,"<Message>");
	if(temp)
	{
		char* start = strchr(temp,'>');
		start++;
		char* end = strchr(start,'<');
		end--;
		*end = '\0';
		strcpy(pr_resp,start);
		return;
	}

	strcpy(pr_resp,"XML Error: Received unparseable message");
	printf("XML Error: Received unparseable message");

	

}




