#include <stdio.h>
#include <stdlib.h>
int main( int argc, char** argv ){
   FILE* ofp; //파일1
   FILE* ifp;
   long flen;//파일 길이
   char *temp;

   ofp = fopen ( argv[1] , "r+" );  
   ifp = fopen ( argv[2] , "rb" );
    
   if ( ofp == NULL ) { printf("file1 open error"); }
   if ( ifp == NULL) { printf("file2 open error"); }

   fseek ( ofp , 0 , SEEK_END ); 
   rewind ( ifp );
   fseek ( ifp , 0 , SEEK_END );
   flen = ftell ( ifp ); // 파일2의 길이
 
  
   temp = ( char* )calloc( flen, sizeof( char ) ); 
  
   rewind ( ifp );
   fread ( temp , flen , 1, ifp );//파일2의 내용 저장

   fseek ( ofp, -1, SEEK_END ); 
   fwrite ( temp, flen, 1, ofp ); //파일2 내용 병합 
  
  //파일 닫기
    fclose ( ofp );
    fclose ( ifp );

}
