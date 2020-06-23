#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main( int argc, char** argv ){
    FILE* fp = fopen( argv[1], "rb+" );
	  long dataLen, fLen, offset;
    char* rem; // offset 뒷 부분의 데이터들 저장
    
	  offset = strtol ( argv[2], NULL, 10 );//오프셋
    fseek( fp, 0, SEEK_END );
	  fLen = ftell(fp); //파일의 길이
	  dataLen = strlen( argv[3] );// 끼워넣을 데이터의 길이
	
  	rem = (char*)calloc( fLen-offset , sizeof(char));

	  fseek (fp, offset, SEEK_SET);
	  fread( rem, fLen-offset, 1, fp ); //파일의 뒷부분 데이터들 저장
	
	  fseek (fp, offset, SEEK_SET);
	  fwrite( argv[3], strlen(argv[3]), 1, fp ); //끼워넣을 데이터 파일에 씀
    
	  fseek(fp, offset + strlen( argv[3] ), SEEK_SET);
	  fwrite( rem, strlen(rem), 1, fp ); //추가된 데이터 뒷부분 병합
  	fclose(fp);

}

