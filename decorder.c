#include <stdio.h>
#include <stdlib.h>

#define TEXT_TYPE 1  //소개글 글자타입
#define NUM_TYPE 2

FILE* description(FILE* fpr,FILE* fpw) //소개글 디코딩
{
	fprintf(fpw,"*DESCRIPTION*\n");
	char tmp;
	char c;
	int a;
	int check=0;                          //문자인지 숫자인지 체크
	while((tmp = fgetc(fpr)) != EOF)     //끝까지 1글자씩 읽어옴
	{
		if(tmp >= 'A' && tmp <= 'Z')  //영문자일때
		{
			if(check == TEXT_TYPE)  //이전 문자가 텍스트이면
			{
				fputc(c,fpw);   //이전문자 출력
			}
			else if(check == NUM_TYPE) // 이전문자 숫자이면
			{
				for(int i = 0;i<a;i++)   //숫자만큼 반복출력
					fputc(c,fpw);
			}
			c = tmp;    //읽은문자 c에 저장
			check = TEXT_TYPE;   //체크타입 변경
		}
		else if( tmp>='0' && tmp <='9')    //숫자일때
		{
			if(check == TEXT_TYPE)  //이전문자 텍스트
			{
				a = tmp-'0';  //읽은 숫자 a에 저장
			}
			if(check == NUM_TYPE) //이전문자 숫자일때
			{
				a=a*10;        //두자리수 이상의 경우임
				a+= tmp-'0';
			}
			check = NUM_TYPE;  //체크타입 변경
		}
		else if(tmp == '\n')  //줄바뀜
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
		else              //특수문자(숫자텍스트)
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
