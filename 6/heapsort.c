#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "person.h"

void unpack_sn(const char *recordbuf, unsigned long *res);//char *resbuf) {
void readPage(FILE *fp, char *pagebuf, int pagenum);
void writePage(FILE *fp, const char *pagebuf, int pagenum);
void buildHeap(FILE *inputfp, char **heaparray);
void makeSortedFile(FILE *outputfp, char **heaparray);
void heapify(char** heaparray);
int entire_page, entire_records, records_page;


// 과제 설명서대로 구현하는 방식은 각자 다를 수 있지만 약간의 제약을 둡니다.
// 레코드 파일이 페이지 단위로 저장 관리되기 때문에 사용자 프로그램에서 레코드 파일로부터 데이터를 읽고 쓸 때도
// 페이지 단위를 사용합니다. 따라서 아래의 두 함수가 필요합니다.
// 1. readPage(): 주어진 페이지 번호의 페이지 데이터를 프로그램 상으로 읽어와서 pagebuf에 저장한다
// 2. writePage(): 프로그램 상의 pagebuf의 데이터를 주어진 페이지 번호에 저장한다
// 레코드 파일에서 기존의 레코드를 읽거나 새로운 레코드를 쓸 때나
// 모든 I/O는 위의 두 함수를 먼저 호출해야 합니다. 즉, 페이지 단위로 읽거나 써야 합니다.


int main(int argc, char *argv[]) {
    FILE *inputfp;    // 입력 레코드 파일의 파일 포인터
    FILE *outputfp;    // 정렬된 레코드 파일의 파일 포인터

    if (!strcmp(argv[1], "s")) {
        inputfp = fopen(argv[2], "r+");
        if (inputfp == NULL) {
            printf("input file open error");
            return 0;
        }
        outputfp = fopen(argv[3], "w+");
        if (outputfp == NULL) {
            printf("output file open error");
            return 0;
        }
        char header[PAGE_SIZE] = {0};
        readPage(inputfp, header, 0); // 헤더 읽어오기
        entire_page = header[0];
        entire_records = header[4];
        records_page = entire_page - 1; //헤더를 제외한 페이지 개수
        writePage(outputfp, header, 0); //결과파일에서도 헤더페이지는 똑같이 복사

        char **heaparray = (char **) calloc(sizeof(char *), entire_records + 1);
        for (int i = 0; i < entire_records + 1; i++) {
            heaparray[i] = (char *) calloc(sizeof(char), RECORD_SIZE);
        }

        buildHeap(inputfp, heaparray);
        makeSortedFile(outputfp, heaparray);
        fclose(inputfp);
        fclose(outputfp);
    } else {
        printf("Wrong option");
    }
    return 0;
}
//
// 페이지 번호에 해당하는 페이지를 주어진 페이지 버퍼에 읽어서 저장한다. 페이지 버퍼는 반드시 페이지 크기와 일치해야 한다.
//
void readPage(FILE *fp, char *pagebuf, int pagenum) {
    if (fp == NULL) {
        printf("page open error");
    } else {
        fseek(fp, pagenum * PAGE_SIZE, SEEK_SET);
        fread((void *) pagebuf, PAGE_SIZE, 1, fp);
    }
}
//
// 페이지 버퍼의 데이터를 주어진 페이지 번호에 해당하는 위치에 저장한다. 페이지 버퍼는 반드시 페이지 크기와 일치해야 한다.
//
void writePage(FILE *fp, const char *pagebuf, int pagenum) {
    if (fp == NULL) {
        printf("page open error");
    } else {
        fseek(fp, pagenum * PAGE_SIZE, SEEK_SET);
        fwrite(pagebuf, PAGE_SIZE, 1, fp);
    }
}
//
// 주어진 레코드 파일에서 레코드를 읽어 heap을 만들어 나간다. Heap은 배열을 이용하여 저장되며, 
// heap의 생성은 Chap9에서 제시한 알고리즘을 따른다. 레코드를 읽을 때 페이지 단위를 사용한다는 것에 주의해야 한다.
//
void buildHeap(FILE *inputfp, char **heaparray) {
    char recordBuf[RECORD_SIZE];
    char pageBuf[PAGE_SIZE];
    unsigned long parent_sn, Child_sn;
    char recordTmp[RECORD_SIZE];
    int recordCnt = 1; //현재까지 레코드 개수
    for (int i = 1; i <= records_page; i++) { //페이지 다 읽어옴 (0:헤더파일 , 1~)
        //i=page num
        readPage(inputfp, pageBuf, i);
        for (int j = 1; j <= PAGE_SIZE / RECORD_SIZE; j++) {//각 페이지 당 레코드 개수만큼 읽어옴
            if (recordCnt <= entire_records) {
                memset(recordBuf, (char) 0xFF, RECORD_SIZE);
                memcpy(recordBuf, &pageBuf[(j - 1) * RECORD_SIZE], RECORD_SIZE);
                memcpy(heaparray[(recordCnt - 1)], recordBuf, RECORD_SIZE);
                recordCnt++;
                int now = recordCnt - 2; //여기에 추가
                unpack_sn(heaparray[now], &Child_sn);
                unpack_sn(heaparray[now / 2], &parent_sn);
                if (Child_sn < parent_sn) {

                    memcpy(recordTmp, heaparray[now], RECORD_SIZE);
                    memcpy(heaparray[now], heaparray[now / 2], RECORD_SIZE);
                    memcpy(heaparray[now / 2], recordTmp, RECORD_SIZE);
                    unpack_sn(heaparray[now], &Child_sn);
                    unpack_sn(heaparray[now / 2], &parent_sn);
                }
            }
        }
    }
}
void heapify(char** heaparray){
    unsigned long parent_sn, Child_sn;
    char recordTmp[RECORD_SIZE];
    int recordCnt = 1; //현재까지 레코드 개수
    records_page=entire_records/(PAGE_SIZE/RECORD_SIZE)+1;
    for (int i = 1; i <= records_page; i++) { //페이지 다 읽어옴 (0:헤더파일 , 1~)
        //i=page num
        for (int j = 1; j <= PAGE_SIZE / RECORD_SIZE; j++) {//각 페이지 당 레코드 개수만큼 읽어옴
            if (recordCnt <= entire_records) {
                recordCnt++;
                int now = recordCnt - 2; //여기에 추가
                unpack_sn(heaparray[now], &Child_sn);
                unpack_sn(heaparray[now / 2], &parent_sn);
                if (Child_sn < parent_sn) {

                    memcpy(recordTmp, heaparray[now], RECORD_SIZE);
                    memcpy(heaparray[now], heaparray[now / 2], RECORD_SIZE);
                    memcpy(heaparray[now / 2], recordTmp, RECORD_SIZE);
                    unpack_sn(heaparray[now], &Child_sn);
                    unpack_sn(heaparray[now / 2], &parent_sn);
                }
            }
        }
    }
}
//
// 완성한 heap을 이용하여 주민번호를 기준으로 오름차순으로 레코드를 정렬하여 새로운 레코드 파일에 저장한다.
// Heap을 이용한 정렬은 Chap9에서 제시한 알고리즘을 이용한다.
// 레코드를 순서대로 저장할 때도 페이지 단위를 사용한다.
//
void makeSortedFile(FILE *outputfp, char **heaparray) {
    char pageBuf[PAGE_SIZE];
    int recordCnt = 0; //현재까지 레코드 개수
    int org_entire_records=entire_records;
    int org_records_page=records_page;
    entire_records--;
    for(int i=1;i<=org_entire_records;i++){
        heapify(&heaparray[i]);
        entire_records--;
    }
    entire_records=org_entire_records;
    records_page=org_records_page;
    for (int i = 1; i <= records_page; i++) {
        memset(pageBuf, (char) 0xFF, PAGE_SIZE);
        for (int j = 1; j <= (int) (PAGE_SIZE / RECORD_SIZE); j++) {
            if (recordCnt < entire_records) {
                memcpy(&pageBuf[(j - 1) * RECORD_SIZE], heaparray[recordCnt], RECORD_SIZE);
                recordCnt++;
            } else
                break;
        }
        writePage(outputfp, pageBuf, i);
    }
}
void unpack_sn(const char *recordbuf, unsigned long *res) {
    char buffer[6][30] = {0};
    char *ptr;
    char *tmp = (char *) calloc(sizeof(char), strlen(recordbuf));
    strcpy(tmp, recordbuf);

    ptr = strtok(tmp, "#");
    int cnt = 0;
    while (ptr != NULL) {
        strcpy(buffer[cnt++], ptr);
        ptr = strtok(NULL, "#");
    }
    *res = strtoul(buffer[0], NULL, 10);
    free(tmp);
    return;
}

