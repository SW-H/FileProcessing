 #include <stdio.h>
 #include <stdlib.h>
 #include <time.h> //시간측정 함수를 위해 include 

 int main(int argc, char **argv){
   // 표준입력으로 받은 레코드 파일에 저장되어 있는 전체 레코드를 "순차적"으로 읽어들이고,
   // 이때 걸리는 시간을 측정하는 코드 구현함
   

   FILE* fp = fopen ( argv[1], "rb" );
   if( fp == NULL ){ printf( "레코드 파일 열기 실패" );}
   clock_t start, end;//시간 측정을 위한 변수 
   int records; //전체 레코드 수
   long duration;//전체 소요시간 
   
   fseek ( fp, 0, SEEK_END );
   records = ftell ( fp ) / 100;
   
   char buffer[records][100];//레코드 파일 읽어드릴 공간 
   
   rewind(fp);

   start = clock();//시간측정 시작
   
   for( int i = 0 ; i < records ; i++ ){
      fread ( buffer[i] , 100, records , fp);
   }
   
   end = clock(); //시간측정  끝
  
   duration = (long)( end - start / CLOCKS_PER_SEC * 1000000 ); //1000000 us = 1 sec
   printf("#records: %d timecost: %ld us\n", records, duration );

   fclose(fp);
   return 0;
}
