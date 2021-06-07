#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define TRUE 1
#define FALSE 0
#define MAX_ID_SIZE 255
#define MAX_NAME_SIZE 255
#define MAX_BUF_SIZE 512

//체크섬 배열
#define SIZE_OF_ROW 12 //row크기
#define SIZE_OF_COL 100 //col크기

int check = 0;
char hexstr[255];
char desc_str[MAX_BUF_SIZE];

// 문자열로 아이템을 판별 후 int형을 반환하는 함수
int checkItem(char * str)
{
    if (strcmp(str, "BOMB") == 0) return 1;
    else if (strcmp(str, "POTION") == 0) return 2;
    else if (strcmp(str, "CURE") == 0) return 3;
    else if (strcmp(str, "BOOK") == 0) return 4;
    else if (strcmp(str, "SHIELD") == 0) return 5;
    return 6;
}

// 10진수를 16진수로 변환하는 함수
char * change_hex(char * str){
    int value = atoi(str);
    
    sprintf(hexstr,"%x",value);
	
	check = 0;
	return hexstr;
}

//문자열의 반복횟수를 표시하는 함수
int getrepeat(char *str, char *lcs, int count)
{
    char *ptr = str;
    int repeat = 0;
    if (strlen(str) < count) return 0;
    while (strncmp(ptr,lcs,count) == 0) {
        ptr += count;
        repeat++;    
    }
    return repeat;        
}

//Description의 문자열을 압축하는 함수
char *runlengthencoding(char *str, char *rle, int count)
{
    char *ptr = str;
    char *ptrend = &str[strlen(str)];
    char lcs[100];
    char encoding[100];
    int repeat;
    strcpy(rle,"");
    
    if (count == 0) //소개글에서 11, 22와 같은 예시 변환
    {
        int len = strlen(str);
        for (int i = 0; i < len; i++)
        {
            if (isdigit(str[i])) //아스키 코드 판별
                rle[i] = str[i]-10; //아스키코드에서 -10을한 문자로 표시
            else
                rle[i] = str[i];
        }
        
        rle[len] = '\0';

        return rle;
    }

    while (ptr < ptrend)
    {
        strncpy(lcs, ptr, count);
        lcs[count] = '\0';
        repeat = getrepeat(ptr, lcs, count);
	//repeat값이 있을 경우
        if (repeat > 1)
        {
            if (count > 1)
            {
                sprintf(encoding, "\"%s\"%d", lcs, repeat); //BBC와 같은 문자열이 여러번 반복될 때, "BBC"로 출력
            }
	    else
            {
                sprintf(encoding, "%s%d", lcs,repeat); //동일한 문자열과 반복횟수 출력
            }
            strcat(rle, encoding);
            ptr = ptr + count * repeat;
            }
	else
        {
            sprintf(encoding, "%c", *ptr); //문자열이 하나일 때
            strcat(rle, encoding);
            ptr++;
        }
    }
    
    return rle;    
}

//압축한 문자열을 가져오는 함수
char *getrle(char str[], char rle[])
{
    char strlcs[100];
    
	strcpy(strlcs, str);
    for (int count = 0; count <= 4; count++)
    {
        runlengthencoding(strlcs, rle, count);
        strcpy(strlcs, rle);
    }

    return rle;
}

// 원본 데이터 압축하는 함수
void trans(char file1[], char file2[])
{
    FILE * fp1, * fp2;
    // readfp로 argv[1]으로부터 받아온 파일을 읽기 모드로 fopen
    if (!(fp1 = fopen(file1, "r"))) {
        fprintf(stderr, "Cannot open %s file.\n", file1);
        exit(EXIT_FAILURE);
    }
    // writefp로 argv[2]로부터 받아온 파일을 쓰기 모드로 fopen
    if (!(fp2 = fopen(file2, "w"))) {
        fprintf(stderr, "Cannot open %s file.\n", file2);
        exit(EXIT_FAILURE);
    }

   int i, loop = TRUE;
    char buf[MAX_BUF_SIZE], tmp[MAX_BUF_SIZE], rle[MAX_BUF_SIZE];
    char * bufptr, * itemStr;
    while (loop) {
        if (strcmp(buf, "*DESCRIPTION*") != 0) {    // 버퍼에 담긴 문자열이 "*DESCRIPTION*"이 아닐경우 파일에서 문자열을 가지고와 개행을 없앰
            fgets(buf, MAX_BUF_SIZE, fp1);           
            buf[strlen(buf) - 1] = '\0';        
        }     
        if (strcmp(buf, "*USER STATUS*") == 0) {    // 버퍼에 담긴 문자열이 "*USER STATUS*"일 경우
            fgets(buf, MAX_BUF_SIZE, fp1);          
            buf[strlen(buf) - 1] = '\0';            // 버퍼에 담긴 문자열의 개행을 없앰
            while (1) {                             // 무한루프
                bufptr = strtok(buf, ":");          // 버퍼에 담긴 문자열을 :로 끊어서 저장
				if (strcmp(bufptr,"HP") == 0 || strcmp(bufptr,"MP") == 0) check = 1;
                bufptr = strtok(NULL, "\n");        // 버퍼포인터에다 압축데이터에 저장될 부분만 끊어서 저장
                for (i = 0 ; i < strlen(bufptr) ; i ++) bufptr[i] = bufptr[i + 1];  // 주어진 데이터의 양식이 :후 공백이 있으므로 공백을 제거하기 위함
                fgets(tmp, MAX_BUF_SIZE, fp1);      // 템프에다가 문자열을 가지고 옴
                if (tmp[0] != '\n') tmp[strlen(tmp) - 1] = '\0';    // 템프가 개행만을 가지고 있지 않다면 마지막의 개행을 없앰
                if (tmp[0] == '\n') {               // 템프가 개행일 경우에는 다음 소캣으로 넘어가는 과정이므로 따로 구분
                    fprintf(fp2,"%s\n", change_hex(bufptr));
                    break;
                } else {
                    if (strcmp(bufptr, "FEMALE") == 0) fprintf(fp2, "F/");          // 성병을 나타내는 경우에는 앞글자만 저장
                    else if (strcmp(bufptr, "MALE") == 0) fprintf(fp2, "M/");
                    else if (check == 1) { 
                       fprintf(fp2,"%s/", change_hex(bufptr)); 
                    }
                    else fprintf(fp2, "%s/", bufptr);
                    strcpy(buf, tmp);
                }
            }
        } else if (strcmp(buf, "*ITEMS*") == 0) {               // 버퍼에 담긴 문자열이 "*ITEMS*"일 경우
            fgets(buf, MAX_BUF_SIZE, fp1);          
            buf[strlen(buf) - 1] = '\0';
            while (1) {
                bufptr = strtok(buf, ":");                      
                itemStr = bufptr;
                bufptr = strtok(NULL, "\n");
                for (i = 0 ; i < strlen(bufptr) ; i ++) bufptr[i] = bufptr[i + 1];
                fgets(tmp, MAX_BUF_SIZE, fp1);
                if (tmp[0] != '\n') tmp[strlen(tmp) - 1] = '\0';
                if (tmp[0] == '\n') {                                   // 템프가 개행일 경우 위와 마찬가지로 다음 소켓으로 넘어가는 과정, 아이템을 확인해서 파일에 압축
                    if (checkItem(itemStr) == 1) fprintf(fp2, "A%s\n", change_hex(bufptr));
                    else if (checkItem(itemStr) == 2) fprintf(fp2, "B%s\n", change_hex(bufptr));
                    else if (checkItem(itemStr) == 3) fprintf(fp2, "C%s\n", change_hex(bufptr));
                    else if (checkItem(itemStr) == 4) fprintf(fp2, "D%s\n", change_hex(bufptr));
                    else if (checkItem(itemStr) == 5) fprintf(fp2, "E%s\n", change_hex(bufptr));
                    else if (checkItem(itemStr) == 6) fprintf(fp2, "F%s\n", change_hex(bufptr));
                    break;
                } else {                            // A, B, ... 순서대로 주어진 아이템을 나타냄                            
                    if (checkItem(itemStr) == 1) fprintf(fp2, "A%s/", change_hex(bufptr));
                    else if (checkItem(itemStr) == 2) fprintf(fp2, "B%s/", change_hex(bufptr));
                    else if (checkItem(itemStr) == 3) fprintf(fp2, "C%s/", change_hex(bufptr));
                    else if (checkItem(itemStr) == 4) fprintf(fp2, "D%s/", change_hex(bufptr));
                    else if (checkItem(itemStr) == 5) fprintf(fp2, "E%s/", change_hex(bufptr));
                    else if (checkItem(itemStr) == 6) fprintf(fp2, "F%s/", change_hex(bufptr));
                    strcpy(buf, tmp);
                }
            }
        } else if (strcmp(buf, "*FRIENDS LIST*") == 0) {        // 버퍼에 담긴 문자열이 "*FRIENDS LIST*"일 경우
            fgets(buf, MAX_BUF_SIZE, fp1);                      
            buf[strlen(buf) - 1] = '\0';
            while (1) {
                bufptr = strtok(buf, ":");
                bufptr = strtok(NULL, "\n");
                for (i = 0 ; i < strlen(bufptr) ; i ++) bufptr[i] = bufptr[i + 1];
                fgets(tmp, MAX_BUF_SIZE, fp1);
                if (tmp[0] != '\n') tmp[strlen(tmp) - 1] = '\0';
                if (tmp[0] == '\n') {
                    fprintf(fp2, "%s", bufptr);
                    fgets(buf, MAX_BUF_SIZE, fp1);
                    buf[strlen(buf) - 1] = '\0';
                    if (strcmp(buf, "*DESCRIPTION*") == 0) break;       // 소켓의 변화가 일어나는 경우 |을 파일에 입력하지 않기 위한 코드
                    fprintf(fp2, "|");  
                } else {                                                // 소켓 1 유저 정보에서 성별과 동일
                    if (strcmp(bufptr, "FEMALE") == 0) fprintf(fp2, "F/");
                    else if (strcmp(bufptr, "MALE") == 0) fprintf(fp2, "M/");
                    else fprintf(fp2, "%s/", bufptr);
                    strcpy(buf, tmp);
                }
            }
            fprintf(fp2, "\n");
        } else {
            while (!feof(fp1)) {        // 마지막 소켓이므로 파일의 끝에 도달하기 까지 반복
                if (fgets(buf,MAX_BUF_SIZE,fp1) == NULL) break;          
                bufptr = strtok(buf, "\n");
                bufptr = getrle(bufptr,rle);                // 버퍼에 담은 문자열을 개행까지 가져온 뒤 문자열을 압축하는 코드
                fprintf(fp2, "%s\n", bufptr);               
            }
            loop = FALSE;
        } 
    }
    
	fclose(fp1);
	fclose(fp2);
}


void CheckSumInsert(char file[]){
    FILE * input_fptr;
    // writefp로 argv[2]로부터 받아온 파일을 쓰기 모드로 fopen
    if (!(input_fptr = fopen(file, "r+"))) {
        fprintf(stderr, "Cannot open %s file.\n", file);
        exit(EXIT_FAILURE);
    }

	char input_data[SIZE_OF_COL+2][SIZE_OF_ROW+2] = {0}; //checksum 데이터가 들어가는 2차원 배열
    char buf[MAX_BUF_SIZE];
	int rCount; //카운터
    int NoRow;//number of row
    int i,j;
    unsigned short  int RowCS, xRowCS; //...checksum 값을 빼내서 비교하는 배열
    unsigned short  int ColCS[SIZE_OF_ROW+2] = {0}; //배열 초기화(전체 배열의 크기는 checksum값이 들어가는 칸(+2)
    unsigned short  int xColCS[SIZE_OF_ROW+2] = {0}; // 동일
    int NoOfError= 0;


	if(input_fptr ==NULL) exit(1);
    for (NoRow = 0 ; (rCount = fread(input_data[NoRow], sizeof(char), SIZE_OF_ROW, input_fptr)) != 0 ; NoRow++) {
            input_data[NoRow][rCount] = '\0';
			//데이터를 2차원 배열에 넣고 체크섬 값이 들어갈 부분에는 NULL\0값을 넣어준다.
			//printf("[%d,%d]\n%s\n",NoRow, rCount,input_data[NoRow]);
    }

    for( j = 0 ; j <= SIZE_OF_ROW+1 ; j++ )
            ColCS[j] = 0;

    for( i = 0 ; i < NoRow ; i++ ) {
            input_data[i][SIZE_OF_ROW] = 0 ;
            input_data[i][SIZE_OF_ROW+1] = 0 ;
            RowCS =0;
            for( j = 0 ; j < SIZE_OF_ROW ; j++ ) {
                    RowCS += (unsigned short) input_data[i][j];
                    ColCS[j] += (unsigned short) input_data[i][j];
					//각 칸 마다 마지막 row와 col에+=해서 아스키 값을 더해주면서 입력한다
			
			}

            input_data[i][SIZE_OF_ROW] = (unsigned char)(0xFF & RowCS);
            input_data[i][SIZE_OF_ROW+1] = (unsigned char)(0xFF & (RowCS >> 8));
			//printf("\nRow = %d, CS = %x,(%x,%x)", i, RowCS, (unsigned char)input_data[i][SIZE_OF_ROW], (unsigned char)input_data[i][SIZE_OF_ROW+1]);
			
			//값을 1바이트 1바이트로 나눠서 0xFF 0xFF 저장(오버플로우 방지)

	}
	
	for(i = 0; i < NoRow ; i++ ) {
		for(j=SIZE_OF_ROW ; j < SIZE_OF_ROW+2; j++){
			ColCS[j] += (unsigned short) input_data[i][j];
		}
	}
    for( j = 0 ; j <= SIZE_OF_ROW+1 ; j++ ) {
            input_data[NoRow][j] = (unsigned char) (0xFF & ColCS[j]);
            input_data[NoRow+1][j] = (unsigned char) (0xFF & (ColCS[j] >> 8));
			printf("\n Column = %d, RC = %u", j, ColCS[j]);
			
	}

	fseek(input_fptr,0,SEEK_SET);

    fwrite(&input_data[0][0],sizeof(char),sizeof(char)*(NoRow+2)*(SIZE_OF_ROW+2),input_fptr);

	fclose(input_fptr);
}

// 메인 함수
int main(int argc, char * argv[])
{
    trans(argv[1], argv[2]); 
	CheckSumInsert(argv[2]);
	
    return 0;
}
