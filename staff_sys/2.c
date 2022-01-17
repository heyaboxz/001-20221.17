#include <stdio.h>
#include <string.h>
int main(int argc, const char *argv[])
{
	// int i,a,b,c,d;
	// char * errinfo1="error";
	// char errinfo2[10];
	// char errinfo3[5];
	// memcpy(errinfo2,"error",strlen("error"));
	// for(i=0;i<5;i++)
		// errinfo3[i]=errinfo2[i];

	// a = sizeof(errinfo1);
	// b = sizeof(errinfo2);
	// c = strlen(errinfo1);
	// d = strlen(errinfo3);
	// printf("a=%d\nb=%d\nc=%d\nd=%d\n",a,b,c,d);

     int va = 0x11111111;
    // char arr[20]="qwwwwwwwwwwwww";
     char *pva=(char*)&va;
    // int *pa=(int*)arr;
     printf("pva=%p *pva=%#x va:%#x\n",pva,*pva,va);
     printf("char*=%ld,int*=%ld\n",sizeof(char*),sizeof(int*));  
     
     
     
   
     
     
     
     
     
	return 0;
}
