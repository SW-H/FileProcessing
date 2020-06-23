#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main( int argc, char** argv ){
    FILE* fp;
	  long offset,fLen;
    
	  fp = fopen( argv[1], "r+"); //파일 열기
    offset = strtol( argv[2], NULL, 10);
	  fseek( fp, 0, SEEK_END );
	  fLen = ftell( fp );
	
	  fseek ( fp, offset , SEEK_SET ); //오프셋 위치로 파일포인터 이동
	
    if( offset > fLen ){ 
		  printf( "[error]offset이 파일을 벗어납니다\n"); 
	  } 

    else 
		  fwrite( argv[3], strlen(argv[3]), 1, fp ); //데이터 덮어쓰기 

    //파일 닫기 
	  fclose(fp);
	
}
