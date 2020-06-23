#include <stdio.h>
#include <string.h>
#include <stdlib.h>

int main( int argc, char** argv ){
    int contentLen; //원본 파일의 길이
    int cpyn; //복사할 횟수
    FILE* orgFp; //원본파일 
    FILE* cpyFp; // ->복사할 파일 
    char* content; //buffer
    
    //파일 열기
    orgFp = fopen( argv[1], "rb+" );
    cpyFp = fopen( argv[2], "wt" );
 
    if( orgFp == NULL ) { printf( "원본파일 열기 실패" ); }
    if( cpyFp == NULL ) { printf( "복사할 파일 열기 실패" ); }
 
    fseek( orgFp, 0, SEEK_END );
    contentLen = ftell( orgFp ); //원본파일의 길이 측정
    
    content = ( char* )calloc( contentLen, sizeof(char) );
    cpyn = contentLen / 100; 
 
    rewind( orgFp );

    //복사	
    fread( content, 100, cpyn+1, orgFp );
    fwrite( content, strlen( content ), 1, cpyFp );
 
    //파일 닫기
    fclose( orgFp );
    fclose( cpyFp );
}




