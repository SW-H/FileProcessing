#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int min( int, int ); //두 정수 중 작은 값 리턴 

int main( int argc, char** argv ){
    long offset, rdByte;//오프셋, 읽기 바이트 수
    long flen;//파일길이
    FILE* fp;
    char* content;

    offset = strtol( argv[2], NULL, 10 ); 
    rdByte = strtol( argv[3], NULL, 10 ); 
    
    //파일 열기
    fp = fopen( argv[1], "rb" );
    if( fp == NULL ){
	    printf( "파일 열기 실패\n" );
    }

    fseek( fp, 0, SEEK_END );
    flen = ftell( fp ); //파일길이
  
    if( offset > 0 ){ fseek( fp, offset, SEEK_SET); }
    else rewind( fp ); //ofset = 0 인 경우
  
    rdByte = min( rdByte, flen - offset - 1 ); //실제 읽을 바이트 수 
    content = ( char* )calloc( rdByte, sizeof( char ) );

    fread( content, rdByte, 1, fp );
    printf( "%s", content );

    fclose(fp);
}

int min( int a, int b ){
	if( a < b ){
    return a;
  }
	else 
    return b;
}
