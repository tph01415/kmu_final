#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#define TEXT_TYPE 1  //소개글 글자타입
#define NUM_TYPE 2

char eofItem = 0;
#define SIZE_ROW 50
#define SIZE_COL 100

FILE* check(FILE* fpr)
{
	FILE* tempFp = fopen("temp.txt","a+");
	char input_data[SIZE_COL+2][SIZE_ROW+2] = {0};
	int noRow=0;
	int rCount;
	int i,j;
	unsigned short int rowCS,xRowCS;
	unsigned short int colCS[SIZE_ROW+2] = {0};
	unsigned short int xColCS[SIZE_ROW+2] = {0};
	typedef struct error_loc {
		int row,col;
		int diff;
	}Error_Loc;
	Error_Loc detected_error[4] = {{0,0,0}};

	while((rCount = fread(input_data[noRow],sizeof(char),SIZE_ROW+2,fpr))>1) /////////NULL 1
		noRow++;

	for(j=0;j<SIZE_ROW;j++)
	{
		xColCS[j] = (unsigned short)(0xFF&input_data[noRow-2][j]);
		xColCS[j] +=(0xFF00 & (input_data[noRow-1][j]<<8));
	}

	int numError = 0;

	for( i = 0;i<noRow-2;i++){
		xRowCS = (unsigned short)(0xFF & input_data[i][SIZE_ROW]);
		xRowCS +=(0xFF00 & (input_data[i][SIZE_ROW+1]<<8));
		rowCS = 0;
		for(j=0;j<SIZE_ROW;j++){
			rowCS+=input_data[i][j];
			colCS[j] += input_data[i][j];
		}
		if(xRowCS != rowCS){
			printf("%d Row error detected\n",i);
			detected_error[numError].row = i;
			detected_error[numError].diff = xRowCS - rowCS;
			numError++;
		}
	}

	for(j=0;j<SIZE_ROW;j++){
		if(xColCS[j] != colCS[j]){
			for(i=j;i<numError;i++){
				if(detected_error[i].diff == (xColCS[j] - colCS[j])){
					detected_error[i].col = j;
					break;
				}
			}
			if(i==numError)
				printf("복구 불가 변조위치\n");
			else{
				input_data[detected_error[i].row][detected_error[i].col] += detected_error[i].diff;
			}
		}
	}

	for(i=0;i<SIZE_ROW;i++)
	{
		for(j=0;j<noRow-2;j++)
			fprintf(tempFp,"%c",input_data[i][j]);
	}
	fclose(tempFp);
	fpr = fopen("temp.txt","r+");
	return fpr;
}


void print(FILE* fpr, FILE *fpw){ // user 함수 조금이라도 깨끗하게 보이기 위해 설정
  int c;
  while((c = fgetc(fpr)) != '/')
    fprintf(fpw, "%c", c);
}

void hexToDeci(FILE* fpr, FILE* fpw) // 16진수 > 10진수 변환
{
	char hex[16];
	int i = 0, deci = 0, c, k;
	int val, len;

	while(1) {
		c = fgetc(fpr);
		if(c == '/')
			break;
		else if(c=='\n')
		{
			eofItem = 1;
			break;
		}
		else
			hex[i++] = c;

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

void friend(FILE* fpr, FILE* fpw)
{
	char c;
	int i = 1;
	fprintf(fpw, "\n\n*FRIENDS LIST*");
	while(1)
	{
		fprintf(fpw, "\nFRIEND%d ID: ", i);
		print(fpr, fpw);
		fprintf(fpw, "\nFRINED%d NAME: ", i);
		print(fpr, fpw);
		fprintf(fpw, "\nFRIEND%d GENDER: ", i);
		while((c = fgetc(fpr)) != '/'){
			if(c == 'F') 
				fprintf(fpw, "FEMALE");
			else fprintf(fpw, "MALE"); // F가 아닐 경우도 설정
		}
		//                       print(fpr, fpw);
		fprintf(fpw, "\nFRINED%d AGE: ", i);
		while(1)
		{
			c=fgetc(fpr);
			if(c=='|' || c=='\n')
				break;
			fprintf(fpw, "%c", c);
		}
		fprintf(fpw, "\n");
		i++;
		if(c=='\n')
			break;
	}
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
  hexToDeci(fpr, fpw);
  
  fprintf(fpw, "\nMP: ");
  hexToDeci(fpr, fpw);

  fprintf(fpw, "\nCOIN: ");
  hexToDeci(fpr, fpw);

}

void items(FILE* fpr, FILE* fpw)
{
	eofItem =0;
	char c;
	fprintf(fpw, "\n\n*ITEMS*");
	while(1)
	{
		if(eofItem ==1)
			break;

		c = fgetc(fpr);
		if(c == 'A')
		{
			fprintf(fpw, "\nBOMB: ");
			hexToDeci(fpr, fpw);
		}
		else if(c == 'B')
		{
			fprintf(fpw, "\nPOTION: ");
			hexToDeci(fpr, fpw);
		}
		else if(c == 'C')
		{
			fprintf(fpw, "\nCURE: ");
			hexToDeci(fpr, fpw);
		}
		else if(c == 'D')
		{
			fprintf(fpw, "\nBOOK: ");
			hexToDeci(fpr, fpw);
		}
		else if(c == 'E')
		{
			fprintf(fpw, "\nSHIELD: ");
			hexToDeci(fpr, fpw);
		}
		else if(c == 'F')
		{
			fprintf(fpw, "\nCANNON: ");
			hexToDeci(fpr, fpw);
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
	int i;
	char tmp;
	char c;
	char str[10]={0};
	int a;
	int check_str = 0; // 문자열 체크변수 (1 = 시작, 2 = 끝, 0 = 문자열 구간X)
	int n=0; // 문자열 위치 가리키는 변수
	int check=0;                          //문자인지 숫자인지 체크
	while((tmp = fgetc(fpr)) != EOF)     //끝까지 1글자씩 읽어옴
	{
		if(check_str == 1) // 문자열 구간 시작 > 문자열 값 저장 
		{
			if(tmp >= 'A' && tmp <='Z')
			{
				if(check == TEXT_TYPE)
				{
					str[n] = c;
					n++;
				}
				else if(check == NUM_TYPE)
				{
					for(i=n; i<n+a; i++)
						str[i] = c;
					n=i;
				}
				c = tmp;
				check = TEXT_TYPE;
			}
			else if(tmp>='0' && tmp <='9')
			{
				if(check == TEXT_TYPE)
				{
					a = tmp-'0';
				}
				if(check == NUM_TYPE)
				{
					a=a*10;
					a+=tmp-'0';
				}
				check = NUM_TYPE;
			}
			else if(tmp == '"') // " 쌍따옴표 한 번 더 나와서 문자열이 끝나는 지점
			{
				if(check == TEXT_TYPE)
				{
					str[n] = c;
					n++;
				}
				else if(check == NUM_TYPE)
				{
					for(i=n; i<n+a; i++)
						str[i] = c;
					n=i;
				}
				check_str = 2;
				str[n] = '\0';
			}
			else              //특수문자(숫자텍스트)
			{
			  if(check == TEXT_TYPE)
			  {
				str[n] = c;
				n++;
			  }
			  else if(check == NUM_TYPE)
			  {
				for(i = n;i<n+a; i++)
					str[i] = c;
				n = i;
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
		else if(check_str == 2) // 문자열이 끝나고 다음 나오는 숫자에 따른 문자열 반복
		{
			a=tmp-'0';
			for(i=0; i<a; i++)
			{
				fprintf(fpw, "%s", str);
			}
			check_str = 0;  // 각 값들 초기화
			check = 0;
			n=0;
			str[0] = '\0';
		}
		else if(check_str == 0) // 문자열 구간이 아닐때
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
		  else if(tmp == '"') // 문자열 일 때
		  {
				if(check == TEXT_TYPE)
				{
					fputc(c,fpw);
				}
				else if(check == NUM_TYPE)
				{
					for(i=0;i<a;i++)
						fputc(c,fpw);
				}
			check_str = 1; //  문자열 check 변수 1로 변경 -> 다음 "가 나올때까지 배열로 입력받음
			check = 0; // check 타입 초기화
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

//	fpr = check(fpr);

	user(fpr, fpw);
  items(fpr,fpw);
	friend(fpr, fpw);
	description(fpr,fpw);

//	remove("temp.txt");
	fclose(fpr);
	fclose(fpw);
	return 0;
}
