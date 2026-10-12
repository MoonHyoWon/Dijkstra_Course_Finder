#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <windows.h>
#include <conio.h> //_getch() 함수를 사용하기 위해 선언

#define BUILDNUM 14 //건물 갯수
#define WEINUM 22 //거리 파일 갯수 (가중치 갯수)
#define CLASSNUM 50 //교양 파일 갯수 (교양 갯수)
#define MARKNUM 700
#define INF 10000 //무한대

enum ColorType {
	BLACK,  	//0
	darkBLUE,	//1
	DarkGreen,	//2
	darkSkyBlue,    //3
	DarkRed,  	//4
	DarkPurple,	//5
	DarkYellow,	//6
	GRAY,		//7
	DarkGray,	//8
	BLUE,		//9
	GREEN,		//10
	SkyBlue,	//11
	RED,		//12
	PURPLE,		//13
	YELLOW,		//14
	WHITE		//15
} COLOR;

typedef struct user
{
	char id[20]; //회원 아이디
	char pw[20]; //회원 비밀번호
	char building[20]; //회원 전공 건물
	char singularBuild[20]; //회원 복수 전공 건물
	//char bookMark[20][20]; //회원별 저장한 북마크
} User;
//User u[50]; //파일에서 저장할 회원 구조체
User r; //회원가입에 사용될 구조체

typedef struct Class
{
	char className[30]; //교양 이름
	char field[20]; //교양 분야
	char building[20]; //교양 건물명
} Class;
Class c[CLASSNUM]; //파일에서 저장할 교양 구조체

Class b[CLASSNUM]; //build의 축약 단어 / 건물 기준 조회에서 사용될 구조체 변수
Class f[BUILDNUM]; //field의 축약 단어 / 분야 기준 조회에서 사용될 구조체 변수

typedef struct book
{
	User u;
	Class bookMark[14][30]; //북마크는 인당 14개만 저장가능 (교양 이름이 저장될 예정)
	int BMindex;
} bookMark;
bookMark user[50];

typedef struct mark
{
	char id[20];
	char className[20];
} Mark;
Mark m[MARKNUM];

typedef struct No
{
	int num;  //건물 노드 번호
	char Build[20]; //건물명
} node;
node Node[BUILDNUM];

typedef struct distan
{
	int firstNum; //시작 건물 번호
	int finalNum; //도착 건물 번호
	int weight; //가중치
} Distan;
Distan dista[WEINUM]; //파일에서 저장할 거리(노드와 가중치) 구조체

int buildArray[BUILDNUM][BUILDNUM]; //다익스트라를 돌리기 위한 2차원 배열

FILE* userFile;
FILE* classFile;
FILE* bookmarkFile;
FILE* distanceFile;
FILE* NodeFile;

char id[20], pw[20], building[20];

int v[BUILDNUM]; //확정된 노드
int d[BUILDNUM]; //거리

typedef struct Nayeol {
	int dis; //비교할 거리
	int count;
}Na;

Na N[BUILDNUM] = { {INF,0}, {INF,0}, {INF,0}, {INF,0} };

int final[BUILDNUM];
char answer[20]; //후에 문자형(혹은 문자열)으로 변경 예정
int check = -1; //아이디에 해당하는 비밀번호, 전공건물을 찾기위한 인덱스
int maxIndex = -1;

void square(int width, int height, int x, int y); //ui 그리는 함수 1
void square3(int width, int height, int x, int y); //ui 그리는 함수2
void square_char(int width, int height, int x, int y, char* msg); //ui그리는 함수 + 가운데 정렬 글씨
void goto_xy(int x, int y); //위치 이동 함수
void start_ui(void); //시작 화면 - 로그인 & 회원가입
void start_fcn(void); //시작 기능
void login_ui(void); //로그인 ui
void join_ui(void); //회원가입 ui
void main_ui(void); //선택화면
void Build_ui(void); //건물 선택 화면 1.전공건물 / 2.복수전공건물
void BuildPrint_ui(int num); //건물 목록 출력 화면 1.등록 / 2.수정(삭제 포함)
void BuildSign_ui(void); //건물 등록 화면
void BuildModify_ui(int num); //건물 수정 화면

void ClassSuggest_ui(void); //교양 추천 화면
void ClassBuild_ui(void); //교양 조회 건물 선택 화면
void ClassResearch_ui(void); //교양 조회 화면
void FieldChoice_ui(void); //분야 선택 화면

void bookMark_ui(void); //북마크 조회 화면

void UserFileOpen(void);
void classFileOpen(void);

void distanceFileOpen(void);
void NodeFileOpen(void); //노드 파일 열고 읽기

int getBuildingIndex(const char* buildName); //건물의 이름을 전달받으면 건물에 해당하는 숫자 리턴

int getSmallIndex(void);
void daikstra(int start);
void Ques(char name[]); //다익스트라 거리 계산 (함수)
void printQues(void); //다익스트라 거리 기준을 통해 출력하기 (함수)
void print_Daikstra(int num); //건물 목록 출력 후 기준을 통해 출력하기 (본체)

void login_fcn(void); //로그인 함수 (ui호출 포함)
void join_fcn(void); //회원가입 함수 (ui호출 포함)
void Build_fcn(void); //건물 선택 함수 (ui호출 포함)
void BuildPrint_fcn(int num); //건물 목록 함수 (ui호출 포함) / 다익스트라 기준 건물 출력
void buildSign_fcn(int num); //건물 등록 함수 (ui호출 포함)
void buildSign(void); //건물 등록 파일처리 함수 (건물 등록 함수에서 호출될 예정)
void BuildModify_fcn(int num); //건물 수정 함수 (ui호출 미포함)

void ClassSuggest_fcn(void); //교양 추천 함수
void ClassResearch_fcn(int num, int product); //교양 조회 함수
void ClassBuild_fcn(void); //교양 조회 건물 선택 함수
void ClassPrint(Class str[], int product); //교양 출력 함수
void FieldChoice_fcn(void); //분야 선택 함수
void getClassBuild(int num); //건물 기준으로 정렬된 교양 목록을 얻는 함수
void getClassField(char str[]); //입력 받은 교양(str)기준으로 정렬된 교양 목록을 얻는 함수

void addBookMark(Class str[], int num, int product, int productNum); //북마크 추가 함수
void deleteBookMark(int productNum); //북마크 삭제 함수
void printBookMark();
void bookMark_fcn(void);

void printBuild(void); //건물 등록 X시의 건물 출력 함수

int main(void)
{
	system("mode con: cols=80 lines=57");
	UserFileOpen();
	NodeFileOpen();
	distanceFileOpen();
	classFileOpen();

	start_fcn();

	return 0;
}

//회원 파일 열고 읽기
void UserFileOpen(void)
{
	userFile = fopen("user.txt", "r");
	if (userFile == NULL) {
		printf("파일을 열 수 없습니다.\n");
	} //파일이 존재하지 않을 경우에 출력되는 출력문

	int i = 0;
	while (fscanf(userFile, "%s %s %s %s", user[i].u.id, user[i].u.pw, user[i].u.building, user[i].u.singularBuild) != EOF) {
		maxIndex = i;
		i++;
		if (i >= 50) break;
	} //scnaf는 띄어쓰기는 읽을 수 없기에 띄어쓰기로 구분하여 파일에 있는 값을 변수에 저장 //파일에서 읽을 것이 없을 때까지 or 50개 이상일 경우 while문을 빠져나온다.

	fclose(userFile); //구조체에 따로 저장했기에 파일을 굳이 열어둘 필요가 없다.
}

//북마크 파일 열고 읽기
void bookmarkFileOpen(void)
{
	char id[20] = { 0 };
	char mark[30] = { 0 };

	bookmarkFile = fopen("bookmark.txt", "r");
	if (bookmarkFile == NULL) {
		printf("파일을 열 수 없습니다.\n");
	} //파일이 존재하지 않을 경우에 출력되는 출력문

	for (int i = 0; i < BUILDNUM; i++) {
		strcpy(user[check].bookMark[i]->className, "");
		strcpy(user[check].bookMark[i]->building, "");
		strcpy(user[check].bookMark[i]->field, "");
	}

	int bmi = 0;
	while (fscanf(bookmarkFile, "%s %s", id, mark) != EOF) {
		if (strcmp(user[check].u.id, id) == 0) {
			user[check].BMindex = bmi;
			strcpy(user[check].bookMark[user[check].BMindex]->className, mark);
			for (int k = 0; k < CLASSNUM; k++) {
				if (strcmp(user[check].bookMark[user[check].BMindex]->className, c[k].className) == 0) {
					strcpy(user[check].bookMark[user[check].BMindex]->building, c[k].building);
					strcpy(user[check].bookMark[user[check].BMindex]->field, c[k].field);
				}
			}
			bmi++;
		}
	} //파일에서 읽을 것이 없을 때까지 //scnaf는 띄어쓰기는 읽을 수 없기에 띄어쓰기로 구분하여 파일에 있는 값을 변수에 저장

	fclose(bookmarkFile); //구조체에 따로 저장했기에 파일을 굳이 열어둘 필요가 없다.
}


//교양 파일 열고 읽기
void classFileOpen(void)
{
	classFile = fopen("class.txt", "rt");
	if (classFile == NULL) {
		printf("파일을 열 수 없습니다.\n");
	} //파일 안에 값이 존재하지 않거나 파일 자체가 존재하지 않을 경우 열리지 않는다.

	int i = 0;
	while (fscanf(classFile, "%s  %s  %s", &c[i].className, &c[i].field, &c[i].building) != EOF) {
		i++;
		if (i >= CLASSNUM) break;
	} //scnaf는 띄어쓰기는 읽을 수 없기에 띄어쓰기로 구분하여 파일에 있는 값을 변수에 저장

	fclose(classFile); //구조체에 따로 저장했기에 파일을 굳이 열어둘 필요가 없다.
}

//거리 파일 열고 읽기
void distanceFileOpen(void)
{
	int i, j;
	distanceFile = fopen("distance.txt", "r");
	if (distanceFile == NULL) {
		printf("파일을 열 수 없습니다.\n");
	} //파일 안에 값이 존재하지 않거나 파일 자체가 존재하지 않을 경우 열리지 않는다.

	for (i = 0; i < WEINUM; i++) {
		fscanf(distanceFile, "%d %d %d", &dista[i].firstNum, &dista[i].finalNum, &dista[i].weight);
	} //scnaf는 띄어쓰기는 읽을 수 없기에 띄어쓰기로 구분하여 파일에 있는 값을 변수에 저장

	fclose(distanceFile); //구조체와 배열에에 따로 저장했기에 파일을 굳이 열어둘 필요가 없다.

	for (i = 0; i < BUILDNUM; i++) {
		for (j = 0; j < BUILDNUM; j++) {
			buildArray[i][j] = INF;
		}
	} //다익스트라 2차원 배열을 INF(무한대)로 초기화

	for (int i = 0; i < WEINUM; i++) {
		if (dista[i].firstNum != -1 && dista[i].finalNum != -1) { //두 인덱스 값이 정상적으로 있을 때
			buildArray[dista[i].firstNum][dista[i].finalNum] = dista[i].weight;
			buildArray[dista[i].finalNum][dista[i].firstNum] = dista[i].weight;
		}
	}

	for (i = 0; i < BUILDNUM; i++) {
		buildArray[i][i] = 0;
	} //자기 자신과의 이동 가중치는 0으로 설정
}

//노드 파일 열고 읽기
void NodeFileOpen(void)
{
	int i, j;
	NodeFile = fopen("node.txt", "r");
	if (NodeFile == NULL) {
		printf("파일을 열 수 없습니다.\n");
	} //파일 안에 값이 존재하지 않거나 파일 자체가 존재하지 않을 경우 열리지 않는다.

	for (i = 0; i < BUILDNUM; i++) {
		fscanf(NodeFile, "%d %s", &Node[i].num, Node[i].Build);
	} //scnaf는 띄어쓰기는 읽을 수 없기에 띄어쓰기로 구분하여 파일에 있는 값을 변수에 저장

	fclose(NodeFile); //구조체와 배열에에 따로 저장했기에 파일을 굳이 열어둘 필요가 없다.
}

//시작 기능 (ui호출 포함)
void start_fcn(void)
{
	start_ui();
	goto_xy(15, 54);
	scanf("%s", answer);

	if (strcmp(answer, "1") == 0) {
		login_fcn();
	}
	else if (strcmp(answer, "2") == 0) {
		join_fcn();
	}
	else {
		start_fcn(); //자기 자신을 호출할지는 생각해봐야함~ ///////////////////////////////////////////////////////////////
	}
}

//로그인 함수 (ui호출 포함)
void login_fcn(void)
{
	login_ui();

	int num = 0, ch = 0;

	goto_xy(14, 18);
	scanf("%s", id);
	if (strcmp(id, "@") != 0) {
		for (int i = 0; i < 50; i++) {
			if (strcmp(user[i].u.id, id) == 0) {
				check = i;
				break;
			}
		}
		if (check != -1) {
			while (1) {
				char str[20] = { 0 };
				goto_xy(14, 31);
				num = 0;
				while ((ch = _getch()) != '\r') {
					str[num] = (char)ch;
					if (str[num] == '\b') { //BackSpace가 입력될 경우
						continue;
					}
					printf("*");
					num++;
				}

				for (int i = 0; i < 20; i++) {
					pw[i] = str[i];
				}

				if (strcmp(user[check].u.pw, pw) == 0) {
					break;
				}
				else if (strcmp(pw, "@") == 0) {
					start_fcn();
				}
				goto_xy(14, 34);
				printf("비밀번호가 일치하지 않습니다.");
				Sleep(800);
				goto_xy(14, 34);
				printf("                              "); //비밀번호가 일치하지 않습니다 지우기
				goto_xy(14, 31);
				printf("                          "); //비밀번호 지우기
				goto_xy(14, 31);
			}

			main_ui();
		}
		else {
			goto_xy(14, 22);
			printf("일치하는 아이디가 없습니다.");
			Sleep(600);
			start_fcn();
		}
	}
	else start_fcn();
}

//회원가입 함수 (ui호출 포함)
void join_fcn(void)
{
	int flag = 1;

	join_ui();
	goto_xy(14, 18);
	scanf("%s", r.id);
	if (strcmp(r.id, "@") != 0) {
		for (int i = 0; i < 50; i++) {
			if (strcmp(user[i].u.id, r.id) == 0) {
				flag = 0;
				break;
			}
		}
		if (flag == 0) {
			goto_xy(14, 21);
			printf("이미 존재하는 아이디입니다. 다른 아이디를 입력하세요.");
			join_fcn();
		}
		else {
			goto_xy(14, 31);
			scanf("%s", r.pw);
			if (strcmp(r.pw, "@") != 0) {
				userFile = fopen("user.txt", "a+");
				if (userFile == NULL) {
					printf("파일을 열 수 없습니다.\n");
				}

				fprintf(userFile, "\n%s %s %s %s", r.id, r.pw, "none", "none"); //전공건물은 로그인 후 등록하기에 입력 X

				fclose(userFile);

				UserFileOpen(); //파일에 업로드 해준 후 프로그램의 구조체에 다시 변수값을 입력받는다.
			}
		}
	}

	start_fcn();
}

//건물 선택 함수 (ui호출 포함)
void Build_fcn(void)
{
	Build_ui();

	goto_xy(15, 54);
	scanf("%s", answer);

	if (strcmp(answer, "1") == 0) {
		BuildPrint_fcn(1);
	}
	else if (strcmp(answer, "2") == 0) {
		BuildPrint_fcn(2);
	}
	else if (strcmp(answer, "0") == 0) {
		main_ui();
	}
	else {
		Build_fcn();
	}
}

//건물 목록 함수 (ui호출 포함) / 다익스트라 기준 건물 출력
void BuildPrint_fcn(int num)
{
	BuildPrint_ui(num);

	if (num == 1) {
		if (strcmp(user[check].u.building, "none") != 0) { //건물이 등록 되어 있는지 확인
			print_Daikstra(1);
		}
		else { //없을 경우 그냥 목록 출력하는 반복문
			printBuild();
		}
	}
	else if (num == 2) {
		if (strcmp(user[check].u.singularBuild, "none") != 0) { //건물이 등록 되어 있는지 확인
			print_Daikstra(2);
		}
		else { //없을 경우 그냥 목록 출력하는 반복문
			printBuild();
		}
	}

	goto_xy(15, 54);
	scanf("%s", answer);

	if (strcmp(user[check].u.building, "none") == 0) {
		if (strcmp(answer, "1") != 0) {
			BuildPrint_fcn(1);
		}
	}
	if (strcmp(answer, "1") == 0) {
		if (num == 1 && strcmp(user[check].u.building, "none") != 0) { //건물이 등록 되어 있는지 확인
			goto_xy(4, 45);
			square_char(71, 30, 4, 6, "건물이 이미 등록 되어 있습니다.");
			//건물 출력할 네모 화면에 맞춰 글씨 출력
			//printf("건물이 등록 되어 있습니다.");
			Sleep(1000);
			BuildPrint_fcn(1); //등록되어 있을 경우 다시 원점으로 복귀

		}
		else if (num == 2 && strcmp(user[check].u.singularBuild, "none") != 0) { //건물이 등록 되어 있는지 확인
			goto_xy(4, 45);
			square_char(71, 30, 4, 6, "건물이 이미 등록 되어 있습니다.");
			//건물 출력할 네모 화면에 맞춰 글씨 출력
			//printf("건물이 등록 되어 있습니다.");
			Sleep(1000);
			BuildPrint_fcn(2); //등록되어 있을 경우 다시 원점으로 복귀

		}
		else {
			buildSign_fcn(num); //등록이 안 됐을 경우 등록 함수로 이동
		}
	}
	else if (strcmp(answer, "2") == 0) {
		BuildModify_fcn(num); //수정 함수로 이동
	}
	else if (strcmp(answer, "0") == 0) {
		Build_fcn();
	}
	else {
		BuildPrint_fcn(num); //잘못 입력시 자기 자신으로 복귀
	}
}

//건물 등록 함수
void buildSign_fcn(int num)
{
	BuildSign_ui();

	int check_building = 0;
	printBuild();

	goto_xy(31, 45);
	scanf("%s", answer);

	if (num == 1) {
		for (int i = 0; i < 14; i++) {
			if (strcmp(Node[i].Build, answer) == 0) {
				check_building = 1;
				break;
			}
		}
		if (check_building == 1) {
			strcpy(user[check].u.building, answer);
			buildSign();
		}
		else {
			goto_xy(6, 48);
			printf("올바르지 않은 건물명입니다.");
		}
	}
	else if (num == 2) {
		for (int i = 0; i < 14; i++) {
			if (strcmp(Node[i].Build, answer) == 0) {
				check_building = 1;
				break;
			}
		}
		if (check_building == 1) {
			if (strcmp(user[check].u.building, answer) == 0) {
				goto_xy(6, 48);
				printf("전공 건물을 선택하였습니다.");
			}
			else {
				strcpy(user[check].u.singularBuild, answer);
				buildSign();
			}
		}
		else {
			goto_xy(6, 48);
			printf("올바르지 않은 건물명입니다.");
		}
	}

	Sleep(1000);
	BuildPrint_fcn(num); //추가 연산 이후 목록으로 복귀
}

//건물 등록 파일 처리 함수 (건물 등록 함수에서 호출될 예정)
void buildSign(void)
{
	FILE* signFile;

	signFile = fopen("sign.txt", "w+");
	if (signFile == NULL) {
		printf("파일을 열 수 없습니다.\n");
	}

	for (int i = 0; i <= maxIndex; i++) {
		fprintf(signFile, "%s %s %s %s\n", user[i].u.id, user[i].u.pw, user[i].u.building, user[i].u.singularBuild);
		if (i >= 50) break;
	}

	fclose(signFile);

	remove("user.txt");             // 원래 파일 삭제
	rename("sign.txt", "user.txt"); // 임시 파일을 원래 파일로 이름 변경

	UserFileOpen(); //파일에 업로드 해준 후 프로그램의 구조체에 다시 변수값을 입력받는다.
}

//건물 수정 함수
void BuildModify_fcn(int num)
{
	BuildModify_ui(num);

	int check_building = 0;
	if (num == 1) {
		if (strcmp(user[check].u.building, "none") != 0) { //건물이 등록 되어 있는지 확인
			print_Daikstra(1);
		}
	}
	else if (num == 2) {
		if (strcmp(user[check].u.singularBuild, "none") != 0) { //건물이 등록 되어 있는지 확인
			print_Daikstra(2);
		}
	}

	goto_xy(33, 41);
	scanf("%s", answer);

	if (num == 2 && strcmp(answer, "@") == 0) {
		strcpy(user[check].u.singularBuild, "none");
		buildSign();
	}
	if (num == 1) {
		for (int i = 0; i < 14; i++) {
			if (strcmp(Node[i].Build, answer) == 0) {
				check_building = 1;
				break;
			}
		}
		if (check_building == 1) {
			strcpy(user[check].u.building, answer);
			buildSign();
		}
		else {
			goto_xy(6, 44);
			printf("올바르지 않은 건물명입니다.");
		}
	}
	else if (num == 2) {
		for (int i = 0; i < 14; i++) {
			if (strcmp(Node[i].Build, answer) == 0) {
				check_building = 1;
				break;
			}
		}
		if (strcmp(user[check].u.building, answer) == 0) {
			goto_xy(6, 48);
			printf("전공 건물을 선택하였습니다.");
		}
		else if (check_building == 1) {
			strcpy(user[check].u.singularBuild, answer);
			buildSign();
		}
		else {
			if (strcmp(answer, "@") == 0) {
				goto_xy(6, 48);
				printf("건물이 삭제되었습니다.");
			}
			else {
				goto_xy(6, 48);
				printf("올바르지 않은 건물명입니다.");
			}
		}
	}

	Sleep(1000);
	BuildPrint_fcn(num);
}

//건물 삭제 함수
void BuildDelete_fcn(void)
{
	FILE* signFile;

	signFile = fopen("sign.txt", "w+");
	if (signFile == NULL) {
		printf("파일을 열 수 없습니다.\n");
	}

	for (int i = 0; i <= maxIndex; i++) {
		fprintf(signFile, "%s %s %s %s\n", user[i].u.id, user[i].u.pw, user[i].u.building, user[i].u.singularBuild);
		if (i >= 50) break;
	}

	fclose(signFile);

	remove("user.txt");             // 원래 파일 삭제
	rename("sign.txt", "user.txt"); // 임시 파일을 원래 파일로 이름 변경

	UserFileOpen(); //파일에 업로드 해준 후 프로그램의 구조체에 다시 변수값을 입력받는다.
}

//교양 추천 함수
void ClassSuggest_fcn(void)
{
	ClassSuggest_ui();

	goto_xy(15, 54);
	scanf("%s", answer); //몇번 선택할 것인지 선택

	if (strcmp(answer, "1") == 0) {
		ClassBuild_fcn(); //건물 기준 조회
	}
	else if (strcmp(answer, "2") == 0) {
		FieldChoice_fcn(); //분야 기준 조회
	}
	else if (strcmp(answer, "0") == 0) {
		main_ui(); //메인화면으로 복귀
	}
	else ClassSuggest_fcn;
}

//건물 기준 조회 건물 선택 함수 ++++++++++++++++++++
void ClassBuild_fcn(void)
{
	ClassBuild_ui();
	goto_xy(15, 54);
	scanf("%s", answer); //몇번 선택할 것인지 선택

	if (strcmp(answer, "1") == 0) {
		getClassBuild(1); //전공건물 선택
		ClassResearch_fcn(1, 0);
	}
	else if (strcmp(answer, "2") == 0) {
		getClassBuild(2); //복수 전공 건물 선택
		ClassResearch_fcn(1, 0);
	}
	else ClassBuild_fcn();
}

//교양 조회 함수
void ClassResearch_fcn(int num, int product)
{
	ClassResearch_ui();

	if (num == 1) {
		square_char(41, 2, 19, 2, "건 물 기 준 교 양 목 록");
		ClassPrint(b, product);
	}
	else if (num == 2) {
		square_char(41, 2, 19, 2, "분 야 기 준 교 양 목 록");
		ClassPrint(f, product);
	}

	goto_xy(14, 54);
	scanf("%s", answer);

	int choiceNum = 0;
	choiceNum = atoi(answer) + (product * 14);

	if (strcmp(answer, "0") == 0) {
		ClassSuggest_fcn(); //메인화면으로 복귀
	}
	else if (strcmp(answer, "a") == 0) {
		if (num == 1) { //건물 기준 조회
			if (product > 0) {
				ClassResearch_fcn(1, product - 1);
			}
			else ClassResearch_fcn(1, product);
		}
		else if (num == 2) { //분야 기준 조회
			if (product > 0) {
				ClassResearch_fcn(2, product - 1);
			}
			else ClassResearch_fcn(2, product);
		}
	}
	else if (strcmp(answer, "d") == 0) {
		if (num == 1) { //건물 기준 조회
			if (product < 3) {
				ClassResearch_fcn(1, product + 1);
			}
			else ClassResearch_fcn(1, product);
		}
		else if (num == 2) { //분야 기준 조회
			ClassResearch_fcn(2, product);
		}
	}
	else if (choiceNum % 14 >= 0 && choiceNum % 14 <= 13) {
		if (choiceNum % 14 == 0) {
			choiceNum = 14;
		}
		else choiceNum = choiceNum % 14;
		if (num == 1) {
			addBookMark(b, num, product, choiceNum % 15); // 14까지 제대로 인식하기 위해
		}
		else if (num == 2) {
			addBookMark(f, num, product, choiceNum % 15);
		}
	}
	else ClassResearch_fcn(num, product);
}

//분야 조회 선택 함수
void FieldChoice_fcn(void)
{
	FieldChoice_ui();

	goto_xy(15, 54);
	scanf("%s", answer);

	if (strcmp(answer, "1") == 0) {
		getClassField("문학,예술,문화");
	}
	else if (strcmp(answer, "2") == 0) {
		getClassField("역사와철학");
	}
	else if (strcmp(answer, "3") == 0) {
		getClassField("정치,경제,사회");
	}
	else if (strcmp(answer, "4") == 0) {
		getClassField("과학과기술");
	}
	else if (strcmp(answer, "5") == 0) {
		getClassField("실용실천");
	}
	else if (strcmp(answer, "0") == 0) {
		ClassSuggest_fcn();
	}

	ClassResearch_fcn(2, 0);
}

//건물 기준으로 정렬된 교양 목록을 얻는 함수
void getClassBuild(int num)
{
	int i, j, k = 0;
	for (i = 0; i < BUILDNUM; i++) {
		daikstra(i);
	}

	if (num == 1) {
		Ques(user[check].u.building);

		for (i = 0; i < BUILDNUM; i++) { //건물 갯수만큼 반복
			for (j = 0; j < CLASSNUM; j++) { //교양 갯수만큼 반복
				if (strcmp(Node[final[i]].Build, c[j].building) == 0) {
					strcpy(b[k].building, c[j].building);
					strcpy(b[k].className, c[j].className);
					strcpy(b[k].field, c[j].field);
					k++;
				}
			}
		}
	}
	else if (num == 2) {
		if (strcmp(user[check].u.singularBuild, "none") == 0) {
			ClassBuild_fcn();
		}
		else {
			Ques(user[check].u.singularBuild);

			for (i = 0; i < BUILDNUM; i++) { //건물 갯수만큼 반복
				for (j = 0; j < CLASSNUM; j++) { //교양 갯수만큼 반복
					if (strcmp(Node[final[i]].Build, c[j].building) == 0) {
						strcpy(b[k].building, c[j].building);
						strcpy(b[k].className, c[j].className);
						strcpy(b[k].field, c[j].field);
						k++;
					}
				}
			}
		}
	}
}

//분야 기준으로 교양 목록 획득 함수
void getClassField(char str[])
{
	int i, j, k = 0;
	for (i = 0; i < BUILDNUM; i++) {
		daikstra(i);
	}

	Ques(user[check].u.building);

	for (int i = 0; i < BUILDNUM; i++) {
		strcpy(f[i].building, "");
		strcpy(f[i].className, "");
		strcpy(f[i].field, "");
	}

	for (i = 0; i < BUILDNUM; i++) { //건물 갯수만큼 반복
		for (j = 0; j < CLASSNUM; j++) { //교양 갯수만큼 반복
			if (strcmp(c[j].field, str) == 0) { //교양의 분야가 입력 받은 분야와 일치한다면
				if (strcmp(Node[final[i]].Build, c[j].building) == 0) { //정렬된 건물과 일치하는 것을 대입
					strcpy(f[k].building, c[j].building);
					strcpy(f[k].className, c[j].className);
					strcpy(f[k].field, c[j].field);
					k++;
				}
			}
		}
	}
}

//북마크 추가 함수
void addBookMark(Class str[], int num, int product, int productNum)
{
	int findNum = ((product * 14) + productNum) - 1;
	int flag = 0;

	if (user[check].BMindex == -1)
		user[check].BMindex++; //첫 북마크 기능의 경우 bmIndex == 0

	if (user[check].BMindex >= 0 && user[check].BMindex < BUILDNUM - 1) { //북마크 저장의 갯수 14개(0 ~ 13까지)
		for (int i = 0; i <= user[check].BMindex; i++) {
			if (strcmp(str[findNum].className, user[check].bookMark[i]->className) == 0) {
				flag = 1; //이미 교양이 북마크에 존재할 경우
				goto_xy(6, 42);
				printf("%d번 교양이 이미 북마크에 존재합니다.", findNum + 1);
				Sleep(1600);
				goto_xy(6, 42);
				printf("                                        ");
				break;
			}
		}
		if (flag == 0) {
			strcpy(user[check].bookMark[user[check].BMindex]->className, str[findNum].className);
			strcpy(user[check].bookMark[user[check].BMindex]->building, str[findNum].building);
			strcpy(user[check].bookMark[user[check].BMindex]->field, str[findNum].field);
			user[check].BMindex++;

			bookmarkFile = fopen("bookmark.txt", "a+");
			if (bookmarkFile == NULL) {
				printf("파일을 열 수 없습니다.\n");
			}
			else {
				fprintf(bookmarkFile, "%s %s\n", user[check].u.id, user[check].bookMark[user[check].BMindex - 1]->className);

				fclose(bookmarkFile);

				bookmarkFileOpen(); //파일에 업로드 해준 후 프로그램의 구조체에 다시 변수값을 입력받는다.

				goto_xy(6, 42);
				printf("%d번 교양이 북마크에 추가되었습니다.", findNum + 1);
				Sleep(1600);
				goto_xy(6, 42);
				printf("                                       ");
			}
		}
	}
	else {
		square_char(71, 30, 4, 9, "북마크가 꽉 찼습니다.");
		goto_xy(21, 25);
		printf("사용하지 않는 북마크 목록을 비워주세요.");
		Sleep(3000);
	}

	ClassResearch_fcn(num, product);
}

//북마크 삭제를 묻는 함수
int deleteQuest(int productNum)
{
	square_char(71, 30, 4, 9, "해당 교양을 북마크에서 삭제하시겠습니까? (y/n)");
	goto_xy((80 - (strlen(user[check].bookMark[productNum]->className)) - 3) / 2, 22);
	printf("[ %s ]", user[check].bookMark[productNum]->className);
	goto_xy(36, 26);
	printf(">> ");
	scanf("%s", answer);
	Sleep(1000);

	if (strcmp(answer, "y") == 0 || strcmp(answer, "Y") == 0) {
		return 1;
	}
	else return 0;
}

//북마크 삭제 함수
void deleteBookMark(int productNum)
{
	char id[20] = { 0 };
	char mark[30] = { 0 };

	bookmarkFile = fopen("bookmark.txt", "r");
	if (bookmarkFile == NULL) {
		printf("파일을 열 수 없습니다.\n");
	} //파일이 존재하지 않을 경우에 출력되는 출력문

	FILE* markNotFile;

	markNotFile = fopen("marknot.txt", "w+");
	if (markNotFile == NULL) {
		printf("파일을 열 수 없습니다.\n");
	}

	int i = 0;
	while (fscanf(bookmarkFile, "%s %s", m[i].id, m[i].className) != EOF) {
		if (strcmp(user[check].bookMark[productNum]->className, m[i].className) == 0 &&
			strcmp(user[check].u.id, m[i].id) == 0) {
			continue; //
		}
		else fprintf(markNotFile, "%s %s\n", m[i].id, m[i].className);
		i++;
	} //////////////// ***********************************************************************************여기 수정해야됨! 0605 문제 없는 것 같은데?

	fclose(bookmarkFile); //구조체에 따로 저장했기에 파일을 굳이 열어둘 필요가 없다.
	fclose(markNotFile);

	remove("bookmark.txt");             // 원래 파일 삭제
	rename("marknot.txt", "bookmark.txt"); // 임시 파일을 원래 파일로 이름 변경

	goto_xy(6, 42);
	printf("%s 교양이 북마크에서 삭제되었습니다.", user[check].bookMark[productNum]->className);
	Sleep(1600);
	goto_xy(6, 42);
	printf("                                                               ");

	bookmarkFileOpen(); //파일에 업로드 해준 후 프로그램의 구조체에 다시 변수값을 입력받는다.
}

void printBookMark(void)
{
	int value = 0; //최종 값
	int choice = 0; //선택한 번호 값
	for (int i = 1; i <= 14; i++) {
		value = i;
		choice = value - 1;
		if (choice >= CLASSNUM) {
			break;
		}
		//전공건물에 속한 교양일때
		if (strcmp(user[check].bookMark[choice]->building, user[check].u.building) == 0) {
			goto_xy(8, 11 + (i - 1) * 2);
			SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), DarkGreen);
			printf("%-4d %-28s %-18s %-12s", value, user[check].bookMark[choice]->className, user[check].bookMark[choice]->field, user[check].bookMark[choice]->building);
			SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), WHITE);
		}
		//복수전공건물에 속한 교양일때
		else if (strcmp(user[check].bookMark[choice]->building, user[check].u.singularBuild) == 0) {
			goto_xy(8, 11 + (i - 1) * 2);
			SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), DarkYellow);
			printf("%-4d %-28s %-18s %-12s", value, user[check].bookMark[choice]->className, user[check].bookMark[choice]->field, user[check].bookMark[choice]->building);
			SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), WHITE);
		}
		//그 외의 건물에 속한 교양일때
		else {
			goto_xy(8, 11 + (i - 1) * 2);
			printf("%-4d %-28s %-18s %-12s", value, user[check].bookMark[choice]->className, user[check].bookMark[choice]->field, user[check].bookMark[choice]->building);
		}
		//%-ns는 왼쪽 정렬된 n칸 출력하는 문자열이다.
	}
}

void bookMark_fcn(void)
{
	bookmarkFileOpen(); //북마크 파일 읽기 함수
	bookMark_ui(); //북마크 UI
	printBookMark(); //북마크 내용 출력 함수

	goto_xy(15, 54);
	scanf("%s", answer);

	int choiceNum = atoi(answer);

	if (strcmp(answer, "0") == 0) {
		main_ui();
	}
	else if (choiceNum % 15 >= 1 && choiceNum % 15 <= 14) {
		int QuestNum = deleteQuest(choiceNum % 15 - 1);
		if (QuestNum == 1) {
			deleteBookMark(choiceNum % 15 - 1);
		}
		else {
			goto_xy(6, 42);
			printf("북마크 삭제가 취소 되었습니다.");
			Sleep(1600);
			goto_xy(6, 42);
			printf("                                                               ");
		}
		bookMark_fcn();
	}

}

//교양 출력 함수
void ClassPrint(Class str[], int product) //교양 정보가 담긴 str배열 / 페이지를 설정할 product
{
	int value = 0; //최종 값
	int choice = 0; //선택한 번호 값
	for (int i = 1; i <= 14; i++) {
		value = i + (14 * product);
		choice = value - 1;
		if (choice >= CLASSNUM) {
			break;
		}
		//전공건물에 속한 교양일때
		if (strcmp(str[choice].building, user[check].u.building) == 0) {
			goto_xy(8, 11 + (i - 1) * 2);
			SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), DarkGreen);
			printf("%-4d %-28s %-18s %-12s", value, str[choice].className, str[choice].field, str[choice].building);
			SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), WHITE);
		}
		//복수전공건물에 속한 교양일때
		else if (strcmp(str[choice].building, user[check].u.singularBuild) == 0) {
			goto_xy(8, 11 + (i - 1) * 2);
			SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), DarkYellow);
			printf("%-4d %-28s %-18s %-12s", value, str[choice].className, str[choice].field, str[choice].building);
			SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), WHITE);
		}
		//그 외의 건물에 속한 교양일때
		else {
			goto_xy(8, 11 + (i - 1) * 2);
			printf("%-4d %-28s %-18s %-12s", value, str[choice].className, str[choice].field, str[choice].building);
		}
		//%-ns는 왼쪽 정렬된 n칸 출력하는 문자열이다.
	}
}

//다익스트라가 사용되지 않은 기본 건물 목록 출력
void printBuild(void)
{
	for (int j = 0; j < BUILDNUM; j++) {
		if (strcmp(Node[j].Build, user[check].u.building) == 0) {
			SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), DarkGreen); //전공건물일 때 초록색으로 표시
			goto_xy(7, 6 + (j + 1) * 2);
			printf(" ▶ %s", Node[j].Build);
			SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), WHITE);
		}
		else {
			goto_xy(7, 6 + (j + 1) * 2);
			printf(" ▶ %s", Node[j].Build);
		}
	}
}

//가장 작은 가중치 판별 함수
int getSmallIndex(void) {
	int min = INF; //가중치를 비교할 변수 선언, 가장 작은 값을 구할 것이기에 무한대로 초기화 
	int index = 0; //인덱스는 0부터 시작
	for (int i = 0; i < BUILDNUM; i++) { //i가 0부터 빌딜 갯수(14)까지 반복
		if (d[i] < min && v[i] != 1) { //거리가 min보다 작고, 확정된 노드가 아닐때
			min = d[i]; //min에 i번째 거리 값을 집어넣고
			index = i; //index 값에 i 값을 집어넣는다
		}
	}

	return index; //index를 반환해준다.
}

//다익스트라 거리 계산
void daikstra(int start) {
	for (int i = 0; i < BUILDNUM; i++) {
		d[i] = buildArray[start][i]; //다익스트라 2차원 배열의 원소를 거리 값으로 채워준다.
		v[i] = 0; //확정된 노드는 없다.
	}
	v[start] = 1; //v[0] = 1로 방문하였다는 표시를 해준다.
	for (int i = 0; i < BUILDNUM - 2; i++) { //이중 for문을 사용할 예정이기에 최종 건물 갯수에서 -2를 해준다. (총 12번 반복)
		int index = getSmallIndex(); //가장 작은 인덱스를 반환해주는 함수를 호출하고 인덱스를 변수에 저장한다.
		v[index] = 1; //v[인덱스]는 방문한 노드이다.
		for (int j = 0; j < BUILDNUM; j++) { //j는 0부터 최종 건물 갯수까지 반복한다(0 ~ 13까지 반복)
			if (v[j] != 1) { //v[j]가 방문한 노드가 아니라면
				if (d[index] + buildArray[index][j] < d[j]) { //거리[인덱스] + 2차원 배열[인덱스][j] < 거리[j]에 해당한다면
					d[j] = d[index] + buildArray[index][j];
					buildArray[start][j] = d[index] + buildArray[index][j];
				}
			}
		}
	}
}

//다익스트라 거리 계산 후 기준을 통해 출력하기 (함수)
void Ques(char name[]) {
	int i, j;
	int index;
	int stmp;

	for (i = 0; i < BUILDNUM; i++) {
		if (strcmp(Node[i].Build, name) == 0) { //건물 이름 중 (전공건물/복수전공건물)이 있다면 
			index = i;                     //인덱스에 i를 집어넣는다.
		}
	}

	for (i = 0; i < BUILDNUM; i++) {       //건물의 갯수(14)만큼 반복
		N[i].dis = buildArray[index][i];   //i번째 거리는 2차원 배열의[index][i]번째에 있는 정수 값이다
		N[i].count = i;                    //현재 순서는 배열에서 저장된 i 순서를 따른다
	}

	for (j = 0; j < BUILDNUM; j++) {                //건물의 갯수(14)만큼 반복
		for (int k = j + 1; k < BUILDNUM; k++) {    //j+1부터 건물의 갯수만큼 반복 (최대 13번, j가 커질수록 반복하는 횟수가 줄어든다
			if (N[k].dis < N[j].dis) {              //나열할 거리 두개를 비교하여 인덱스 k 거리보다 인덱스 j 거리가 더 짧다면
				stmp = N[j].dis;                    //k의 거리와 j의 거리를 바꾸어주고
				N[j].dis = N[k].dis;
				N[k].dis = stmp;

				stmp = N[j].count;                  //count 또한 서로 바꾸어준다.
				N[j].count = N[k].count;
				N[k].count = stmp;
			}                                       //최종적으로 거리가 짧은 순으로 count가 정렬된다
		}
		final[j] = N[j].count;                      //count를 final(최종 정렬본) 변수에 저장한다
	}
}
//원래 아래랑 합쳐있는 거였는데 출력이 따로 필요해서 분리함!

//다익스트라 거리 기준을 통해 출력하기
void printQues(void)
{
	for (int j = 0; j < BUILDNUM; j++) {
		if (strcmp(Node[final[j]].Build, user[check].u.building) == 0) {
			SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), DarkGreen);
			goto_xy(7, 6 + (j + 1) * 2);
			printf("%2d. %s", j + 1, Node[final[j]].Build);
			SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), WHITE);
		}
		else if (strcmp(Node[final[j]].Build, user[check].u.singularBuild) == 0) {
			SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), DarkYellow);
			goto_xy(7, 6 + (j + 1) * 2);
			printf("%2d. %s", j + 1, Node[final[j]].Build);
			SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), WHITE);
		}
		else {
			goto_xy(7, 6 + (j + 1) * 2);
			printf("%2d. %s", j + 1, Node[final[j]].Build);
		}
	}
}

//건물 목록 출력 후 기준을 통해 출력하기 (본체)
void print_Daikstra(int num)
{
	int i;

	for (i = 0; i < BUILDNUM; i++) {
		daikstra(i);                    //0부터 13까지의 2차원 다익스트라 배열을 정리해준다
	}

	if (num == 1) {
		Ques(user[check].u.building);  //배열을 전공 건물에 가까운 순으로 정렬해주고
		printQues();                   //정렬된 값을 통해 출력해준다
	}
	else if (num == 2) {
		Ques(user[check].u.singularBuild);  //배열을 복수 전공 건물에 가까운 순으로 정렬해주고
		printQues();                        //정렬된 값을 통해 출력해준다   
	}
}

//**********************************************************************************************

// UI 그리기 함수들 

//시작 화면 - 로그인 & 회원가입
void start_ui(void)
{
	Sleep(600);
	system("cls");
	square(79, 52, 0, 0);
	square(79, 2, 0, 53);

	goto_xy(14, 4); // SWmap 로고 출력 print문
	printf("%2s■■%1s\n", " ", " ");
	goto_xy(14, 5);
	printf("■%4s■%1s■%2s■%2s■\t%2s■%2s■%2s%3s■■%2s%1s■■■%2s\n", " ", " ", " ", " ", " ", " ", " ", " ", " ", " ", " ", " ", " ");
	goto_xy(14, 6);
	printf("■%7s■%2s■%2s■\t■%2s■%2s■%1s■%4s■%1s■%4s■\n", " ", " ", " ", " ", " ", " ", " ", " ", " ", " ", " ");
	goto_xy(14, 7);
	printf("%2s■■%3s■%2s■%2s■\t■%2s■%2s■%1s■%4s■%1s■%4s■\n", " ", " ", " ", " ", " ", " ", " ", " ", " ", " ", " ");
	goto_xy(14, 8);
	printf("%6s■%1s■%2s■%2s■\t■%2s■%2s■%1s■■■■%1s■■■%2s\n", " ", " ", " ", " ", " ", " ", " ", " ", " ", " ", " ", " ", " ");
	goto_xy(14, 9);
	printf("■%4s■%1s■%2s■%2s■\t■%2s■%2s■%1s■%4s■%1s■%8s\n", " ", " ", " ", " ", " ", " ", " ", " ", " ", " ", " ");
	goto_xy(14, 10);
	printf("%2s■■%4s%1s■%2s■%2s\t■%2s■%2s■%1s■%4s■%1s■%8s\n", " ", " ", " ", " ", " ", " ", " ", " ", " ", " ", " ", " ");

	goto_xy(31, 14);
	printf("|  교 양  추 천  |");

	square_char(40, 6, 20, 20, "1. 로 그 인");
	square_char(40, 6, 20, 35, "2. 회 원 가 입");
	goto_xy(2, 54);
	printf("번호 입력 >> ");
}

//로그인 ui
void login_ui(void)
{
	Sleep(600);
	system("cls");
	square(79, 52, 0, 0);
	goto_xy(2, 54);
	square_char(41, 2, 19, 2, "로 그 인");

	goto_xy(36, 14);
	printf("아 이 디");
	square(59, 4, 10, 16);

	goto_xy(35, 27);
	printf("비 밀 번 호");
	square(59, 4, 10, 29);

	square_char(41, 4, 19, 44, "@ 입력 시 시작화면으로 돌아갑니다.");
}

//회원가입 ui
void join_ui(void)
{
	Sleep(600);
	system("cls");
	square(79, 52, 0, 0);
	goto_xy(2, 54);
	square_char(41, 2, 19, 2, "회 원 가 입");

	goto_xy(36, 14);
	printf("아 이 디");
	square(59, 4, 10, 16);

	goto_xy(35, 27);
	printf("비 밀 번 호");
	square(59, 4, 10, 29);

	square_char(41, 4, 19, 44, "@ 입력 시 시작화면으로 돌아갑니다.");
}

//회원 정보 출력 박스
void user_print(void)
{
	square(59, 8, 10, 8);
	goto_xy(12, 10);
	printf(" %7s%-15s : %s", " ", "ID", user[check].u.id);
	goto_xy(12, 12);
	if (strcmp(user[check].u.building, "none") == 0) {
		printf(" %7s%s 이 등록되지 않았습니다.", " ", "전공 건물");
	}
	else {
		printf(" %7s%-15s : %s", " ", "전공 건물", user[check].u.building);
	}
	goto_xy(12, 14);
	if (strcmp(user[check].u.singularBuild, "none") == 0) {
		printf(" %7s%-15s이 등록되지 않았습니다.", " ", "복수 전공 건물");
	}
	else {
		printf(" %7s%-15s : %s", " ", "복수 전공 건물", user[check].u.singularBuild);
	}
}

//선택화면 1. 전공건물관리 / 2.교양목록 / 3. 북마크  *************** 이동경로 표시 X
void main_ui(void)
{
	if (strcmp(user[check].u.building, "none") == 0) { //전공 건물이 등록 되어 있는지 않는다면 바로 전공 등록 화면으로 이동
		BuildPrint_fcn(1);
	}

	Sleep(600);
	system("cls");
	square(79, 52, 0, 0);

	square_char(41, 2, 19, 2, "메 인 화 면");

	goto_xy(65, 1);
	printf("0 = 뒤로가기");

	square(79, 2, 0, 53);

	user_print();

	square_char(39, 6, 20, 21, "1. 나 의 전 공 건 물");
	square_char(39, 6, 20, 31, "2. 교 양 목 록");
	square_char(39, 6, 20, 41, "3. 북 마 크");

	goto_xy(2, 54);
	printf("번호 입력 >> ");
	goto_xy(15, 54);
	scanf("%s", answer);

	if (strcmp(answer, "1") == 0) {
		Build_fcn();
	}
	else if (strcmp(answer, "2") == 0) {
		if (strcmp(user[check].u.building, "none") == 0) {
			SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), RED);
			square(59, 8, 10, 8);
			goto_xy(25, 11);
			printf("전공 건물이 등록되지 않았습니다");
			goto_xy(27, 13);
			printf("건물을 먼저 등록하여주세요");
			SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), WHITE);

			Sleep(600);

			main_ui();
		}
		ClassSuggest_fcn();
	}
	else if (strcmp(answer, "3") == 0) {
		bookMark_fcn();
	}
	else if (strcmp(answer, "0") == 0) {
		start_fcn();
	}
	else {
		main_ui();
	}
}

//건물 선택 화면 1.전공건물 / 2.복수전공건물  ++이동경로 표시 완료
void Build_ui(void)
{
	Sleep(600);
	system("cls");
	square(79, 52, 0, 0);
	square(79, 2, 0, 53);

	goto_xy(65, 1);
	printf("0 = 뒤로가기");

	square_char(41, 2, 19, 2, "나 의 건 물 관 리");

	user_print();


	goto_xy(27, 50);
	printf("메인 화면 -> 나의 건물 관리");

	square_char(40, 6, 20, 20, "1. 나 의 전 공 건 물");
	square_char(40, 6, 20, 35, "2. 나 의 복 수 전 공 건 물");

	goto_xy(2, 54);
	printf("번호 입력 >> ");
}

//건물 목록 출력 화면 1.등록 / 2.수정(삭제 포함)     *************** 이동경로 표시 X
void BuildPrint_ui(int num)
{
	Sleep(600);
	system("cls");
	square(79, 52, 0, 0);
	square(79, 2, 0, 53);
	square_char(41, 2, 19, 2, "전 공 관 리");

	goto_xy(65, 1);
	printf("0 = 뒤로가기");

	square(71, 30, 4, 6); //건물 출력할 네모 화면 출력
	goto_xy(6, 37);
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), DarkGreen);
	printf("* 전공건물");
	goto_xy(18, 37);
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), DarkYellow);
	printf("* 복수전공건물");
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), WHITE);

	if (num == 1 && strcmp(user[check].u.building, "none") != 0) { //건물이 등록되어 있지 않으면 등록 불가(빨간색 변경)
		SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), RED);
		square_char(35, 6, 4, 41, "1. 건 물 등 록");
		SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), WHITE);

	}
	else if (num == 2 && strcmp(user[check].u.singularBuild, "none") != 0) { //건물이 등록되어 있지 않으면 등록 불가(빨간색 변경)
		SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), RED);
		square_char(35, 6, 4, 41, "1. 건 물 등 록");
		SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), WHITE);
	}
	else square_char(35, 6, 4, 41, "1. 건 물 등 록");
	square_char(35, 6, 40, 41, "2. 건 물 수 정");

	goto_xy(2, 54);
	printf("번호 입력 >> ");
}

//건물 등록 화면    *************** 이동경로 표시 X
void BuildSign_ui(void)
{
	Sleep(600);
	system("cls");
	square(79, 52, 0, 0);
	square_char(41, 2, 19, 2, "건 물 등 록");

	goto_xy(65, 1);
	printf("0 = 뒤로가기");

	square(71, 30, 4, 6); //건물 출력할 네모 화면 출력
	goto_xy(6, 37);
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), DarkGreen);
	printf("* 전공건물");
	goto_xy(18, 37);
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), DarkYellow);
	printf("* 복수전공건물");
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), WHITE);

	square(71, 2, 4, 44);
	goto_xy(6, 45);
	printf("등록할 건물 이름 입력 >> ");
}

//건물 수정 화면     *************** 이동경로 표시 X
void BuildModify_ui(int num)
{
	Sleep(600);
	system("cls");
	square(79, 52, 0, 0);

	goto_xy(65, 1);
	printf("0 = 뒤로가기");

	if (num == 1) {
		square_char(41, 2, 19, 2, "전 공 건 물 수 정");
	}
	else if (num == 2) {
		square_char(41, 2, 19, 2, "복 수 전 공 건 물 수 정");
	}

	square(71, 30, 4, 6); //건물 출력할 네모 화면 출력
	goto_xy(6, 37);
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), DarkGreen);
	printf("* 전공건물");
	goto_xy(18, 37);
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), DarkYellow);
	printf("* 복수전공건물");
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), WHITE);

	square(71, 2, 4, 40);
	goto_xy(6, 41);
	printf("재등록할 건물 이름 입력 >> ");

	if (num == 2) {
		square_char(71, 2, 4, 45, "@ 입력 시 복수 전공 건물 삭제");
	}
}

//교양 추천 화면 ++이동경로 표시 완료
void ClassSuggest_ui(void)
{
	Sleep(600);
	system("cls");
	square(79, 52, 0, 0);
	square(79, 2, 0, 53);

	goto_xy(65, 1);
	printf("0 = 뒤로가기");

	square_char(41, 2, 19, 2, "교 양 추 천");

	user_print();

	goto_xy(29, 50);
	printf("메인 화면 -> 교양 추천");

	square_char(40, 6, 20, 20, "1. 건 물 기 준 추 천");
	square_char(40, 6, 20, 35, "2. 분 야 기 준 추 천");

	goto_xy(2, 54);
	printf("번호 입력 >> ");
}

//교양 조회 건물 선택 화면 ++이동경로 표시 완료
void ClassBuild_ui(void)
{
	Sleep(600);
	system("cls");
	square(79, 52, 0, 0);
	square(79, 2, 0, 53);

	goto_xy(65, 1);
	printf("0 = 뒤로가기");

	square_char(41, 2, 19, 2, "건 물 관 리");

	user_print();

	goto_xy(20, 50);
	printf("메인 화면 -> 교양 추천 -> 건물 기준 추천");

	square_char(40, 6, 20, 20, "1. 전 공 건 물 기 준");
	square_char(40, 6, 20, 35, "2. 복 수 전 공 건 물 기 준");

	goto_xy(2, 54);
	printf("번호 입력 >> ");
}

//교양 조회 화면    *************** 이동경로 표시 X
void ClassResearch_ui(void)
{
	Sleep(600);
	system("cls");
	square(79, 52, 0, 0);
	square(79, 2, 0, 53);

	goto_xy(65, 1);
	printf("0 = 뒤로가기");

	square_char(71, 2, 4, 6, "NO.  교양 이름                   분야              건물          ");
	square(71, 30, 4, 9); //교양 분야 건물 출력할 네모 화면 출력

	goto_xy(6, 40);
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), DarkGreen);
	printf("* 전공건물");
	goto_xy(18, 40);
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), DarkYellow);
	printf("* 복수전공건물");
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), WHITE);

	square_char(71, 2, 4, 44, "해당하는 번호 입력 시 북마크에 추가됩니다.");

	square(71, 3, 4, 47);
	goto_xy(18, 48);
	printf("a 입력 시 이전 페이지, d 입력 시 다음 페이지");
	goto_xy(26, 49);
	printf("0 입력 시 이전 화면으로 이동");

	goto_xy(2, 54);
	printf("번호 입력 > ");
}

//분야 선택 화면 ++이동경로 표시 완료
void FieldChoice_ui(void)
{
	Sleep(600);
	system("cls");
	square(79, 52, 0, 0);

	square_char(41, 2, 19, 2, "분 야 선 택");
	square(79, 2, 0, 53);

	goto_xy(65, 1);
	printf("0 = 뒤로가기");
	user_print();

	square_char(41, 4, 19, 19, "1. 문학, 예술, 문화");
	square_char(41, 4, 19, 25, "2. 역사와 철학");
	square_char(41, 4, 19, 31, "3. 정치, 경제, 사회");
	square_char(41, 4, 19, 37, "4. 과학과 기술");
	square_char(41, 4, 19, 43, "5. 실용 실천");

	goto_xy(20, 50);
	printf("메인 화면 -> 교양 추천 -> 분야 기준 추천");

	goto_xy(2, 54);
	printf("번호 입력 >> ");
}

//북마크 조회 화면  *************** 이동경로 표시 X
void bookMark_ui(void)
{
	Sleep(600);
	system("cls");
	square(79, 52, 0, 0);
	square(79, 2, 0, 53);

	goto_xy(65, 1);
	printf("0 = 뒤로가기");

	square_char(41, 2, 19, 2, "북 마 크 목 록");

	square_char(71, 2, 4, 6, "NO.  교양 이름                   분야              건물          ");
	square(71, 30, 4, 9); //건물 출력할 네모 화면 출력

	goto_xy(6, 40);
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), DarkGreen);
	printf("* 전공건물");
	goto_xy(18, 40);
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), DarkYellow);
	printf("* 복수전공건물");
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), WHITE);

	square_char(71, 2, 4, 44, "해당하는 번호 입력 시 북마크에서 삭제됩니다.");

	square(71, 3, 4, 47);
	//goto_xy(18, 48);
	//printf("a 입력 시 이전 페이지, d 입력 시 다음 페이지");
	goto_xy(26, 49);
	printf("0 입력 시 이전 화면으로 이동");

	goto_xy(2, 54);
	printf("번호 입력 >> ");
}



void square(int width, int height, int x, int y)
{
	goto_xy(x, y);
	for (int i = 0; i < width; i++) {   //첫 가로 라인 그리기
		if (i == 0) printf("┏");
		else if (i == width - 1) printf("┓\n");
		else  printf("━");
	}
	goto_xy(x, y + 1);
	for (int j = 0; j < height; j++) {  //세로 라인 그리기
		printf("┃");
		for (int k = 0; k < width - 2; k++) {
			printf(" ");
		}
		printf("┃\n");
		goto_xy(x, y + (j + 1));
	}
	for (int m = 0; m < width; m++) {   //마지막 가로 라인 그리기
		if (m == 0) printf("┗");
		else if (m == width - 1) printf("┛\n");
		else printf("━");
	}
}

void square3(int width, int height, int x, int y)
{
	goto_xy(x, y);
	for (int i = 0; i < width; i++) {
		if (i == 0) printf("┣");
		else if (i == width - 1) printf("┨\n");
		else  printf("━");
	}
	goto_xy(x, y + 1);
	for (int j = 0; j < height; j++) {
		printf("┃");
		for (int k = 0; k < width - 2; k++) {
			printf(" ");
		}
		printf("┃\n");
		goto_xy(x, y + (j + 1));
	}
	for (int m = 0; m < width; m++) {
		if (m == 0) printf("┗");
		else if (m == width - 1) printf("┛\n");
		else printf("━");
	}
}

void square_char(int width, int height, int x, int y, char* msg)
{
	goto_xy(x, y);
	for (int i = 0; i < width; i++) {
		if (i == 0) printf("┏");
		else if (i == width - 1) printf("┓\n");
		else  printf("━");
	}
	goto_xy(x, y + 1);
	for (int j = 0; j < height; j++) {
		printf("┃");
		for (int k = 0; k < width - 2; k++) {
			if (j == height / 2 && k == ((width)-strlen(msg)) / 2) {
				goto_xy(x + ((width)-strlen(msg)) / 2 + 1, y + height / 2);
				printf("%s", msg);
				goto_xy(x + width - 1, y + j);
			}
			else if (j != height / 2) printf(" ");
		}

		printf("┃");
		goto_xy(x, y + (j + 1));
	}
	for (int m = 0; m < width; m++) {
		if (m == 0) printf("┗");
		else if (m == width - 1) printf("┛\n");
		else printf("━");
	}
}

void goto_xy(int x, int y)
{
	HANDLE handle = GetStdHandle(STD_OUTPUT_HANDLE);
	COORD pos;
	pos.X = x;
	pos.Y = y;
	SetConsoleCursorPosition(handle, pos);
}