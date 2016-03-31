#include<stdio.h>

void print(char *ptr)
{
	printf("function ptr:%s\n",ptr);
}

int main(int argc,char **argv)
{
	void (*func)(char p);
	
	(*print)("hello,world\n");
		
	return 0;
}
