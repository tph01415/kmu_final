#include <stdio.h>
#include <stdlib.h>
#define SIZE_ROW 2
#define SIZE_COL 100

void check(FILE* fpr)
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

	while((rCount = fread(input_data[noRow],sizeof(char),SIZE_ROW+2,fpr))>1)
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
			for(i=0;i<numError;i++){
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
	fpr = fopen("temp.txt","r+");
}

int main()
{
	FILE* fpr = fopen("chk.txt","r+");
	check(fpr);


	fclose(fpr);
	return 0;
}

