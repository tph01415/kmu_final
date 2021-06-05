#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

/*----------------
----------------*/


#define TRUE 1
#define FALSE 0
#define MAX_ID_SIZE 255
#define MAX_NAME_SIZE 255
#define MAX_BUF_SIZE 512

void trans(FILE * fp1, FILE * fp2);
int checkItem(char * str);

int debug = 1;
int check = 0;
char hexstr[255];
char desc_str[MAX_BUF_SIZE];

int main(int argc, char * argv[])
{
    FILE * readfp, * writefp;
    // readfp濡?argv[1]?쇰줈遺??諛쏆븘???뚯씪???쎄린 紐⑤뱶濡?fopen
    if (!(readfp = fopen(argv[1], "r"))) {
        fprintf(stderr, "Cannot open %s file.\n", argv[1]);
        exit(EXIT_FAILURE);
    }
    // writefp濡?argv[2]濡쒕???諛쏆븘???뚯씪???곌린 紐⑤뱶濡?fopen
    if (!(writefp = fopen(argv[2], "w"))) {
        fprintf(stderr, "Cannot open %s file.\n", argv[2]);
        exit(EXIT_FAILURE);
    }
    trans(readfp, writefp); 

    fclose(readfp);
    fclose(writefp);
    
    return 0;
}

int checkItem(char * str)
{
    if (strcmp(str, "BOMB") == 0) return 1;
    else if (strcmp(str, "POTION") == 0) return 2;
    else if (strcmp(str, "CURE") == 0) return 3;
    else if (strcmp(str, "BOOK") == 0) return 4;
    else if (strcmp(str, "SHIELD") == 0) return 5;
    return 6;
}

char * change_hex(char * str){
    int value = atoi(str);
    
    sprintf(hexstr,"%x",value);
	
	check = 0;
	return hexstr;
}

int getrepeat(char *str, char *lcs, int count)
{
    char *ptr = str;
    int repeat = 0;
    if (strlen(str) < count)
        return 0;
    while (strncmp(ptr,lcs,count) == 0)
    {
        ptr += count;
        repeat++;    
    }
    return repeat;        
}

char *runlengthencoding(char *str, char *rle, int count)
{
    char *ptr = str;
    char *ptrend = &str[strlen(str)];
    char lcs[100];
    char encoding[100];
    int repeat;
    strcpy(rle,"");
    
    if (count == 0)
    {
        int len = strlen(str);
        for (int i = 0; i < len; i++)
        {
            if (isdigit(str[i]))
                rle[i] = str[i]-10;
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
        if (repeat > 1)
        {
            if (count > 1)
            {
                sprintf(encoding, "\"%s\"%d", lcs, repeat);
            }
	    else
            {
                sprintf(encoding, "%s%d", lcs,repeat);
            }
            strcat(rle, encoding);
            ptr = ptr + count * repeat;
            }
	else
        {
            sprintf(encoding, "%c", *ptr);
            strcat(rle, encoding);
            ptr++;
        }
    }
    
    return rle;    
}

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

void trans(FILE * fp1, FILE * fp2)
{
    int i, loop = TRUE;
    char buf[MAX_BUF_SIZE], tmp[MAX_BUF_SIZE], rle[MAX_BUF_SIZE];
    char * bufptr, * itemStr;
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
                bufptr = strtok(NULL, "\n");
                for (i = 0 ; i < strlen(bufptr) ; i ++) bufptr[i] = bufptr[i + 1];
                fgets(tmp, MAX_BUF_SIZE, fp1);
                if (tmp[0] != '\n') tmp[strlen(tmp) - 1] = '\0';
                if (tmp[0] == '\n') {
                    fprintf(fp2,"%s\n", change_hex(bufptr));
                    break;
                } else {
                    if (strcmp(bufptr, "FEMALE") == 0) fprintf(fp2, "F/");
                    else if (strcmp(bufptr, "MALE") == 0) fprintf(fp2, "M/");

                    else if (check == 1) { 
                       fprintf(fp2,"%s/", change_hex(bufptr)); 
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
                itemStr = bufptr;
                bufptr = strtok(NULL, "\n");
                for (i = 0 ; i < strlen(bufptr) ; i ++) bufptr[i] = bufptr[i + 1];
                fgets(tmp, MAX_BUF_SIZE, fp1);
                if (tmp[0] != '\n') tmp[strlen(tmp) - 1] = '\0';
                if (tmp[0] == '\n') {
                    if (checkItem(itemStr) == 1) fprintf(fp2, "A%s\n", change_hex(bufptr));
                    else if (checkItem(itemStr) == 2) fprintf(fp2, "B%s\n", change_hex(bufptr));
                    else if (checkItem(itemStr) == 3) fprintf(fp2, "C%s\n", change_hex(bufptr));
                    else if (checkItem(itemStr) == 4) fprintf(fp2, "D%s\n", change_hex(bufptr));
                    else if (checkItem(itemStr) == 5) fprintf(fp2, "E%s\n", change_hex(bufptr));
                    else if (checkItem(itemStr) == 6) fprintf(fp2, "F%s\n", change_hex(bufptr));
                    break;
                } else {
                    if (checkItem(itemStr) == 1) fprintf(fp2, "A%s/", change_hex(bufptr));
                    else if (checkItem(itemStr) == 2) fprintf(fp2, "B%s/", change_hex(bufptr));
                    else if (checkItem(itemStr) == 3) fprintf(fp2, "C%s/", change_hex(bufptr));
                    else if (checkItem(itemStr) == 4) fprintf(fp2, "D%s/", change_hex(bufptr));
                    else if (checkItem(itemStr) == 5) fprintf(fp2, "E%s/", change_hex(bufptr));
                    else if (checkItem(itemStr) == 6) fprintf(fp2, "F%s/", change_hex(bufptr));
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
                bufptr = getrle(bufptr,rle);
                fprintf(fp2, "%s\n", bufptr);
            }

            loop = FALSE;
        } 
    }
}
