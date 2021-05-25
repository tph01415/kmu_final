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

int main(int argc, char* argv[]){
	FILE * fp;
	fp = fopen(argv[1],"r");
	

}
