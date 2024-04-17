#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include <Windows.h>
#include <string.h>
#include <time.h>
#include <conio.h>
#define STRING_SIZE 256
#define MAX_MENU_ITEMS 100

//메뉴 구조체
struct Menu
{
    char category[STRING_SIZE];
    char menu_name[STRING_SIZE];
    int menu_price;
};

// 장바구니 구조체
struct Cart {
    int items[STRING_SIZE]; // 선택한 메뉴를 저장할 장바구니 배열
    int count;                 // 장바구니에 담긴 메뉴의 수
    int itemsCount[STRING_SIZE];
};

// 사용자 모드 함수 원형 선언
void displayFunction(struct Cart* cart, struct Menu* menulist, int index, int year, int month, int day);
void selectCategory(int categoryChoice, struct Cart* cart, struct Menu* menulist, int index, int year, int month, int day);
void displayMenu(int categoryChoice, struct Cart* cart, struct Menu* menulist, int index, int year, int month, int day, char selectedCategory[]);
void addToCart(int menuChoice, int menuCount, struct Cart* cart, struct Menu* menulist);
void viewCart(struct Cart* cart, struct Menu* menulist);
void checkout(struct Cart* cart, struct Menu* menulist, int index, int year, int month, int day);
void clearCart(struct Cart* cart);


// 로그 파일에 정보를 추가하는 함수
void addToLog(int year, int month, int day, const char* name, const char* menu_name, int menu_price, int menuCount) {
    char date[STRING_SIZE];
    char count[STRING_SIZE];
    sprintf(date, "%d-%02d-%02d", year, month, day);
    sprintf(count, "%d원 - %d개", menu_price, menuCount);

    FILE* logFile = fopen("log.txt", "a");
    if (logFile == NULL) {
        logFile = fopen("log.txt", "w");
        fclose(logFile);
    }
    if (logFile != NULL) {
        fprintf(logFile, "%s %s %s %s\n", date, name, menu_name, count);
        fclose(logFile);
    }
}


void trim(char* str, char* cpy, int cpy_len)
{
    int str_len, start_point = 0;
    int i, j;

    if (!str || !cpy)
    {
        return;
    }

    str_len = strlen(str) - 1;
    while (*(str + str_len) == ' ' || *(str + str_len) == '\t')
    {
        str_len--;
    }
    while (*(str + start_point) == ' ' || *(str + start_point) == '\t')
    {
        start_point++;
    }

    for (i = start_point, j = 0; i <= str_len && j < cpy_len; i++, j++) {
        cpy[j] = str[i];
    }
    cpy[j] = '\0';
}

void date_input(int* year, int* month, int* day, int logyear, int logmonth, int logday)
{
    int y, m, d;
    int err = 0, notint, ann;
    int days[13] = { 0, 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 };
    while (1)
    {
        err = 0;
        printf("오늘 날짜를 입력해주세요.\n->");
        notint = scanf(" %d", &y);
        if (notint != 1)
        {
            while (getchar() != '\n');
            system("cls");
            err = 1;
        }
        else if (y <= 0)
        {
            while (getchar() != '\n');
            system("cls");
            err = 1;
        }
        else
        {
            ann = getchar();
            if (ann == ' ' || ann == '\t' || ann == '\n')
            {
                notint = scanf(" %d", &m);
                if (notint != 1)
                {
                    while (getchar() != '\n');
                    system("cls");
                    err = 1;
                }
                else if (m <= 0 || m > 12)
                {
                    while (getchar() != '\n');
                    system("cls");
                    err = 1;
                }
                else
                {
                    if (y % 4 == 0 && y % 100 != 0 && m == 2)
                    {
                        days[2] = 29;
                    }
                    if (y % 400 == 0)
                    {
                        days[2] = 29;
                    }
                    notint = scanf(" %d", &d);
                    if (notint != 1)
                    {
                        while (getchar() != '\n');
                        system("cls");
                        err = 1;
                    }
                    else if (d <= 0 || d > days[m])
                    {
                        while (getchar() != '\n');
                        system("cls");
                        err = 1;
                    }

                    else
                    {
                        if (logmonth == -1 && logday == -1) { // 첫 기록일 때
                            *year = y;
                            *month = m;
                            *day = d;
                            return;
                        }
                        else if (y > logyear) { // 최신연도일 때
                            *year = y;
                            *month = m;
                            *day = d;
                            return;
                        }
                        else if (y < logyear) { // 과거연도일 때
                            err = 2;
                        }
                        else { // 같은 연도일 때
                            if (m > logmonth) {
                                *year = y;
                                *month = m;
                                *day = d;
                                return;
                            }
                            else if (m < logmonth) {
                                err = 2;
                            }
                            else { // 같은 월일 때
                                if (d >= logday) {
                                    *year = y;
                                    *month = m;
                                    *day = d;
                                    return;
                                }
                                else {
                                    err = 2;
                                }
                            }
                        }
                    }
                }
            }
        }
        if (err == 1)
        {
            printf("공백을 구분자로 정확한 날짜를 입력해주세요.\n예) 2023 12 25\n\n");
        }
        else if (err == 2) {
            printf("로그 기록보다 과거의 날짜는 입력할 수 없습니다.\n");
            printf("최신 기록 : %d년 %d월 %d일\n\n", logyear, logmonth, logday);
        }
    }
}


int start_menu()
{
    int mode;
    int notint;
    while (1)
    {
        printf("원하는 모드를 선택해주세요.\n\n");
        printf("1. 사용자 모드\n");
        printf("2. 관리자 모드\n");
        printf("3. 종료\n\n");
        printf("메뉴 선택 : ");
        notint = scanf(" %d", &mode);

        int err = 0;
        if (notint != 1)
        {
            while (getchar() != '\n');
            system("cls");
            err = 1;
        }
        else if (mode < 1 || mode>3)
        {
            while (getchar() != '\n');
            system("cls");
            err = 1;
        }
        else
        {
            while (getchar() != '\n');
            return mode;
        }
        if (err == 1)
        {
            printf("해당 메뉴는 존재하지 않습니다. 밑의 메뉴 중 선택해주세요.\n");
            printf("\n1:사용자 모드, 2:관리자 모드, 3:종료\n\n");
        }
    }

}

// 카테고리 목록을 출력하는 함수
void displayFunction(struct Cart* cart, struct Menu* menulist, int index, int year, int month, int day) {
    int functionChoice;

    printf("\n");
    printf("원하는 메뉴를 선택해주세요.\n");
    printf("\n");
    printf("1. 카테고리 선택\n");
    printf("2. 결제\n");
    printf("3. 뒤로가기\n");
    printf("\n");
    printf("메뉴 선택 : ");
    scanf("%d", &functionChoice);
    switch (functionChoice) {
    case 1:
        system("cls");
        selectCategory(functionChoice, cart, menulist, index, year, month, day);
        break;
    case 2:
        system("cls");
        checkout(cart, menulist, index, year, month, day);
        break;
    case 3:
        system("cls");
        printf("메인 메뉴로 돌아갑니다.\n");
        //start_menu();
        break;
    default:
        system("cls");
        printf("해당 메뉴는 존재하지 않습니다.\n");
        break;
    }

}


// 카테고리 선택하는 함수
void selectCategory(int functionChoice, struct Cart* cart, struct Menu* menulist, int index, int year, int month, int day) {
    int categoryChoice;
    printf("\n");
    printf("원하는 카테고리를 선택해주세요.\n");
    printf("\n");

    int menuItem = 1;
    char selectedCategory[STRING_SIZE]; // 선택한 카테고리 저장 변수

    for (int i = 1; i < index; i++) {
        // 중복 카테고리 표시를 방지
        int duplicate = 0;
        for (int j = 1; j < i; j++) {
            if (strcmp(menulist[i].category, menulist[j].category) == 0) {
                duplicate = 1;
                break;
            }
        }
        if (!duplicate) {
            printf("%d. %s\n", menuItem, menulist[i].category);
            menuItem++;
        }
    }

    printf("0. 뒤로가기\n");
    printf("\n");
    printf("메뉴 선택 : ");
    scanf("%d", &categoryChoice);

    if (categoryChoice == 0) {
        // 뒤로 가기
        system("cls");
        displayFunction(cart, menulist, index, year, month, day);
        return;
    }
    else if (categoryChoice >= 1 && categoryChoice < menuItem) {
        system("cls");
        // 선택한 카테고리 저장
        int categoryIndex = 0;
        for (int i = 1; i < index; i++) {
            int duplicate = 0;
            for (int j = 1; j < i; j++) {
                if (strcmp(menulist[i].category, menulist[j].category) == 0) {
                    duplicate = 1;
                    break;
                }
            }
            if (!duplicate) {
                if (categoryIndex == categoryChoice - 1) {
                    strcpy(selectedCategory, menulist[i].category);
                    break;
                }
                categoryIndex++;
            }
        }
        displayMenu(categoryChoice, cart, menulist, index, year, month, day, selectedCategory); // 메뉴 표시 함수로 이동
    }
    else {
        system("cls");
        printf("올바른 입력이 아닙니다. 다시 선택해주세요.\n");
        while (getchar() != '\n');
        selectCategory(functionChoice, cart, menulist, index, year, month, day);
    }
}

// displayMenu 함수에서 카테고리별 메뉴를 표시하는 코드 수정
void displayMenu(int categoryChoice, struct Cart* cart, struct Menu* menulist, int index, int year, int month, int day, char selectedCategory[]) {
    int menuChoice;
    printf("\n");
    printf("원하는 메뉴를 선택해주세요.\n");
    printf("\n");
    int chaind[MAX_MENU_ITEMS] = { 0 };
    int menuItem = 1;
    char cou[STRING_SIZE] = { 0, };
    char trim_cou[STRING_SIZE];
    int re;
    int c;

    for (int i = 0; i < index; i++) {
        // 선택한 카테고리에 해당하는 메뉴만 표시
        if (strcmp(menulist[i].category, selectedCategory) == 0) {
            printf("%d. %s %d원\n", menuItem, menulist[i].menu_name, menulist[i].menu_price);
            chaind[menuItem - 1] = i;
            menuItem++;
        }
    }

    printf("0. 뒤로가기\n\n");
    printf("메뉴 선택 : ");
    scanf("%d", &menuChoice);

    if (menuChoice == 0) {
        // 뒤로 가기
        system("cls");
        selectCategory(categoryChoice, cart, menulist, index, year, month, day);
        return;
    }
    else if (menuChoice >= 1 && menuChoice < menuItem) {
        while (getchar() != '\n');
        while (1) {
            re = 0;
            printf("갯수 선택 [1] : ");
            fgets(cou, STRING_SIZE, stdin);
            cou[strcspn(cou, "\n")] = '\0';
            //gets_s(cou, STRING_SIZE);
            system("cls");
            if (cou[0] == 0) {
                menuChoice = chaind[menuChoice - 1];
                addToCart(menuChoice, 1, cart, menulist);
                return displayFunction(cart, menulist, index, year, month, day);
            }
            trim(cou, trim_cou, STRING_SIZE);
            for (int i = 0; i < strlen(trim_cou); i++) {
                if (!(trim_cou[i] >= 48 && trim_cou[i] <= 57)) {
                    re = 1;
                    break;
                }
            }
            if (re == 0) {
                c = atoi(trim_cou);
                if (c == 0) {
                    return displayMenu(categoryChoice, cart, menulist, index, year, month, day, selectedCategory);
                }
                else if (c > 0) {
                    menuChoice = chaind[menuChoice - 1];
                    addToCart(menuChoice, c, cart, menulist);
                    //checkout(cart, menulist, index, year, month, day);
                    return displayFunction(cart, menulist, index, year, month, day);
                }
                else {
                    while (getchar() != '\n');
                    re = 1;
                }
            }
            else if (re == 1) {
                menuItem = 1;
                printf("\n");
                printf("잘못 입력하였습니다. 0이상의 정수 또는 엔터를 입력해주세요.\n");
                printf("\n");
                for (int i = 0; i < index; i++) {
                    // 선택한 카테고리에 해당하는 메뉴만 표시
                    if (strcmp(menulist[i].category, selectedCategory) == 0) {
                        printf("%d. %s %d원\n", menuItem, menulist[i].menu_name, menulist[i].menu_price);
                        chaind[menuItem - 1] = i;
                        menuItem++;
                    }
                }
                printf("0. 뒤로가기\n\n");
                printf("메뉴 선택 : %d\n", menuChoice);
            }
        }
    }
    else {
        system("cls");
        printf("올바른 입력이 아닙니다. 다시 선택해주세요.\n");
        while (getchar() != '\n');
        displayMenu(categoryChoice, cart, menulist, index, year, month, day, selectedCategory);
    }
}


// 메뉴를 장바구니에 추가하는 함수
void addToCart(int menuChoice, int menuCount, struct Cart* cart, struct Menu* menulist) {
    // 선택한 메뉴를 장바구니에 추가
    printf("%s %d개가 장바구니에 추가되었습니다.\n", menulist[menuChoice].menu_name, menuCount);
    Sleep(2000);
    cart->items[cart->count] = menuChoice;
    cart->itemsCount[cart->count] = menuCount;
    cart->count++;
}

// 장바구니 내역을 출력하고 결제하는 함수
void checkout(struct Cart* cart, struct Menu* menulist, int index, int year, int month, int day) {
    char paymentChoice[STRING_SIZE]; // "Y" or "N"
    int totalPrice = 0;
    system("cls");
    if (cart->count < 1) {
        printf("장바구니가 비어있습니다. 메뉴를 선택하세요.\n");
        Sleep(1000);
        system("cls");
        return;
    }
    printf("장바구니\n");

    for (int i = 0; i < cart->count; i++) {
        int menuChoice = cart->items[i];
        printf("%d) %s %d원 x %d = %d원\n", i + 1, menulist[menuChoice].menu_name, menulist[menuChoice].menu_price, cart->itemsCount[i], menulist[menuChoice].menu_price * cart->itemsCount[i]);
        totalPrice += menulist[menuChoice].menu_price * cart->itemsCount[i];
    }

    printf("총 가격: %d원\n", totalPrice);

    // 결제 여부 확인
    printf("결제(Yes), 취소(No), 갯수증감(번호±) : ");
    scanf("%s", paymentChoice);

    if (strcmp(paymentChoice, "Y") == 0 || strcmp(paymentChoice, "y") == 0 || strcmp(paymentChoice, "Yes") == 0 || strcmp(paymentChoice, "yes") == 0) {
        // 결제 완료
        printf("카드를 삽입해주십시오.\n");
        // 사용자 이름 입력
        char username[STRING_SIZE];
        printf("사용자 이름을 입력해주세요: ");
        scanf("%s", username);

        // 결제 내역 로그에 추가
        for (int i = 0; i < cart->count; i++) {
            int menuChoice = cart->items[i];
            addToLog(year, month, day, username, menulist[menuChoice].menu_name, menulist[menuChoice].menu_price, cart->itemsCount[i]);
        }
        system("cls");
        printf("결제완료!\n");
        clearCart(cart);
    }
    else if (strcmp(paymentChoice, "N") == 0 || strcmp(paymentChoice, "n") == 0 || strcmp(paymentChoice, "No") == 0 || strcmp(paymentChoice, "no") == 0) {
        // 결제 취소
        system("cls");
        printf("결제를 취소하였습니다.\n");
        displayFunction(cart, menulist, index, year, month, day);
    }

    else {
        int menuIndex;
        char sign;
        if (sscanf(paymentChoice, "%d%c", &menuIndex, &sign) == 2) {
            menuIndex--;  // 메뉴 인덱스는 0부터 시작하므로 1 감소
            if (menuIndex >= 0 && menuIndex < cart->count) {
                if (sign == '+') {
                    cart->itemsCount[menuIndex]++;
                }
                else if (sign == '-') {
                    cart->itemsCount[menuIndex]--;
                    if (cart->itemsCount[menuIndex] < 0) {
                        cart->itemsCount[menuIndex] = 0;  // 음수 방지
                        printf("갯수는 0 이상이어야 합니다. 다시 선택해주세요.\n");
                        Sleep(1500);
                        system("cls");
                        checkout(cart, menulist, index, year, month, day);
                    }
                }
                system("cls");
                printf("갯수가 조정되었습니다.\n");
                checkout(cart, menulist, index, year, month, day);
                return;
            }
        }
        system("cls");
        printf("올바른 입력이 아닙니다. 다시 선택해주세요.\n");
        Sleep(1500);
        checkout(cart, menulist, index, year, month, day);
    }
}

// 장바구니를 초기화하는 함수
void clearCart(struct Cart* cart) {
    // 장바구니를 초기화
    cart->count = 0;
}

int man_menu()
{
    int mode;
    int notint;
    int err = 0;
    while (1)
    {
        printf("원하는 메뉴를 선택해주세요.\n\n");
        printf("1. 메뉴 추가\n");
        printf("2. 메뉴 변경\n");
        printf("3. 메뉴 삭제\n");
        printf("4. 뒤로 가기\n\n");
        printf("메뉴 선택 : ");
        notint = scanf(" %d", &mode);
        if (notint != 1)
        {
            while (getchar() != '\n');
            system("cls");
            err = 1;
        }
        else if (mode < 1 || mode>4)
        {
            while (getchar() != '\n');
            system("cls");
            err = 1;
        }
        else
        {
            while (getchar() != '\n');
            return mode;
        }
        if (err == 1)
        {
            printf("해당 메뉴는 존재하지 않습니다. 밑의 메뉴 중 선택해주세요.\n");
            printf("\n1:메뉴 추가, 2:메뉴 변경, 3:메뉴 삭제, 4:뒤로 가기\n\n");
        }
    }
}

int menu_input(struct Menu* m, int* index, char* chate, char* menuname, int* menuprice, int mode)
{
    char cha[STRING_SIZE];
    char men[STRING_SIZE];
    char trim_cha[STRING_SIZE];
    char trim_men[STRING_SIZE];
    int price = 0;
    int ann;
    int k = 0;
    int o = 0;
    while (1)
    {
        k = 0;
        o = 0;
        if (mode == 0)
        {
            printf("추가하려는 메뉴를 입력해주세요.\n");
            printf("관리자 모드 화면으로 돌아가길 원하시면 c를 입력해주세요.\n\n");
            printf("입력 : ");
        }
        else if (mode == 1)
        {
            printf("변경하려는 메뉴를 입력해주세요.\n");
            printf("관리자 모드 화면으로 돌아가길 원하시면 c를 입력해주세요.\n\n");
            printf("입력 : ");
        }
        else if (mode == -1)
        {
            printf("변경할 내용을 입력해주세요 : ");
        }
        else if (mode == 2)
        {
            printf("삭제하려는 메뉴를 입력해주세요.\n");
            printf("관리자 모드 화면으로 돌아가길 원하시면 c를 입력해주세요.\n\n");
            printf("입력 : ");
        }
        scanf("%s", cha);
        trim(cha, trim_cha, STRING_SIZE);
        if (!strcmp("c", trim_cha) && mode != -1)
        {
            while ((ann = getchar()) != '\n')
            {
                if (ann != ' ' && ann != '\t')
                {
                    k = 1;
                    break;
                }
            }
            if (k == 1)
            {
                system("cls");
                printf("메뉴 입력이 잘못되었습니다. 정확히 입력해주세요.\n");
                printf("예) Pizza ShrimpPizza 10000\n\n");
                while ((ann = getchar()) != '\n');
                continue;
            }
            else if (k == 0)
            {
                return 1;
            }
        }
        if (strlen(trim_cha) < 2 || strlen(trim_cha) > 10)
        {
            system("cls");
            printf("메뉴 입력이 잘못되었습니다. 정확히 입력해주세요.\n");
            printf("예) Pizza ShrimpPizza 10000\n\n");
            while ((ann = getchar()) != '\n');
            continue;
        }
        ann = getchar();
        if (ann == ' ' || ann == '\t' || ann == '\n')
        {
            scanf("%s", men);
            trim(men, trim_men, STRING_SIZE);
            if (strlen(trim_men) < 2 || strlen(trim_men) > 16)
            {
                system("cls");
                printf("메뉴 입력이 잘못되었습니다. 정확히 입력해주세요.\n");
                printf("예) Pizza ShrimpPizza 10000\n\n");
                while ((ann = getchar()) != '\n');
                continue;
            }
            ann = getchar();
            if (ann == ' ' || ann == '\t' || ann == '\n')
            {
                scanf(" %d", &price);
                if (price < 1000 || price > 1000000)
                {
                    system("cls");
                    printf("메뉴 입력이 잘못되었습니다. 정확히 입력해주세요.\n");
                    printf("예) Pizza ShrimpPizza 10000\n\n");
                    while ((ann = getchar()) != '\n');
                    continue;
                }
                while ((ann = getchar()) != '\n')
                {
                    if (ann == ' ' || ann == '\t')
                    {

                    }
                    else
                    {
                        k = 1;
                    }
                }
            }
            else
            {
                k = 1;
            }
        }
        else
        {
            k = 1;
        }
        if (k != 1)
        {
            trim(cha, trim_cha, STRING_SIZE);
            trim(men, trim_men, STRING_SIZE);
            for (int i = 0; i < strlen(cha); i++)
            {
                if (!((trim_cha[i] >= 48 && trim_cha[i] <= 57) || (trim_cha[i] >= 65 && trim_cha[i] <= 90) || (trim_cha[i] >= 97 && trim_cha[i] <= 122)))
                {
                    system("cls");
                    printf("메뉴 입력이 잘못되었습니다. 정확히 입력해주세요.\n");
                    printf("예) Pizza ShrimpPizza 10000\n\n");
                    o = 1;
                    break;
                }
            }
            if (o == 1)
            {
                continue;
            }
            for (int i = 0; i < strlen(men); i++)
            {
                if (!((trim_men[i] >= 48 && trim_men[i] <= 57) || (trim_men[i] >= 65 && trim_men[i] <= 90) || (trim_men[i] >= 97 && trim_men[i] <= 122)))
                {
                    system("cls");
                    printf("메뉴 입력이 잘못되었습니다. 정확히 입력해주세요.\n");
                    printf("예) Pizza ShrimpPizza 10000\n\n");
                    o = 1;
                    break;
                }
            }
            if (mode == 0) {
                for (int i = 0; i < *index; i++) {
                    if (!(strcmp(m[i].category, cha)) && !(strcmp(m[i].menu_name, men))) {
                        printf("동일한 메뉴가 존재합니다.\n");
                        Sleep(1000);
                        system("cls");
                        o = 1;
                        break;
                    }
                }
            }
            else if (mode == 1 || mode == 2) {
                int check = 0;
                for (int i = 0; i < *index; i++) {
                    if (!(strcmp(m[i].category, cha)) && !(strcmp(m[i].menu_name, men)) && m[i].menu_price == price) {
                        check++;
                        break;
                    }
                }
                if (check == 0) {
                    printf("해당 메뉴는 없는 메뉴입니다.\n");
                    Sleep(1000);
                    system("cls");
                    o = 1;
                }
            }
            else if (mode == -1) {
                if (!(strcmp(chate, cha)) && !(strcmp(menuname, men)) && (*menuprice == price)) {
                    strcpy(chate, cha);
                    strcpy(menuname, men);
                    *menuprice = price;
                    return 0;
                }
                for (int i = 0; i < *index; i++) {
                    if (!(strcmp(m[i].category, cha)) && !(strcmp(m[i].menu_name, men)) && m[i].menu_price == price) {
                        printf("동일한 메뉴가 존재합니다.\n");
                        Sleep(1000);
                        system("cls");
                        o = 1;
                        break;
                    }
                }
            }
            if (o == 1)
            {
                continue;
            }
            strcpy(chate, cha);
            strcpy(menuname, men);
            *menuprice = price;
            return 0;
        }
        if (k == 1)
        {
            system("cls");
            printf("메뉴 입력이 잘못되었습니다. 정확히 입력해주세요.\n");
            printf("예) Pizza ShrimpPizza 10000\n\n");
        }
        //while ((ann = getchar()) != '\n');
    }

}

void menu_add(struct Menu* m, int* index)
{
    char cha[STRING_SIZE];
    char men[STRING_SIZE];
    char chprice[STRING_SIZE];
    int price;
    int re;
    system("cls");
    re = menu_input(m, index, cha, men, &price, 0);
    if (re == 1) {
        return;
    }
    sprintf(chprice, "%d", price);
    strcpy(m[*index].category, cha);
    strcpy(m[*index].menu_name, men);
    m[*index].menu_price = price;
    (*index)++;
    FILE* menuFile = fopen("menu.txt", "a");
    if (menuFile != NULL) {
        fputs("\n", menuFile);
        fputs(cha, menuFile);
        fputs(" ", menuFile);
        fputs(men, menuFile);
        fputs(" ", menuFile);
        fputs(chprice, menuFile);
        fclose(menuFile);
    }
    printf("카테고리:%s\n", cha);
    printf("메뉴명:%s\n", men);
    printf("금액:%d\n", price);
    printf("을 메뉴에 추가하였습니다.");
    Sleep(5000);
}

void menu_change(struct Menu* m, int* index)
{
    char cha1[STRING_SIZE];
    char men1[STRING_SIZE];
    int price1;
    char cha2[STRING_SIZE];
    char men2[STRING_SIZE];
    int price2;
    int re;
    system("cls");
    re = menu_input(m, index, cha1, men1, &price1, 1);
    if (re == 1) {
        return;
    }
    strcpy(cha2, cha1);
    strcpy(men2, men1);
    price2 = price1;
    menu_input(m, index, cha2, men2, &price2, -1);
    if (!(strcmp(cha1, cha2)) && !(strcmp(men1, men2)) && price1 == price2) {
        printf("변경 사항이 없습니다.\n");
        Sleep(1000);
    }
    else {
        for (int i = 0; i < *index; i++) {
            if (!(strcmp(m[i].category, cha1)) && !(strcmp(m[i].menu_name, men1))) {
                strcpy(m[i].category, cha2);
                strcpy(m[i].menu_name, men2);
                m[i].menu_price = price2;
                break;
            }
        }
        FILE* menuFile = fopen("menu.txt", "w");
        char chprice[STRING_SIZE];
        if (menuFile != NULL) {
            for (int i = 1; i < *index; i++) {
                sprintf(chprice, "%d", m[i].menu_price);
                fputs("\n", menuFile);
                fputs(m[i].category, menuFile);
                fputs(" ", menuFile);
                fputs(m[i].menu_name, menuFile);
                fputs(" ", menuFile);
                fputs(chprice, menuFile);
            }
            fclose(menuFile);
        }

        printf("카테고리:%s -> %s\n", cha1, cha2);
        printf("메뉴명:%s -> %s\n", men1, men2);
        printf("금액:%d -> %d\n", price1, price2);
        printf("으로 변경하였습니다.");
        Sleep(5000);
    }
}

void menu_del(struct Menu* m, int* index)
{
    char cha[STRING_SIZE];
    char men[STRING_SIZE];
    int price;
    int del;
    int re;
    system("cls");
    re = menu_input(m, index, cha, men, &price, 2);
    if (re == 1) {
        return;
    }
    for (int i = 0; i < *index; i++) {
        if (!(strcmp(m[i].category, cha)) && !(strcmp(m[i].menu_name, men))) {
            del = i;
            break;
        }
    }
    strcpy(m[del].category, m[*index - 1].category);
    strcpy(m[del].menu_name, m[*index - 1].category);
    m[del].menu_price = m[*index - 1].menu_price;
    (*index)--;
    FILE* menuFile = fopen("menu.txt", "w");
    char chprice[STRING_SIZE];
    if (menuFile != NULL) {
        for (int i = 1; i < *index; i++) {
            sprintf(chprice, "%d", m[i].menu_price);
            fputs("\n", menuFile);
            fputs(m[i].category, menuFile);
            fputs(" ", menuFile);
            fputs(m[i].menu_name, menuFile);
            fputs(" ", menuFile);
            fputs(chprice, menuFile);
        }
        fclose(menuFile);
    }
    printf("카테고리:%s\n", cha);
    printf("메뉴명:%s\n", men);
    printf("금액:%d\n", price);
    printf("을 메뉴에서 삭제하였습니다.");
    Sleep(5000);
}

int main(void)
{
    struct Cart cart;
    cart.count = 0; // 장바구니 초기화

    FILE* txttest = fopen("menu.txt", "r");
    if (txttest == NULL) {
        printf("메뉴 정보 데이터 파일을 새로 생성했습니다.");
        Sleep(1000);
        system("cls");
        FILE* txttest = fopen("menu.txt", "w");
        fclose(txttest);
    }
    else {
        fclose(txttest);
    }

    int logyear = 0, logmonth = -1, logday = -1;
    FILE* dateFile = fopen("log.txt", "r");
    if (dateFile != NULL) {
        char buffer[STRING_SIZE];
        while (!feof(dateFile)) {
            fgets(buffer, sizeof(buffer), dateFile);
            char* ptr = strtok(buffer, " ");
            int ab = 0;
            while (ptr != NULL) {
                if (ab == 0) {
                    char* date = strtok(ptr, "-");
                    int cd = 0;
                    while (date != NULL) {
                        if (cd == 0) {
                            logyear = atoi(date);
                        }
                        else if (cd == 1) {
                            logmonth = atoi(date);
                        }
                        else if (cd == 2) {
                            logday = atoi(date);
                        }
                        cd++;
                        date = strtok(NULL, "-");
                    }
                }
                ptr = strtok(NULL, " ");
                ab++;
            }
        }
        fclose(dateFile);
    }
    else {
        //fclose(dateFile);
    }
    struct Menu menulist[STRING_SIZE];
    FILE* menuFile = fopen("menu.txt", "r");
    int index = 0;
    srand(time(NULL));
    if (menuFile != NULL) {
        char buffer[STRING_SIZE];
        while (!feof(menuFile)) {
            fgets(buffer, sizeof(buffer), menuFile);
            char* ptr = strtok(buffer, " ");
            int ab = 0;
            while (ptr != NULL) {
                if (ab == 0) {
                    strcpy(menulist[index].category, ptr);
                }
                else if (ab == 1) {
                    strcpy(menulist[index].menu_name, ptr);
                }
                else if (ab == 2) {
                    menulist[index].menu_price = atoi(ptr);
                }
                ptr = strtok(NULL, " ");
                ab++;
            }
            index++;
        }
        fclose(menuFile);
    }

    int man_mode;
    int room = 0;
    int year;
    int month;
    int day;


    date_input(&year, &month, &day, logyear, logmonth, logday);
    system("cls");
    while (1)
    {
        switch (room)
        {
        case 0:
            printf("%d년 %d월 %d일\n", year, month, day);
            room = start_menu();
            break;
        case 1:
            /// 사용자 모드
            system("cls");
            displayFunction(&cart, menulist, index, year, month, day);
            room = 0;
            break;
        case 2:
            system("cls");
            man_mode = man_menu();
            switch (man_mode)
            {
            case 1:
                menu_add(menulist, &index); // 메뉴 추가
                break;
            case 2:
                menu_change(menulist, &index); // 메뉴 변경
                break;
            case 3:
                menu_del(menulist, &index); // 메뉴 삭제
                break;
            case 4:
                system("cls"); // 뒤로 가기
                room = 0;
                break;
            }
            break;
        case 3:
            printf("프로그램을 종료합니다.");
            return 0;
        }
    }
    return 0;
}