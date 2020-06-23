#include <stdio.h>
#include <sys/types.h>
#include <time.h>
#include <stdlib.h>


#define SUFFLE_NUM	10000	 

void GenRecordSequence( int *list, int n );
void swap( int *a, int *b );

int main( int argc, char **argv ){
    // 'read_order_list'를 이용하여 표준 입력으로 받은 레코드 파일로부터 레코드를 random하게 읽어들이고,
    // 이때 걸리는 시간을 측정하는 코드 구현함
    
  int* read_order_list;
  int num_of_records;//전체 레코드 수
  long duration; //소요시간
  FILE* fp = fopen( argv[1], "rb" );
  clock_t start, end; //시간 측정을 위한 변수 
  
  if( fp == NULL ){ printf( "레코드 파일 열기 실패" ); }
  
  fseek ( fp, 0, SEEK_END );
  num_of_records = ftell( fp ) / 100;
  
  read_order_list = (int *)calloc ( sizeof(int), num_of_records );

  GenRecordSequence( read_order_list, num_of_records );
  char buffer[num_of_records][100];
  start = clock(); //시간측정 시작 

  
  for( int i = 0; i < num_of_records ; i++ ){
    fseek ( fp, ( read_order_list[i] - 1 ) * 100 , SEEK_SET );
    fread ( buffer[i], 100, 1 , fp );
  }

  end = clock(); // 시간측정 끝
  duration = (long)( end - start / CLOCKS_PER_SEC * 1000000 );
  
  printf("#records: %d timecost: %ld us\n", num_of_records, duration );
  fclose(fp);

	return 0;
}

void GenRecordSequence(int *list, int n){
	int i, j, k;

	srand((unsigned int)time(0));

	for(i=0; i<n; i++){
		list[i] = i;
	}
	
	for(i=0; i<SUFFLE_NUM; i++){
		j = rand() % n;
		k = rand() % n;
		swap(&list[j], &list[k]);
	}

	return;
}

void swap(int *a, int *b){
	int tmp;

	tmp = *a;
	*a = *b;
	*b = tmp;

	return;
}
