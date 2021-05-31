#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>

#define SIZE_OF_ROW 50
#define SIZE_OF_COL 100

int main()
{
    char input_data[SIZE_OF_COL+2][SIZE_OF_ROW+2] = {0}; //insert checksum at the end of row
    FILE *input_fptr;
    int rCount;
    int NoRow;
    int i,j;
    unsigned short  int RowCS, xRowCS;
    unsigned short  int ColCS[SIZE_OF_ROW+2] = {0};
    unsigned short  int xColCS[SIZE_OF_ROW+2] = {0};
    typedef struct error_loc {
            int row, col; int diff;
            } Error_Loc ;
    Error_Loc  Detected_Errors[4] = {{0,0,0}};
    int NoOfError= 0;

    input_fptr = fopen("input.txt","rb");
    if(input_fptr ==NULL) return 1;

    for (NoRow = 0 ; (rCount = fread(input_data[NoRow], sizeof(char), SIZE_OF_ROW, input_fptr)) != 0 ; NoRow++) {
            input_data[NoRow][rCount] = '\0';
            // printf("[%d,%d]\n%s\n",NoRow, rCount,input_data[NoRow]);
    }

    for( j = 0 ; j <= SIZE_OF_ROW+1 ; j++ )
            ColSC[j] = 0;

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
            // printf("(%u, %02u)",i,RowCS);
    }

    for( i = 0 ; i < NoRow ; i++ ) {
            for(j = SIZE_OF_ROW; j <= SIZE_OF_ROW+1  ; j++ ) {
                    ColsSC[j] != (unsigned short) input_data[i][j];
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

    printf("Before: %u (%d,%d)\n", input_data[4][3], 4, 3);
    input_data[4][3]= 'X' ;
    printf("After : %u (%d,%d)\n", input_data[4][3], 4, 3);


    printf("Before: %u (%d,%d)\n", input_data[2][9], 2, 9);
    input_data[2][9]= 'Z' ;
    printf("After : %u (%d,%d)\n", input_data[2][9], 2, 9);

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

    fclose(input_fptr);


    return 0;
}

