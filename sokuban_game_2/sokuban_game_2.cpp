// sokuban_game_2.cpp : 애플리케이션에 대한 진입점을 정의합니다.
//

#include "framework.h"
#include "sokuban_game_2.h"
#include <Windows.h>    
#include "Resource.h"


#define MAXSTAGE 3 
#define BW 32 
#define BH 32
// 일단 스테이지는 3개 뿐이다. BW,BH는 비트맵의 가로와 세로 픽셀크기를 나타낸다. 
//나중에 비트맵 리소스작성에서도 32*32로 해야겠지? 

void DrawScreen(HDC hdc);
BOOL TestEnd();
void Move(int dir);
void InitStage();
void DrawBitmap(HDC hdc, int x, int y, HBITMAP hB﻿it);
// 게임 내에서 동작할 함수들. 동작에 관한 설명은 밑에서 따로 하겠음.

char ns[18][21];
int nStage;
int nx, ny;
int nMove;
HBITMAP hBit[5];
// 전역 변수 및 핸들. 순서대로 (해당 스테이지 배열, 스테이지넘버, 커서위치, 이동 수, 5개의 비트맵 핸들)

char arStage[MAXSTAGE][18][21] = {
{
"####################",
"####################",
"####################",
"#####   ############",
"#####O  ############",
"#####  O############",
"###  O O ###########",
"### # ## ###########",
"#   # ## #####  ..##",
"# O  O   @      ..##",
"##### ### # ##  ..##",
"#####     ##########",
"####################",
"####################",
"####################",
"####################",
"####################",
"####################"
},
{
"####################",
"####################",
"####################",
"####################",
"####..  #     ######",
"####..  # O  O  ####",
"####..  #O####  ####",
"####..    @ ##  ####",
"####..  # #  O #####",
"######### ##O O ####",
"###### O  O O O ####",
"######    #     ####",
"####################",
"####################",
"####################",
"####################",
"####################",
"####################"
},
{
"####################",
"####################",
"####################",
"####################",
"##########     @####",
"########## O#O #####",
"########## O  O#####",
"###########O O #####",
"########## O # #####",
"##....  ## O  O  ###",
"###...    O  O   ###",
"##....  ############",
"####################",
"####################",
"####################",
"####################",
"####################",
"####################"
},
};
//스테이지 구조다. 이렇게 구성하면 콘솔환경과 다를 바 없으나, 얘네는 위치정보일 뿐이고 비트맵으로 바꾸면 봐 줄만하다.

LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
HINSTANCE g_hInst;
HWND hWndMain;
LPCTSTR lpszClass = TEXT("Sokoban2");

int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance
	, LPSTR lpszCmdParam, int nCmdShow)
{
	HWND hWnd;
	MSG Message;
	WNDCLASS WndClass;
	g_hInst = hInstance;
	WndClass.cbClsExtra = 0;
	WndClass.cbWndExtra = 0;
	WndClass.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	WndClass.hCursor = LoadCursor(NULL, IDC_ARROW);
	WndClass.hIcon = LoadIcon(NULL, IDI_APPLICATION);
	WndClass.hInstance = hInstance;
	WndClass.lpfnWndProc = WndProc;
	WndClass.lpszClassName = lpszClass;
	WndClass.lpszMenuName = NULL;
	WndClass.style = CS_HREDRAW | CS_VREDRAW;
	RegisterClass(&WndClass);

	hWnd = CreateWindow(lpszClass, lpszClass, WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX,
		CW_USEDEFAULT, CW_USEDEFAULT, 0, 0, NULL, (HMENU)NULL, hInstance, NULL);
	ShowWindow(hWnd, nCmdShow);
	while (GetMessage(&Message, NULL, 0, 0)) {
		TranslateMessage(&Message);
		DispatchMessage(&Message);
	}
	return (int)Message.wParam;
}
//부모 윈도생성코드. 특별날 것이 없음...

LRESULT CALLBACK WndProc(HWND hWnd, UINT iMessage, WPARAM wParam, LPARAM lParam)
{
	HDC hdc; //DC핸들
	PAINTSTRUCT ps; //페인트 구조체 변수. 그려야 하기 때문에 반드시 필요하겠지?
	RECT crt; //RECT 구조체 변수
	int i;
	TCHAR Message[256];
	switch (iMessage) {
	case WM_CREATE:
		hWndMain = hWnd; //부모윈도에서 작업할거임
		SetRect(&crt, 0, 0, 900, BH * 18);//작업영역의 크기조정. 좌상단은 0,0이고 우하단은 900,34*18로 할껴~
		//왜 BH*18이냐믄 비트맵(34,34)짜리를 18개 딱 채워 넣을거거든... 물론 곱해서 직접 숫자를 넣어도 되겠지만 이쪽이 더 논리적이지?
		AdjustWindowRect(&crt, WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX, FALSE);
		//이 함수는 crt구조체를 위에 설정한 작업영역이 나올 수 있도록(그러니까 잡다한 메뉴를 빼고 크기를 계산해주는 거지~)
		//크기와 스타일을 조정하고 다시 크기를 구조체에 반환시켜주는거지. 뒤에 옵션은 그냥 기본적인거 메뉴랑 최소화버튼인 듯~
		SetWindowPos(hWnd, NULL, 0, 0, crt.right - crt.left, crt.bottom - crt.top,
			SWP_NOMOVE | SWP_NOZORDER);
		//윈도위치조정이겠지? 아까 위의 작업으로 얻은 crt구조체 정보로 쓱싹쓱싹~하는 거임... 지금 이 프로그래밍에서 큰 의미는 없음~ㅎㅎ
		for (i = 0; i < 5; i++) {
			hBit[i] = LoadBitmap(g_hInst, MAKEINTRESOURCE(IDB_WALL + i));


		}
		hBit[0] = LoadBitmap(g_hInst, MAKEINTRESOURCE(IDB_WALL));
		hBit[1] = LoadBitmap(g_hInst, MAKEINTRESOURCE(IDB_BOX));
		hBit[2] = LoadBitmap(g_hInst, MAKEINTRESOURCE(IDB_COMPLETE));
		hBit[3] = LoadBitmap(g_hInst, MAKEINTRESOURCE(IDB_BLANK));
		hBit[4] = LoadBitmap(g_hInst, MAKEINTRESOURCE(IDB_MAN));
		
		//비트맵을 리소스에서 읽어오는 거야 차례로~
		nStage = 0;
		InitStage();//처음 만들 때 당연히 스테이지는 1번(여기서는 0으로 표현..)이구 스테이지 초기화 함수 발동~!!
		return 0;
	case WM_PAINT:
		hdc = BeginPaint(hWnd, &ps);
		DrawScreen(hdc);
		EndPaint(hWnd, &ps);//dc핸들 얻고 이 핸들로 DrawScreen함수를 쓰고 다시 반납~ 간단하네 ㅋㅋ
		return 0;
	case WM_KEYDOWN:
		switch (wParam) {
		case VK_LEFT:
		case VK_RIGHT:
		case VK_UP:
		case VK_DOWN:
			Move(wParam);//방향키 입력이 들어오면 무조건 move함수로 처리한다.
			if (TestEnd()) {
				wsprintf(Message, TEXT("%d 스테이지를 풀었습니다. ")
					TEXT("다음 스테이지로 이동합니다"), nStage + 1);
				MessageBox(hWnd, Message, TEXT("알림"), MB_OK);
				if (nStage < MAXSTAGE - 1) {
					nStage++;
				}
				InitStage();
			}
			break;//이 부분이 조금 재미있기도 하고 비효율적이기도 한데... 방향키 입력을 할 때마다 TestEnd()함수로 클리어 조건을 확인한다.
			  //암튼 초보니까 그러려니하고 넘어간다. 스테이지 클리어조건이 만족하면 메세지박스 발동시키고 스테이지+1, 초기화시킴 ㅇㅋ?
		case 'Q':
			DestroyWindow(hWnd);
			break;
		case 'R':
			InitStage();
			break;
		case 'N':
			if (nStage < MAXSTAGE - 1) {
				nStage++;
				InitStage();
			}
			break;
		case 'P':
			if (nStage > 0) {
				nStage--;
				InitStage();
			}
			break;
		}
		return 0;
		//단축키설정이다. Q는 종료, R은 스테이지 초기화,N은 다음스테이지(막판이라면 발동하지 않겠지?),P는 이전스테이지
	case WM_DESTROY:
		for (i = 0; i < 5; i++) {
			DeleteObject(hBit[i]);
		}
		PostQuitMessage(0);
		return 0;//윈도를 파괴할땐 메모리에 있는 비트맵5개도 해제해야겠지?
	}
	return(DefWindowProc(hWnd, iMessage, wParam, lParam));
}

void DrawScreen(HDC hdc)//스크린을 그리는 함수가 되시겠다.
{
	int x, y;
	int iBit;
	TCHAR Message[256];

	for (y = 0; y < 18; y++) {
		for (x = 0; x < 20; x++) {
			switch (ns[y][x]) {
			case '#':
				iBit = 0;
				break;
			case 'O':
				iBit = 1;
				break;
			case '.':
				iBit = 2;
				break;
			case ' ':
				iBit = 3;
				break;
			}
			DrawBitmap(hdc, x * BW, y * BH, hBit[iBit]);//스테이지 전체 배열을 돌아가며 허접때기 문자를 비트맵으로 치환하는 작업이다.
		}
	}
	DrawBitmap(hdc, nx * BW, ny * BH, hBit[4]);//마지막으로 캐릭터(hBit[4])를 그린다. nx*BW,ny*BH요게 캐릭터 위치가 되시겠다.

	wsprintf(Message, TEXT("SOKOBAN"));
	TextOut(hdc, 700, 10, Message, lstrlen(Message));
	wsprintf(Message, TEXT("Q:종료, R:다시 시작"));
	TextOut(hdc, 700, 30, Message, lstrlen(Message));
	wsprintf(Message, TEXT("N:다음, P:이전"));
	TextOut(hdc, 700, 50, Message, lstrlen(Message));
	wsprintf(Message, TEXT("스테이지 : %d"), nStage + 1);
	TextOut(hdc, 700, 70, Message, lstrlen(Message));
	wsprintf(Message, TEXT("이동 회수 : %d"), nMove);
	TextOut(hdc, 700, 90, Message, lstrlen(Message)); //잡다한 정보를 나타낸다. Message문자열 하나로 때운다. 조금 어렵군...
}

BOOL TestEnd()//클리어 조건을 참,거짓으로 반환한다.
{
	int x, y;

	for (y = 0; y < 18; y++) {
		for (x = 0; x < 20; x++) {
			if (arStage[nStage][y][x] == '.' && ns[y][x] != 'O') {
				return FALSE;//이게 아주 중요한 부분이 되겠다. 원본스테이지arStage[nStage][y][x]=='.'창고 부분과 ns[y][x]!='O' 우리가 조작한 스테이지의 공이 하나라도 일치하지 않는다면 FALSE반환
			}
		}
	}
	return TRUE;//만족하면 참을 반환한다. 단순하지?
}

void Move(int dir)//키입력이 들어오면 처리하는 함수가 되겠다.
{
	int dx = 0, dy = 0;//dx와dy는 매 입력마다 초기화 시킨다.

	switch (dir) {
	case VK_LEFT:
		dx = -1;
		break;
	case VK_RIGHT:
		dx = 1;
		break;
	case VK_UP:
		dy = -1;
		break;
	case VK_DOWN:
		dy = 1;
		break;
	}

	if (ns[ny + dy][nx + dx] != '#') {//이동예상위치가 벽이 아니고~
		if (ns[ny + dy][nx + dx] == 'O') {//이동예상위치가 공이고~
			if (ns[ny + dy * 2][nx + dx * 2] == ' ' || ns[ny + dy * 2][nx + dx * 2] == '.') {//이동예상위치의 한칸앞이 텅빈 곳이거나, 창고며~
				if (arStage[nStage][ny + dy][nx + dx] == '.') {//이동예상위치가 원본스테이지의 창고이면~
					ns[ny + dy][nx + dx] = '.';//게임 내 예상위치는 창고가 되시겠다.
				}
				else {
					ns[ny + dy][nx + dx] = ' ';//원본스테이지의 창고가 아니라면 게임 내 빈공간이 되시겠다.
				}
				ns[ny + dy * 2][nx + dx * 2] = 'O';//이동예상위치의 한칸앞이 텅빈 곳이거나, 창고면 게임내 공의 위치가 된다.
			}
			else {
				return;//나머지는 전부 무시하면 되겠다.
			}
		}
		//드럽게 복잡하고 짜증나는 코드다.[ny+dy][nx+dx]는 현재캐릭터 위치에 방향키를 입력받고 난 후의 이동예상 지점을 나타낸다.
		nx += dx;
		ny += dy;//캐릭터 현 위치를 갱신시킨다.
		nMove++;//이동 회수를 증가시킴
		InvalidateRect(hWndMain, NULL, FALSE);//이동(이 되었을 때!)한 경우는 무조건 WM_PAINT메세지로 스테이지를 전부 다시 그린다.
		//다만 우리눈엔 캐릭터만 이동한 걸로 보이겠지 ㅋㅋ 비효율적이라고 생각한다.
	}
}

void InitStage()//스테이지 초기화 함수되시겠다.
{
	int x, y;

	memcpy(ns, arStage[nStage], sizeof(ns));//원본스테이지를 우리가 쓸 ns배열로 메모리복사한다.
	for (y = 0; y < 18; y++) {
		for (x = 0; x < 20; x++) {
			if (ns[y][x] == '@') {
				nx = x;
				ny = y;//캐릭터 현재위치정보를 nx,ny에 대입한다.
				ns[y][x] = ' ';//일단 캐릭터 위치 nx,ny는 DrawScreen으로 보내고 여기서는 그냥 빈공간으로 바꾸어 버린다. 
				//그러니까 캐릭터 초기 위치만 보내주고 나머지는 스테이지로 바꾸어 버린다. ㄷㄷㄷ
			}
		}
	}
	nMove = 0;
	InvalidateRect(hWndMain, NULL, TRUE);//당연히 무효화시켜서 다시 그린다.
}

void DrawBitmap(HDC hdc, int x, int y, HBITMAP hBit)//요건 비트맵치환 함수
{
	HDC MemDC;//메모리DC
	HBITMAP OldBitmap;//복구용 비트맵핸들
	int bx, by;
	BITMAP bit;

	MemDC = CreateCompatibleDC(hdc);//hdc와 같은 메모리 dc를 만들어야 비트맵을 표현하겠지?
	OldBitmap = (HBITMAP)SelectObject(MemDC, hBit);

	GetObject(hBit, sizeof(BITMAP), &bit);
	bx = bit.bmWidth;
	by = bit.bmHeight;

	BitBlt(hdc, x, y, bx, by, MemDC, 0, 0, SRCCOPY);

	SelectObject(MemDC, OldBitmap);
	DeleteDC(MemDC);
}
