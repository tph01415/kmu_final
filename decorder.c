#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#define TEXT_TYPE 1  //소개글 글자타입
#define NUM_TYPE 2


void print(FILE* fpr, FILE *fpw){ // user 함수 조금이라도 깨끗하게 보이기 위해 설정
  int c;
  while((c = fgetc(fpr)) != '/')
    fprintf(fpw, "%c", c);
}

void hexToDeci(FILE* fpr, FILE* fpw, int n) // 16진수 > 10진수 변환
{
  char hex[16];
  int i = 0, deci = 0, c, k;
  int val, len;

  switch (n)
  {
  case 1 : // 16진수가 / 앞까지 있는 경우
    while((c = fgetc(fpr)) != '/'){
      hex[i++] = c;
    }
    break;
  
  case 2 : // 16진수가 = 앞까지 있는 경우
    while((c = fgetc(fpr)) != '='){
      hex[i++] = c;
    }
		hex[i] = '\0';
    break;
  }

  for(k = 0; hex[k] != '\0'; k++){
    if(hex[k] > '0' && hex[k] <= '9'){
      val = hex[k] -48;
    }
    else if(hex[k] >= 'a' && hex[k] <= 'f'){
      val = hex[k] - 97 + 10;
    }
    else if(hex[k]>='A' && hex[k]<='F'){
      val = hex[k] - 65 + 10;
    }

    deci += val * pow(16, i-1);
    i--;
  }
  fprintf(fpw, "%d", deci);
}


void user(FILE *fpr, FILE *fpw)
{
  int c, n;
  fprintf(fpw, "*USER STATUS*\n");

  fprintf(fpw, "ID: ");
  print(fpr, fpw);  // 단순 입력은 print 사용
  
  fprintf(fpw, "\nNAME: "); 
  print(fpr, fpw);

  fprintf(fpw, "\nGENDER: ");
  while((c = fgetc(fpr)) != '/'){
    if(c == 'F') 
      fprintf(fpw, "FEMALE");
    else fprintf(fpw, "MALE"); // F가 아닐 경우도 설정
  }

  fprintf(fpw, "\nAGE: ");
  print(fpr, fpw);

  char same[] = {'/'};
  fprintf(fpw, "\nHP: ");
  hexToDeci(fpr, fpw, 1); // 1은 '/'로 끝날 경우
  
  fprintf(fpw, "\nMP: ");
  hexToDeci(fpr, fpw, 1);

  fprintf(fpw, "\nCOIN: ");
  hexToDeci(fpr, fpw, 2); // 2는 '='로 끝날 경우

	// 신뢰성 처리해야할 곳

	while((c = fgetc(fpr)) == '\n'){
		break;
	}

}

void items(FILE* fpr, FILE* fpw)
{
        char c;
        fprintf(fpw, "\n*ITEMS*");
        while(1)
        {
                c = fgetc(fpr);
                if(c == 'A')
                {
                        fprintf(fpw, "\nBOMB: ");
                        hexToDeci(fpr, fpw, 3);
                }
                else if(c == 'B')
                {
                        fprintf(fpw, "\nPOTION: ");
                        hexToDeci(fpr, fpw, 3);
                }
                else if(c == 'C')
                {
                        fprintf(fpw, "\nCURE: ");
                        hexToDeci(fpr, fpw, 3);
                }
                else if(c == 'D')
                {
                        fprintf(fpw, "\nBOOK: ");
                        hexToDeci(fpr, fpw, 3);
                }
                else if(c == 'E')
                {
                        fprintf(fpw, "\nSHIELD: ");
                        hexToDeci(fpr, fpw, 3);
                }
                else if(c == 'F')
                {
                        fprintf(fpw, "\nCANNON: ");
                        hexToDeci(fpr, fpw, 3);
                }
                else
                {
                        break;
                }

        }
}


FILE* description(FILE* fpr,FILE* fpw) //소개글 디코딩
{
	fprintf(fpw,"\n*DESCRIPTION*\n");
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


	user(fpr, fpw);
/*
  items();
	friend();*/
	description(fpr,fpw);

	fclose(fpr);
	fclose(fpw);
	return 0;
}
