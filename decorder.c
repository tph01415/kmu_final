#include <stdio.h>
#include <stldlib.h>

int main(int argc, char* argv[])
{
	FILE* fpr(argv[0],"r+");  //인코딩 후 데이터 파일
	FILE* fpw(argv[1],"w+");  //디코딩 후 데이터 파일
/*
	user();
	items();
	friend();
	description();
*/
	fclose(fpr);
	fclose(fpw);
	return 0;
}
