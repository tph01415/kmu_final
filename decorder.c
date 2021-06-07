#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#define TEXT_TYPE 1  //소개글 글자타입 읽어온게 문자일때
#define NUM_TYPE 2  //반복숫자
#define STRNUM_TYPE 3  //소개글 문자열 반복숫자
char eofItem = 0;
#define SIZE_ROW 12
#define SIZE_COL 100

FILE* check(FILE* fpr)
{
	FILE* tempFp = fopen("temp.txt","a+");     //체크썸검사 후 데이터와 체크썸값을 분리하기위한 임시파일
	char input_data[SIZE_COL+2][SIZE_ROW+2] = {0};  //입력받은 데이터 들어갈 배열
	int noRow=0;  //데이터가 들어간 행 개수
	int rCount;
	int i,j;
	unsigned short int rowCS,xRowCS;  //행 체크썸 값
	unsigned short int colCS[SIZE_ROW+2] = {0};  //열 체크썸값
	unsigned short int xColCS[SIZE_ROW+2] = {0};
	typedef struct error_loc {   //변조 위치와 수준을 알기위한 구조체
		int row,col;
		int diff;
	}Error_Loc;
	Error_Loc detected_error[4] = {{0,0,0}};

	while((rCount = fread(input_data[noRow],sizeof(char),SIZE_ROW+2,fpr))>10)
		noRow++;                      //데이터를 읽어와서  배열에 넣음

	for(j=0;j<SIZE_ROW;j++)
	{
		xColCS[j] = (unsigned short)(0xFF&input_data[noRow-2][j]);  //전달받은 열 체크값
		xColCS[j] +=(0xFF00 & (input_data[noRow-1][j]<<8));
	}

	int numError = 0;  //에러 개수

	for( i = 0;i<noRow-2;i++){
		xRowCS = (unsigned short)(0xFF & input_data[i][SIZE_ROW]);  //전달받은 행 체크값
		xRowCS +=(0xFF00 & (input_data[i][SIZE_ROW+1]<<8));
		rowCS = 0;
		for(j=0;j<SIZE_ROW;j++){
			rowCS+=input_data[i][j];        //전달받은 데이터로 다시 행 체크값 계산
			colCS[j] += input_data[i][j];   // 열 체크값 계산
		}
		if(xRowCS != rowCS){                   // 새로 계산한 값과 받은 값이 다르다면
			printf("%d Row error detected\n",i);
			detected_error[numError].row = i;    //행번호와 차이값을 저장
			detected_error[numError].diff = xRowCS - rowCS;
			numError++;  //에러개수 추가
		}
	}
	for(j=0;j<SIZE_ROW;j++){
		if(xColCS[j] != colCS[j]){   // 열 체크값이 받은것과 다를때
			for(i=0;i<numError;i++){
				if(detected_error[i].diff == (xColCS[j] - colCS[j])){  //차이값을 rowCS차이값과 비교
					detected_error[i].col = j;   //같은게 있으면 해당 열번호 저장
					break;
				}
			}
			if(i==numError){                 //같은 차이값을 발견하지못함(오류검출 실패)
				//printf("error correction fail\n");
			}else{                                                                           //오류검출 성공
				int pos = (SIZE_ROW+2)*detected_error[i].row+detected_error[i].col;  
				printf("%int error correction(%d) %u ->",i,
															pos,
															input_data[detected_error[i].row][detected_error[i].col]
															);
				input_data[detected_error[i].row][detected_error[i].col] += detected_error[i].diff;  //해당 위치에 차이값을 더해서 복구
				printf("%u\n",input_data[detected_error[i].row][detected_error[i].col]);
			}
		}
	}
	for(i=0;i<noRow-2;i++)    //임시파일에 체크썸값을 뺀 데이터부분만 출력
	{
		for(j=0;j<SIZE_ROW;j++){
			if(input_data[i][j] == '\0')
				break;
			fprintf(tempFp,"%c",input_data[i][j]);
		}
	}
	fclose(tempFp);  //임시파일 포인터 종료
	fpr = fopen("temp.txt","r+");  //임시파일을 가리킴
	return fpr;
}


void print(FILE* fpr, FILE *fpw){ // '/' 만날때 까지 출력하는 경우 사용
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
		else if(c=='\n')  //개행문자 만나 해당파트 끝난경우
		{
			eofItem = 1;
			break;
		}
		else
			hex[i++] = c;  //구분자 만나거나 줄이 끝나기 전까지 읽음

	}


	for(k = 0; hex[k] != '\0'; k++){  //변환
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
		fprintf(fpw, "\nFRIEND%d ID: ", i);  //id
		print(fpr, fpw);
		fprintf(fpw, "\nFRIEND%d NAME: ", i);  //이름
		print(fpr, fpw);
		fprintf(fpw, "\nFRIEND%d GENDER: ", i);  //성별
		while((c = fgetc(fpr)) != '/'){         // F,M 풀어서 적음
			if(c == 'F') 
				fprintf(fpw, "FEMALE");
			else fprintf(fpw, "MALE");
		}
		fprintf(fpw, "\nFRIEND%d AGE: ", i);  //나이
		while(1)
		{
			c=fgetc(fpr);
			if(c=='|' || c=='\n')  //친구정보 끝날때 까지 읽어서 출력
				break;
			fprintf(fpw, "%c", c);
		}
		fprintf(fpw, "\n");
		i++;  //친구 번호 증가
		if(c=='\n')
			break;
	}
}

void user(FILE *fpr, FILE *fpw)
{
	int c, n;
	fprintf(fpw, "*USER STATUS*\n");

	fprintf(fpw, "ID: ");  //id
	print(fpr, fpw);  // 단순 입력은 print 사용

	fprintf(fpw, "\nNAME: ");   //이름
	print(fpr, fpw);

	fprintf(fpw, "\nGENDER: ");  //성별
	while((c = fgetc(fpr)) != '/'){
		if(c == 'F') 
			fprintf(fpw, "FEMALE");
		else fprintf(fpw, "MALE");
	}

	fprintf(fpw, "\nAGE: ");  //나이
	print(fpr, fpw);

	fprintf(fpw, "\nHP: ");  //hp, mp, coin의 경우 진수 변환하여 출력
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
		if(eofItem ==1)  //아이템이 몇종류 들어올지 모르기때문에 무한반복 후 개행문자 만나면 종료
			break;

		c = fgetc(fpr);
		if(c == 'A')          //각 알파벳들 아이템에 대입해 진수변환 후 출력
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
	char tmp;           //읽은 문자 임시저장
	char c;           //이전에 읽은 문자, 출력용
	char str[10]={0};
	int a;
	int check_str = 0; // 문자열 체크변수 (1 = 시작, 2 = 끝,3 = 문자열반복 숫자 검사용,  0 = 문자열 구간X)
	int n=0; // 문자열 위치 가리키는 변수
	int check=0;                          //이전에 읽은 값이 문자인지 숫자인지 체크
	while((tmp = fgetc(fpr)) != EOF)     //끝까지 1글자씩 읽어옴
	{
		if(check_str == 1) // 문자열 구간,  문자열에 값 저장 
		{
			if(tmp >= 'A' && tmp <='Z')  //문자
			{
				if(check == TEXT_TYPE)  //텍스트 타입일때
				{
					str[n] = c;    //이전 문자 출력
					n++;    //문자배열 위치 증가
				}
				else if(check == NUM_TYPE)  //숫자타입일때
				{
					for(i=n; i<n+a; i++)   //숫자만큼 반복해서 문자 출력
						str[i] = c;
					n=i;
				}
				c = tmp;             //방금 읽은 문자 c에 저장
				check = TEXT_TYPE;  //텍스트 타입저장
			}
			else if(tmp>='0' && tmp <='9') //숫자
			{
				if(check == TEXT_TYPE)  //문자타입이면 숫자 저장
				{
					a = tmp-'0';
				}
				if(check == NUM_TYPE)  //숫자타입 (10회 이상 반복의 경우)
				{
					a=a*10;
					a+=tmp-'0';
				}
				check = NUM_TYPE;  //숫자타입변경
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
				check_str = 2;   //문자열 끝 표시
				str[n] = '\0';  //문자배열 끝에 널
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
				switch(tmp)           //특수문자 숫자 대입
				{
					case 38: c='0'; break;
					case 39: c='1'; break;
					case 40: c='2'; break;
					case 41: c='3'; break;
					case 42: c='4'; break;
					case 43: c='5'; break;
					case 44: c='6'; break;
					case 45: c='7'; break;
					case 46: c='8'; break;
					case 47: c='9'; break;
				}
				check = TEXT_TYPE;
			}
		}
		else if(check_str == 2) // 문자열이 끝난 구간, 다음 나오는 숫자에 따른 문자열 반복
		{
			a=tmp-'0';      //숫자대입
			check_str = 0;  // 각 값들 초기화
			check = STRNUM_TYPE;  //문자열이 10회 이상반복일 경우를 위해 확인용
			n=0;
		}
		else if(check_str == 0) // 문자열 구간x 일반적인 상황
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
				else if(check == STRNUM_TYPE) //문자열 숫자타입
				{
					for(i=0;i<a;i++)
						fprintf(fpw,"%s",str);  //숫자만큼 문자열 반복
					str[0] = '\0';  //문자열 초기화
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
				if(check == NUM_TYPE || check == STRNUM_TYPE) //이전문자 숫자일때
				{
					a=a*10;        //두자리수 이상의 경우임
					a+= tmp-'0';
				}
				if(check != STRNUM_TYPE)  //문자열반복이 아닌 일반적인 상황
					check = NUM_TYPE;  //체크타입 변경
			}
			else if(tmp == '"') // 문자열 시작 " 만남
			{
				if(check == TEXT_TYPE)  //이전것들 출력
				{
					fputc(c,fpw);
				}
				else if(check == NUM_TYPE)
				{
					for(i=0;i<a;i++)
						fputc(c,fpw);
				}
				else if(check == STRNUM_TYPE)
				{
					for(i=0;i<a;i++)
						fprintf(fpw,"%s",str);
					str[0] = '\0';
				}
				check_str = 1; //  문자열 check 변수 1로 변경 -> 다음 "가 나올때까지 배열로 입력받음
				check = 0; // check 타입 초기화
			}
			else if(tmp == '\n')  //줄바뀜
			{
				if(check == TEXT_TYPE)  //이전것들 출력
					fputc(c,fpw);
				else if(check == NUM_TYPE)
				{
					for(int i = 0;i<a;i++)
						fputc(c,fpw);
				}
				else if(check == STRNUM_TYPE)
				{
					for(i=0;i<a;i++)
						fprintf(fpw,"%s",str);
					str[0] = '\0';
				}
				fputc(tmp,fpw);  //개행문자도 출력
				check = 0; //체크 초기화
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
				else if(check == STRNUM_TYPE)
				{
					for(i=0;i<a;i++)
						fprintf(fpw,"%s",str);
					str[0] = '\0';
				}
				switch(tmp)
				{
					case 38: c='0'; break;
					case 39: c='1'; break;
					case 40: c='2'; break;
					case 41: c='3'; break;
					case 42: c='4'; break;
					case 43: c='5'; break;
					case 44: c='6'; break;
					case 45: c='7'; break;
					case 46: c='8'; break;
					case 47: c='9'; break;
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
	FILE* fpw = fopen(argv[2],"w+");  //디코딩 후 결과 데이터 파일

		fpr = check(fpr);  //신뢰성검사
printf("체크끝");
	user(fpr, fpw);  //user 부분 해석
	items(fpr,fpw);  //item 해석
	friend(fpr, fpw);  //friend 해석
	description(fpr,fpw);  //description 해석

		remove("temp.txt");  //임시파일 삭제
	fclose(fpr); //파일 포인터 삭제
	fclose(fpw);
	return 0;
}
