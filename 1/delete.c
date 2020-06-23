#include <stdio.h>
#include <stdlib.h>
#include <string.h>
int min( int, int );//두 정수 중 작은 값 반환 함수

int main( int argc, char** argv ){
	  FILE* fp = fopen( argv[1], "rb+" ); // 파일 열기
	  long offset = strtol( argv[2], NULL, 10 ); //오프셋 
	  long delByte = strtol( argv[3], NULL, 10 ); //삭제바이트수
	  long curPos; //파일포인터의 위치
  	int fLen; // 파일길이
	
  //삭제데이터 전후 파일내용
    char* buf;
	  char* prev;

	  fseek( fp, 0, SEEK_END );
	  fLen = ftell( fp ); //파일의 전체 길이
	
  	delByte = min( delByte, fLen - offset ); //실제 삭제될 바이트 수
	
	  buf = ( char* )calloc( fLen - offset, sizeof( char ) );
	  prev = ( char* )calloc( offset, sizeof( char ) );
	  rewind( fp );
	  fread( prev, offset, 1, fp ); //offset 이전까지 파일내용
	
	  fseek ( fp, offset + delByte, SEEK_SET ); //삭제데이터 뒷부분으로 파일포인터 이동
	  curPos = ftell( fp );//현재 파일포인터 위치

	  if( fLen - curPos != 0){ //삭제데이터 뒤에 파일내용이 있는 경우	
      fread ( buf, fLen-curPos, 1, fp ); //삭제데이터 뒷부분 파일내용
  	}	
		
    fclose( fp );
		fopen( argv[1], "wt" ); fclose( fp ); //원본파일의 내용을 지우기 위해 다시열기
	
		fp = fopen( argv[1], "at" );

    //삭제데이터의 공간 제외한 나머지 데이터를 파일에 쓰기
		fwrite( prev, offset, 1, fp ); 
		fwrite( buf, fLen - offset - delByte, 1, fp );
		fclose( fp );
	
}

int min( int a, int b ){ //두 정수 중 작은 값 반환 함수
	if ( a < b ) { return a; }
	else return b;
}
