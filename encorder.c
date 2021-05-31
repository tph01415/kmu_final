#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define TRUE 1
#define FALSE 0
#define MAX_ID_SIZE 255
#define MAX_NAME_SIZE 255
#define MAX_BUF_SIZE 512

void trans(FILE * fp1, FILE * fp2);

int debug = 1;
int check = 0;
char hexstr[255];
char desc_str[MAX_BUF_SIZE];

int main(int argc, char * argv[])
{
    FILE * readfp, * writefp;
    // readfp로 argv[1]으로부터 받아온 파일을 읽기 모드로 fopen
    if (!(readfp = fopen(argv[1], "r"))) {
        fprintf(stderr, "Cannot open %s file.\n", argv[1]);
        exit(EXIT_FAILURE);
    }
    // writefp로 argv[2]로부터 받아온 파일을 쓰기 모드로 fopen
    if (!(writefp = fopen(argv[2], "w"))) {
        fprintf(stderr, "Cannot open %s file.\n", argv[2]);
        exit(EXIT_FAILURE);
    }
    trans(readfp, writefp); 

    fclose(readfp);
    fclose(writefp);
    
    return 0;
}


char * change_hex(char * str){

    int value = atoi(str);
    
    sprintf(hexstr,"%x",value);
    if(check == 1) strcat(hexstr,"/");

    check = 0;
    return hexstr;
}


char * change_desc(char * str){
    char buffer[MAX_BUF_SIZE];
    int i=0, j=0,k=0;
    int count=1;
    int index=0;

    for(i=0; i<MAX_BUF_SIZE; i++)desc_str[i] ='\0';

    for(i=0; i<strlen(str); i++){
        if(str[i] == 0)
            break;


        for(j=i; j<strlen(str); j++){
            if(str[j] == str[j+1])
                count++;
            else
                break;
        }

        if(count == 1){
            desc_str[index]=str[i];
            index++;
        }
        else{
            if(isdigit(str[i]) != 0){  
                desc_str[index] = str[i] - 10;
            } else{
                desc_str[index]=str[i];
            }
            sprintf(buffer,"%d",count);
            while(buffer[k]!=0){
                desc_str[index+1+k]=buffer[k];
                k++;
            }   
            index=index+1;
            for(k=k;k>0;k--){
                index=index+1;
                }
            k=0;
        }               

        i = i+count-1;
        count =1;

    }

    return desc_str;
}

void trans(FILE * fp1, FILE * fp2)
{
    int i, loop = TRUE;
    char buf[MAX_BUF_SIZE], tmp[MAX_BUF_SIZE];
    char * bufptr;
    while (loop) {
        if (strcmp(buf, "*DESCRIPTION*") != 0) {
            fgets(buf, MAX_BUF_SIZE, fp1);           
            buf[strlen(buf) - 1] = '\0';        
        }     
        if (strcmp(buf, "*USER STATUS*") == 0) { 
            fgets(buf, MAX_BUF_SIZE, fp1);          
            buf[strlen(buf) - 1] = '\0';
            while (1) {           
                bufptr = strtok(buf, ":");
                if (strcmp(bufptr,"HP") == 0 || strcmp(bufptr,"MP") == 0) check = 1; 
                if(strcmp(bufptr,"COIN") == 0) check = 2;
                bufptr = strtok(NULL, "\n");
                for (i = 0 ; i < strlen(bufptr) ; i ++) bufptr[i] = bufptr[i + 1];
                fgets(tmp, MAX_BUF_SIZE, fp1);
                if (tmp[0] != '\n') tmp[strlen(tmp) - 1] = '\0';
                if (tmp[0] == '\n') {
                    fprintf(fp2,"%s", change_hex(bufptr));
                    fprintf(fp2, "\n");
                    break;
                } else {
                    if (strcmp(bufptr, "FEMALE") == 0) fprintf(fp2, "F/");
                    else if (strcmp(bufptr, "MALE") == 0) fprintf(fp2, "M/");

                    else if (check == 1) { 
                       fprintf(fp2,"%s", change_hex(bufptr)); 
                    }
                    else fprintf(fp2, "%s/", bufptr);
                    strcpy(buf, tmp);
                }
            }
        } else if (strcmp(buf, "*ITEMS*") == 0) {
            fgets(buf, MAX_BUF_SIZE, fp1);          
            buf[strlen(buf) - 1] = '\0';
            while (1) {
                bufptr = strtok(buf, ":");
                bufptr = strtok(NULL, "\n");
                for (i = 0 ; i < strlen(bufptr) ; i ++) bufptr[i] = bufptr[i + 1];
                fgets(tmp, MAX_BUF_SIZE, fp1);
                if (tmp[0] != '\n') tmp[strlen(tmp) - 1] = '\0';
                if (tmp[0] == '\n') {
                    fprintf(fp2, "%s\n", bufptr);
                    break;
                } else {
                    fprintf(fp2, "%s/", bufptr);
                    strcpy(buf, tmp);
                }
            }
        } else if (strcmp(buf, "*FRIENDS LIST*") == 0) {
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
                    if (strcmp(buf, "*DESCRIPTION*") == 0) break;
                    fprintf(fp2, "|");  
                } else {
                    if (strcmp(bufptr, "FEMALE") == 0) fprintf(fp2, "F/");
                    else if (strcmp(bufptr, "MALE") == 0) fprintf(fp2, "M/");
                    else fprintf(fp2, "%s/", bufptr);
                    strcpy(buf, tmp);
                }
            }
            fprintf(fp2, "\n");
        } else {
            while (!feof(fp1)) {
                fgets(buf, MAX_BUF_SIZE, fp1);          
                bufptr = strtok(buf, "\n");
                bufptr = change_desc(bufptr);
                fprintf(fp2, "%s\n", bufptr);
            }

            loop = FALSE;
        } 
    }
}
