#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>

#define SIZE_OF_ROW 50 //row크기
#define SIZE_OF_COL 100 //col크기

int main()
{
	
    char input_data[SIZE_OF_COL+2][SIZE_OF_ROW+2] = {0}; //데이터가 들어가는 2차원 배열
    FILE *input_fptr; //파일 포인터
    int rCount; //카운터
    int NoRow;//number of row
    int i,j;
    unsigned short  int RowCS, xRowCS; //...checksum 값을 빼내서 비교하는 배열
    unsigned short  int ColCS[SIZE_OF_ROW+2] = {0}; //배열 초기화(전체 배열의 크기는 checksum값이 들어가는 칸(+2)
    unsigned short  int xColCS[SIZE_OF_ROW+2] = {0}; // 동일
    typedef struct error_loc { //에러 위치와 에러 차이가 들어가는 구조체
            int row, col; int diff;
            } Error_Loc ;
    Error_Loc  Detected_Errors[4] = {{0,0,0}}; //초기화 최대 에러는 4개까지
    int NoOfError= 0;

    input_fptr = fopen("data.txt","rb");
    if(input_fptr ==NULL) return 1;

    for (NoRow = 0 ; (rCount = fread(input_data[NoRow], sizeof(char), SIZE_OF_ROW, input_fptr)) != 0 ; NoRow++) {
            input_data[NoRow][rCount] = '\0';
			//데이터를 2차원 배열에 넣고 체크섬 값이 들어갈 부분에는 NULL\0값을 넣어준다.
		
            // printf("[%d,%d]\n%s\n",NoRow, rCount,input_data[NoRow]);
    }

    for( j = 0 ; j <= SIZE_OF_ROW+1 ; j++ )
            ColCS[j] = 0;
			//2중 for문을 이용해서 체크섬 값을 계산 및 입력

    for( i = 0 ; i < NoRow ; i++ ) {
            input_data[i][SIZE_OF_ROW] = 0 ;
            input_data[i][SIZE_OF_ROW+1] = 0 ;
            RowCS =0;
            for( j = 0 ; j < SIZE_OF_ROW ; j++ ) {
                    /*
                    if( i == 2 && j == 6 )
                            printf("\nBF [2,6] \"%c %u\" RC = %u, CS = %u\n", input_data[2][6],
                                            input_data[2][6],
                                            RowCS,
                                            input_data[NoRow][j]);
                    */
                    RowCS += (unsigned short) input_data[i][j];
                    ColCS[j] += (unsigned short) input_data[i][j];

					//각 칸 마다 마지막 row와 col에+=해서 아스키 값을 더해주면서 입력한다


                    /*
                    if( i == 2 && j == 6 )
                            printf("\nBF [2,6] \"%c %u\" RC = %u, CS = %u\n", input_data[2][6],
                                            input_data[2][6],
                                            RowCS,
                                            input_data[NoRow][j]);
                    */
            }
            input_data[i][SIZE_OF_ROW] = (unsigned char)(0xFF & RowCS);
            input_data[i][SIZE_OF_ROW+1] = (unsigned char)(0xFF & (RowCS >> 8));
			
			//값을 1바이트 1바이트로 나눠서 0xFF 0xFF 저장(오버플로우 방지)


            // printf("(%u, %02u)",i,RowCS);
    }

    for( i = 0 ; i < NoRow ; i++ ) {
            for(j = SIZE_OF_ROW; j <= SIZE_OF_ROW+1  ; j++ ) {
                    ColCS[j] != (unsigned short) input_data[i][j];
			


            }
    }

    for( j = 0 ; j <= SIZE_OF_ROW+1 ; j++ ) {
            input_data[NoRow][j] = (unsigned char) (0xFF & ColCS[j]);
            input_data[NoRow+1][j] = (unsigned char) (0xFF & (ColCS[j] >> 8));
            // printf("Column Checksum = (%d, %d[%u,%u])\n", j, ColCS[j],input_data[NoRow][j], input_data[NoRow+1][j]);
    }

    /*
    printf("\nCoum checksum****\n");
    for( j = 0 ; j < SIZE_OF_ROW ; j++ )
            printf("%u ",input_data[NoRow][j]);
    printf("\n");
    */

    printf("Before: %u (%d,%d)\n", input_data[2][8], 2, 8);
    input_data[2][8]= 'X' ;
    printf("After : %u (%d,%d)\n", input_data[2][8], 2, 8);


    printf("Before: %u (%d,%d)\n", input_data[3][9], 3, 9);
    input_data[3][9]= 'Z' ;
    printf("After : %u (%d,%d)\n", input_data[3][9], 3, 9);

    printf("\n");


    for( j = 0 ; j < SIZE_OF_ROW + 2 ; j++ ) {
            xColCS[j] = (unsigned short)(0xFF & input_data[NoRow][j]);
            xColCS[j] += (0xFF00 & (input_data[NoRow+1][j] << 8));
            // printf("Column Checksum = (%d, %d[%u,%u])\n", j, xColCS[j],input_data[NoRow][j], input_data[NoRow+1][j]);
            ColCS[j]=0;
    }

    NoOfError = 0;

    for( i = 0 ; i < NoRow ; i++ ) {
            xRowCS = (unsigned short) (0xFF & input_data[i][SIZE_OF_ROW]);
            xRowCS += (0xFF00 & (input_data[i][SIZE_OF_ROW+1] << 8)) ;
            RowCS =0;
            for( j = 0 ; j < SIZE_OF_ROW ; j++ ) {
                    /*
                    if( i == 2 && j == 6 )
                            printf("\nBF [2,6] \"%c %u\" RC = %u, CS = %u\n", input_data[2][6],
                                            input_data[2][6],
                                            RowCS,
                                            input_data[NoRow][j]);
                    */
                    RowCS += input_data[i][j];
                    ColCS[j] += input_data[i][j];
                    /*
                    if( i == 2 && j == 6 )
                            printf("\nBF [2,6] \"%c %u\" RC = %u, CS = %u\n", input_data[2][6],
                                            input_data[2][6],
                                            RowCS,
                                            input_data[NoRow][j]);
                    */
            }
            if ( xRowCS != RowCS ) {
                    printf("%d Row error detected with difference %d\n", i , xRowCS-RowCS);
                    Detected_Errors[NoOfError].row = i;
                    Detected_Errors[NoOfError].diff = xRowCS-RowCS;
                    NoOfError++;
            }

            // printf("(%02u,xRowCS = [%02u], RowCS = [%02u])",i,xRowCS, RowCS);
    }

    for( j = 0 ; j < SIZE_OF_ROW ; j++ ) {
            if ( xColCS[j] != ColCS[j] ) {
                    for( i = 0 ; i < NoOfError ; i++ ) {
                            if (Detected_Errors[i].diff == (xColCS[j]-ColCS[j])) {
                                    Detected_Errors[i].col = j;
                                    break;
                            }
                    }
                    printf("%d Column error detected with difference %d\n",j, xColCS[j]-ColCS[j]);
                    if( i == NoOfError )
                            printf(" <<<<<Error correction impossible error\n");
                    else
                            printf(" >>>>>Error Correction: %u(%d,%d) -->%u \n", input_data[Detected_Errors[i].row][Detected_Errors[i].col],
                                                                          Detected_Errors[i].row, Detected_Errors[i].col,
                                                                          input_data[Detected_Errors[i].row][Detected_Errors[i].col] +
                                                                          Detected_Errors[i].diff);
            }
   }


    /*
    printf("\nCoum checksum****\n");
    for( j = 0 ; j < SIZE_OF_ROW ; j++ )
            printf("%u ",input_data[NoRow][j]);
    printf("\n");
    */

	for( i = 0; i<NoOfError; i++){
		input_data[Detected_Errors[i].row][Detected_Errors[i].col] =  input_data[Detected_Errors[i].row][Detected_Errors[i].col] - (Detected_Errors[i].diff);
		
		printf("\n Error collection has made !!!  %u(%d,%d)", input_data[Detected_Errors[i].row][Detected_Errors[i].col],
															  Detected_Errors[i].row,
															  Detected_Errors[i].col);
	}

    fclose(input_fptr);


    return 0;
}

