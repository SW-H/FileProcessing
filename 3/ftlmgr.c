#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "flash.h"

FILE *flashfp;	// fdevicedriver.c에서 사용

char option;
int blockN;
int ppn,pbn;
int* emptyPbn; // emptyPbn[n] == 1 이면 해당 block은 비어있지 않음
int emptyBlockN = 0; // 비어있는 block 번호

void isEmpty();
int dd_read(int ppn, char*pagebuf);
int dd_write(int ppn, char*pagebuf);
int dd_erase(int pbn);

int main(int argc, char *argv[])
{	
  char sectorbuf[SECTOR_SIZE]={0};
  char sparebuf[SPARE_SIZE]={0};
  char pagebuf[PAGE_SIZE]={0};
  char *blockbuf;


  option = argv[1][0];
  switch ( option ){
    case 'c':
      flashfp = fopen(argv[2],"w+");
      if(flashfp==NULL) printf("file open error\n");

      blockN = atoi(argv[3]);
      emptyPbn = (int*)calloc(sizeof(int),blockN);

      blockbuf=(char*)calloc(sizeof(char),BLOCK_SIZE*blockN);
      memset(blockbuf,(char)0xFF,BLOCK_SIZE*blockN);
      rewind(flashfp);
      fwrite((void*)blockbuf, BLOCK_SIZE* blockN,1, flashfp );
      break;

    case 'w':
      flashfp = fopen(argv[2],"r+");
      ppn = atoi ( argv[3] );
      strcpy(sectorbuf,argv[4]);
      strcpy(sparebuf, argv[5]);

      isEmpty();

      for(int i=strlen(sectorbuf);i<SECTOR_SIZE;i++){
        sectorbuf[i]=(char)0xFF;
      }
      for(int i=strlen(sparebuf);i<SPARE_SIZE;i++){
        sparebuf[i]=(char)0xFF;
      }
      emptyBlockN = -1;

      if(emptyPbn[ppn/4]==1){ //해당 블럭이비어있지 않은 경우 = in-place update가 필요한경우

        for( int i=0; i<blockN; i++ ){ //비어있는 block을 앞에서 부터 찾음 
          if(emptyPbn[i]==0){
            emptyBlockN = i;

            break;
          }
        }

        char temp[PAGE_NUM][PAGE_SIZE]={0};
        for( int i=0;i<PAGE_NUM;i++){
          dd_read((ppn/4)*4+i,temp[i]);
        }

        dd_erase(ppn/4);
        dd_erase(emptyBlockN);

        memcpy(pagebuf,sectorbuf,SECTOR_SIZE);
        memcpy(pagebuf+SECTOR_SIZE,sparebuf,SPARE_SIZE);

        memcpy(temp[ppn % 4],pagebuf,PAGE_SIZE);

        for( int i=0;i<PAGE_NUM;i++){
          dd_write((ppn/4)*4+i,temp[i]);
        }

      }

      else{ //해당 블럭이 비어있는 경우 
        memcpy(pagebuf,sectorbuf,SECTOR_SIZE);
        memcpy(pagebuf+SECTOR_SIZE,sparebuf,SPARE_SIZE);
        dd_write(ppn, pagebuf);
      }
      emptyPbn[ppn/4]=1;
      break;

    case 'r':
      flashfp = fopen(argv[2],"r");
      ppn = atoi(argv[3]);

      if(dd_read(ppn,pagebuf)==-1)
        printf("읽기 실패\n");

      if(pagebuf[0]!=(char)0xFF){
        for(int i=0;i<SECTOR_SIZE;i++){
          if(pagebuf[i]!=(char)0xFF)
            printf("%c",pagebuf[i]);
          else
            break;
        }
      }

      if(pagebuf[SECTOR_SIZE]!=(char)0xFF) printf(" ");

      if(pagebuf[SECTOR_SIZE]!=(char)0xFF){
        for(int i=0;i<SPARE_SIZE;i++){
          if(pagebuf[SECTOR_SIZE+i]!=(char)0xFF)
            printf("%c",pagebuf[SECTOR_SIZE+i]);
          else 
            break;
        }
      }


      break;

    case 'e':
      flashfp = fopen(argv[2],"r+");
      pbn = atoi(argv[3]);
      dd_erase(pbn);
      break;
  }

  fclose(flashfp);
  return 0;
}


void isEmpty(){
  fseek(flashfp,0,SEEK_END);
  blockN = ftell(flashfp) / BLOCK_SIZE;
  rewind(flashfp);
  char emptyPage[PAGE_SIZE];
  char checkPage[PAGE_SIZE];
  emptyPbn = (int*)calloc(sizeof(int),blockN);


  for(int i=0;i<blockN;i++){
func:   for(int j=0;j<PAGE_NUM;j++){
          memset(emptyPage,0xFF,PAGE_SIZE);
          memset(checkPage,0xFF,PAGE_SIZE);
          dd_read(i*PAGE_NUM + j, checkPage);

          if ( memcmp(checkPage,emptyPage,PAGE_SIZE*sizeof(char)) !=0){
            emptyPbn[i]=1;
            i++;
            goto func;
          }

        }
        emptyPbn[i]=0;
  }

}
