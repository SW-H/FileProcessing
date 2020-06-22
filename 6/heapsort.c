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


// ���� ������� �����ϴ� ����� ���� �ٸ� �� ������ �ణ�� ������ �Ӵϴ�.
// ���ڵ� ������ ������ ������ ���� �����Ǳ� ������ ����� ���α׷����� ���ڵ� ���Ϸκ��� �����͸� �а� �� ����
// ������ ������ ����մϴ�. ���� �Ʒ��� �� �Լ��� �ʿ��մϴ�.
// 1. readPage(): �־��� ������ ��ȣ�� ������ �����͸� ���α׷� ������ �о�ͼ� pagebuf�� �����Ѵ�
// 2. writePage(): ���α׷� ���� pagebuf�� �����͸� �־��� ������ ��ȣ�� �����Ѵ�
// ���ڵ� ���Ͽ��� ������ ���ڵ带 �аų� ���ο� ���ڵ带 �� ����
// ��� I/O�� ���� �� �Լ��� ���� ȣ���ؾ� �մϴ�. ��, ������ ������ �аų� ��� �մϴ�.


int main(int argc, char *argv[]) {
    FILE *inputfp;    // �Է� ���ڵ� ������ ���� ������
    FILE *outputfp;    // ���ĵ� ���ڵ� ������ ���� ������

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
        readPage(inputfp, header, 0); // ��� �о����
        entire_page = header[0];
        entire_records = header[4];
        records_page = entire_page - 1; //����� ������ ������ ����
        writePage(outputfp, header, 0); //������Ͽ����� ����������� �Ȱ��� ����

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
// ������ ��ȣ�� �ش��ϴ� �������� �־��� ������ ���ۿ� �о �����Ѵ�. ������ ���۴� �ݵ�� ������ ũ��� ��ġ�ؾ� �Ѵ�.
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
// ������ ������ �����͸� �־��� ������ ��ȣ�� �ش��ϴ� ��ġ�� �����Ѵ�. ������ ���۴� �ݵ�� ������ ũ��� ��ġ�ؾ� �Ѵ�.
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
// �־��� ���ڵ� ���Ͽ��� ���ڵ带 �о� heap�� ����� ������. Heap�� �迭�� �̿��Ͽ� ����Ǹ�, 
// heap�� ������ Chap9���� ������ �˰����� ������. ���ڵ带 ���� �� ������ ������ ����Ѵٴ� �Ϳ� �����ؾ� �Ѵ�.
//
void buildHeap(FILE *inputfp, char **heaparray) {
    char recordBuf[RECORD_SIZE];
    char pageBuf[PAGE_SIZE];
    unsigned long parent_sn, Child_sn;
    char recordTmp[RECORD_SIZE];
    int recordCnt = 1; //������� ���ڵ� ����
    for (int i = 1; i <= records_page; i++) { //������ �� �о�� (0:������� , 1~)
        //i=page num
        readPage(inputfp, pageBuf, i);
        for (int j = 1; j <= PAGE_SIZE / RECORD_SIZE; j++) {//�� ������ �� ���ڵ� ������ŭ �о��
            if (recordCnt <= entire_records) {
                memset(recordBuf, (char) 0xFF, RECORD_SIZE);
                memcpy(recordBuf, &pageBuf[(j - 1) * RECORD_SIZE], RECORD_SIZE);
                memcpy(heaparray[(recordCnt - 1)], recordBuf, RECORD_SIZE);
                recordCnt++;
                int now = recordCnt - 2; //���⿡ �߰�
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
    int recordCnt = 1; //������� ���ڵ� ����
    records_page=entire_records/(PAGE_SIZE/RECORD_SIZE)+1;
    for (int i = 1; i <= records_page; i++) { //������ �� �о�� (0:������� , 1~)
        //i=page num
        for (int j = 1; j <= PAGE_SIZE / RECORD_SIZE; j++) {//�� ������ �� ���ڵ� ������ŭ �о��
            if (recordCnt <= entire_records) {
                recordCnt++;
                int now = recordCnt - 2; //���⿡ �߰�
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
// �ϼ��� heap�� �̿��Ͽ� �ֹι�ȣ�� �������� ������������ ���ڵ带 �����Ͽ� ���ο� ���ڵ� ���Ͽ� �����Ѵ�.
// Heap�� �̿��� ������ Chap9���� ������ �˰����� �̿��Ѵ�.
// ���ڵ带 ������� ������ ���� ������ ������ ����Ѵ�.
//
void makeSortedFile(FILE *outputfp, char **heaparray) {
    char pageBuf[PAGE_SIZE];
    int recordCnt = 0; //������� ���ڵ� ����
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

