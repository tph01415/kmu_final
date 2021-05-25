#include <stdio.h>
#include <stdlib.h>

#define TEXT_TYPE 1
#define NUM_TYPE 2

FILE* description(FILE* fpr,FILE* fpw)
{
	fprintf(fpw,"*DESCRIPTION*\n");
	char tmp;
	char c;
	int a;
	int check=0;
	while((tmp = fgetc(fpr)) != EOF) 
	{
		if(tmp >= 'A' && tmp <= 'Z')
		{
			if(check == TEXT_TYPE)
			{
				fputc(c,fpw);
			}
			else if(check == NUM_TYPE)
			{
				for(int i = 0;i<a;i++)
					fputc(c,fpw);
			}
			c = tmp;
			check = TEXT_TYPE;
		}
		else if( tmp>='0' && tmp <='9')
		{
			if(check == TEXT_TYPE)
			{
				a = tmp-'0';
			}
			if(check == NUM_TYPE)
			{
				a=a*10;
				a+= tmp-'0';
			}
			check = NUM_TYPE;
		}
		else if(tmp == '\n')
		{
			if(check == TEXT_TYPE)
				fputc(c,fpw);
			else if(check == NUM_TYPE)
			{
				for(int i = 0;i<a;i++)
					fputc(c,fpw);
			}
			fputc(tmp,fpw);
			check = 0;
		}
		else
		{
			if(check == TEXT_TYPE)
			{
				fputc(c,fpw);
			}
			else if(check == NUM_TYPE)
			{
				for(int i = 0;i<a;i++)
					fputc(c,fpw);
			}
			switch(tmp)
			{
				case '!': c='1'; break;
				case '@': c='2'; break;
				case '#': c='3'; break;
				case '$': c='4'; break;
				case '%': c='5'; break;
				case '^': c='6'; break;
				case '&': c='7'; break;
				case '*': c='8'; break;
				case '(': c='9'; break;
				case ')': c='0'; break;
			}
			check = TEXT_TYPE;
		}
	}
}

int main(int argc, char* argv[])
{
	if(argc ==1)
	{
		printf("usage) ./decorder.out encoded_data resule.txt \n");
		return 0;
	}
	FILE* fpr = fopen(argv[1],"r+");  //인코딩 후 데이터 파일
	FILE* fpw = fopen(argv[2],"w+");  //디코딩 후 데이터 파일
/*
	user();
	items();
	friend();*/
	description(fpr,fpw);

	fclose(fpr);
	fclose(fpw);
	return 0;
}
