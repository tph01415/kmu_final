#include <stdio.h>
#include <stdlib.h>
/*--------------
 Userstatus - 구조체
 
 
  --------------*/

typedef struct user_status{
	char id[255];
	char name[255];
	char gender;
	int age;
	char hp[20];
	char mp[20];
	char coin[20];
}

char * change_hex(int value){
	int pos =0, mod = 0,j =0;
	char result[20],temp[20];

	while(true){
		mod = value%16;
		if(mod < 10){
			temp[pos] = 48 + mod;
		}else{
			temp[pos] = 65 + (mod-10);
		
		}
		value = value / 16;

		pos++;

		if(value ==0) break;
	}
	for(int i = pos-1; i>=0;i--){
		result[j] = temp[i];
		j++;
	}

	return result;
}

int main(int argc, char* argv[]){
	FILE * input_fp; //인코딩할 파일
	FILE * output_fp;//인코딩이 출력될 파일

	input_fp = fopen(argv[1],"r");
	output_fp = fopen(argv[2],"w");
	



}
