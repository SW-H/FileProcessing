#include "person.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

char *filename;
int allRecordN;

// 과제 설명서대로 구현하는 방식은 각자 다를 수 있지만 약간의 제약을 둡니다.
// 레코드 파일이 페이지 단위로 저장 관리되기 때문에 사용자 프로그램에서 레코드
// 파일로부터 데이터를 읽고 쓸 때도 페이지 단위를 사용합니다. 따라서 아래의 두
// 함수가 필요합니다.
// 1. readPage(): 주어진 페이지 번호의 페이지 데이터를 프로그램 상으로 읽어와서
// pagebuf에 저장한다
// 2. writePage(): 프로그램 상의 pagebuf의 데이터를 주어진 페이지 번호에
// 저장한다 레코드 파일에서 기존의 레코드를 읽거나 새로운 레코드를 쓰거나 삭제
// 레코드를 수정할 때나 모든 I/O는 위의 두 함수를 먼저 호출해야 합니다. 즉
// 페이지 단위로 읽거나 써야 합니다.

//
// 페이지 번호에 해당하는 페이지를 주어진 페이지 버퍼에 읽어서 저장한다. 페이지
// 버퍼는 반드시 페이지 크기와 일치해야 한다.
//
void readPage(FILE *fp, char *pagebuf, int pagenum) {
    if (fp == NULL) {
        printf("page open error");
    } else {
        fseek(fp, pagenum * PAGE_SIZE, SEEK_SET);
        fread((void *) pagebuf, PAGE_SIZE, 1, fp);
    }
    return;
}

//
// 페이지 버퍼의 데이터를 주어진 페이지 번호에 해당하는 위치에 저장한다. 페이지
// 버퍼는 반드시 페이지 크기와 일치해야 한다.
//
void writePage(FILE *fp, const char *pagebuf, int pagenum) {

    if (fp == NULL) {
        printf("page open error");
    } else {
        fseek(fp, pagenum * PAGE_SIZE, SEEK_SET);
        fwrite(pagebuf, PAGE_SIZE, 1, fp);
    }
    return;
}

//
// 새로운 레코드를 저장할 때 터미널로부터 입력받은 정보를 Person 구조체에 먼저
// 저장하고, pack() 함수를 사용하여 레코드 파일에 저장할 레코드 형태를
// recordbuf에 만든다. 그런 후 이 레코드를 저장할 페이지를 readPage()를 통해
// 프로그램 상에 읽어 온 후 pagebuf에 recordbuf에 저장되어 있는 레코드를
// 저장한다. 그 다음 writePage() 호출하여 pagebuf를 해당 페이지 번호에 저장한다.
//
void pack(char *recordbuf, const Person *p) {
    strcpy(recordbuf, p->sn);
    strcat(recordbuf, "#");
    strcat(recordbuf, p->name);
    strcat(recordbuf, "#");
    strcat(recordbuf, p->age);
    strcat(recordbuf, "#");
    strcat(recordbuf, p->addr);
    strcat(recordbuf, "#");
    strcat(recordbuf, p->phone);
    strcat(recordbuf, "#");
    strcat(recordbuf, p->email);
    strcat(recordbuf, "#");
    return;
}

//
// 아래의 unpack() 함수는 recordbuf에 저장되어 있는 레코드를 구조체로 변환할 때
// 사용한다. 이 함수가 언제 호출되는지는 위에서 설명한 pack()의 시나리오를
// 참조하면 된다.
//
void unpack(const char *recordbuf, Person *p) {

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
    strcpy(p->sn, buffer[0]);
    strcpy(p->name, buffer[1]);
    strcpy(p->age, buffer[2]);
    strcpy(p->addr, buffer[3]);
    strcpy(p->phone, buffer[4]);
    strcpy(p->email, buffer[5]);

    free(tmp);

    return;
}

//
// 새로운 레코드를 저장하는 기능을 수행하며, 터미널로부터 입력받은 필드값을
// 구조체에 저장한 후 아래의 insert() 함수를 호출한다.
//
// pack() 함수를 사용하여
// 레코드 파일에 저장할 레코드 형태를 recordbuf에 만든다. 그런 후 이 레코드를
// 저장할 페이지를 readPage()를 통해 프로그램 상에 읽어 온 후 pagebuf에
// recordbuf에 저장되어 있는 레코드를 저장한다. 그 다음 writePage() 호출하여
// pagebuf를 해당 페이지 번호에 저장한다.
void insert(FILE *fp, const Person *p) {
    int insertPageNum = -1, insertRecordNum = -1;
    int entirePageN = 1, recordN = 0, delPage = -1, delRecord = -1;
    char pagebuf[PAGE_SIZE], headerBuffer[PAGE_SIZE], recordbuf[RECORD_SIZE], remainingData[PAGE_SIZE];
    memset(headerBuffer, (char) 0xFF, PAGE_SIZE);
    readPage(fp, headerBuffer, 0); //헤더읽어오기
    rewind(fp);
    entirePageN = headerBuffer[0];
    allRecordN = headerBuffer[4];
    delPage = headerBuffer[8];
    delRecord = headerBuffer[12];
    if (delPage == -1) { //삭제된 레코드가 없음
        memset(recordbuf, (char) 0xFF, RECORD_SIZE);
        pack(recordbuf, p);
        if (allRecordN == ((entirePageN - 1) * (PAGE_SIZE / RECORD_SIZE))) { //레코드 저장할 공간 없는 경우
            insertPageNum = entirePageN;
            insertRecordNum = 0;
            entirePageN++;
            memset(pagebuf, (char) 0xFF, PAGE_SIZE);
            strcpy(pagebuf, recordbuf);
            writePage(fp, pagebuf, insertPageNum);
            allRecordN++;
        } else if (((entirePageN - 1) * (PAGE_SIZE / RECORD_SIZE)) > allRecordN) { //레코드 저장할 공간 남음
            insertPageNum = entirePageN - 1; // append
            insertRecordNum = allRecordN % (PAGE_SIZE / RECORD_SIZE);
            memset(pagebuf, (char) 0xFF, PAGE_SIZE);
            readPage(fp, pagebuf, insertPageNum);
            strcpy(pagebuf + RECORD_SIZE * insertRecordNum, recordbuf);
            writePage(fp, pagebuf, insertPageNum);
            allRecordN++;

        } else {
            printf("error\n");
            return;
        }
    } else { //삭제된 레코드에 새로운 레코드 저장
        insertPageNum = delPage;
        insertRecordNum = delRecord;
        memset(recordbuf, (char) 0xFF, RECORD_SIZE);
        pack(recordbuf, p);
        memset(pagebuf, (char) 0xFF, PAGE_SIZE);
        readPage(fp, pagebuf, insertPageNum);

        delPage = pagebuf[insertRecordNum * RECORD_SIZE + 1];
        delRecord = pagebuf[insertRecordNum * RECORD_SIZE + 5];

        memcpy(pagebuf + insertRecordNum * RECORD_SIZE, recordbuf, RECORD_SIZE);
        writePage(fp, pagebuf, insertPageNum);

    }

    //헤더레코드 업데이트
    int update = allRecordN;
    memset(pagebuf, (char) 0xFF, PAGE_SIZE);
    memcpy(pagebuf, &entirePageN, sizeof(int));
    memcpy(&pagebuf[4], &update, sizeof(int));
    memcpy(&pagebuf[8], &delPage, sizeof(int));
    memcpy(&pagebuf[12], &delRecord, sizeof(int));
    rewind(fp);
    writePage(fp, pagebuf, 0);

    return;
}

//
// 주민번호와 일치하는 레코드를 찾아서 삭제하는 기능을 수행한다.
//
void delete(FILE *fp, const char *sn) {

    char pageBuffer[PAGE_SIZE], headerBuffer[PAGE_SIZE], recordBuffer[RECORD_SIZE];
    int entirePageN;//, recordN;
    int res_page, res_record, recent_delPage, recent_delRecord;
    memset(headerBuffer, (char) 0xFF, PAGE_SIZE);

    rewind(fp);
    readPage(fp, headerBuffer, 0); //헤더읽어오기

    entirePageN = headerBuffer[0];
    allRecordN = headerBuffer[4];
    recent_delPage = headerBuffer[8];
    recent_delRecord = headerBuffer[12];
    for (int i = 1; i < entirePageN; i++) {
        memset(pageBuffer, (char) 0xFF, PAGE_SIZE);
        readPage(fp, pageBuffer, i);
        for (int j = 0; j < (PAGE_SIZE / RECORD_SIZE); j++) {
            memset(recordBuffer, (char) 0xFF, RECORD_SIZE);
            memcpy(recordBuffer, pageBuffer + j * RECORD_SIZE, RECORD_SIZE);

            if (recordBuffer[0] == '*') {
                if (i == entirePageN - 1) {
                    printf("\n해당 주민번호가 존재하지 않습니다.\n"); //찾는 주민번호 없음
                    return;
                }
                continue;
            }
            Person temp;
            unpack(recordBuffer, &temp);

            if (strcmp(temp.sn, sn) == 0) { //삭제할 레코드 발견
                res_record = j;
                res_page = i;
                memcpy(&pageBuffer[res_record * RECORD_SIZE], "*", sizeof(char));
                memcpy(&pageBuffer[res_record * RECORD_SIZE + 1], &recent_delPage, sizeof(int));
                memcpy(&pageBuffer[res_record * RECORD_SIZE + 5], &recent_delRecord, sizeof(int));
                recent_delPage = res_page;
                recent_delRecord = res_record;

                writePage(fp, pageBuffer, res_page);

                //헤더페이지 업데이트
                int update = allRecordN;
                memset(pageBuffer, (char) 0xFF, PAGE_SIZE);
                memcpy(pageBuffer, &entirePageN, sizeof(int));
                memcpy(&pageBuffer[4], &update, sizeof(int));
                memcpy(&pageBuffer[8], &recent_delPage, sizeof(int));
                memcpy(&pageBuffer[12], &recent_delRecord, sizeof(int));
                writePage(fp, pageBuffer, 0);
                return;
            }
        }
    }
    printf("\n해당 주민번호가 존재하지 않습니다.\n"); //찾는 주민번호 없음
    return;
}

int main(int argc, char *argv[]) {
    char headerBuffer[PAGE_SIZE];
    int entirePageN = 1, delPage = -1, delRecord = -1;

    Person person;

    filename = (char *) calloc(sizeof(char), strlen(argv[2]));
    strcpy(filename, argv[2]);

    FILE *fp; // 레코드 파일의 파일 포인터
    if (strcmp(argv[1], "i") == 0) {
        if (0 != access(filename, F_OK)) { //처음 파일 쓸때
            fp = fopen(filename, "w+");
            if (fp == NULL) {
                printf("file open error\n");
                return 1;
            }
            int update = allRecordN;
            memset(headerBuffer, (char) 0xFF, PAGE_SIZE);
            memcpy(headerBuffer, &entirePageN, sizeof(int));
            memcpy(&headerBuffer[4], &update, sizeof(int));
            memcpy(&headerBuffer[8], &delPage, sizeof(int));
            memcpy(&headerBuffer[12], &delRecord, sizeof(int));
            rewind(fp);
            writePage(fp, headerBuffer, 0);
            fclose(fp);
        }
        //이미 데이터가 쓰여있을 때
        fp = fopen(filename, "r+");
        strcpy(person.sn, argv[3]);
        strcpy(person.name, argv[4]);
        strcpy(person.age, argv[5]);
        strcpy(person.addr, argv[6]);
        strcpy(person.phone, argv[7]);
        strcpy(person.email, argv[8]);
        insert(fp, &person);
        fclose(fp);
    } else if (strcmp(argv[1], "d") == 0) {
        if (0 != access(filename, F_OK)) {
            printf("Such sn does not exist\n");
            return 1;
        } else {
            fp = fopen(filename, "r+");
            delete(fp, argv[3]);
        }
    } else {
        printf("Wrong Input\n");
        return 1;
    }
    return 0;
}
