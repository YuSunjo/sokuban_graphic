#include <stdio.h>
#include <stdlib.h>
#include <Windows.h>
#include <conio.h>

#define LEFT 75
#define RIGHT 77
#define UP 72
#define DOWN 80
#define MAXSTAGE 3
#define putchxy(x,y,c) {gotoxy(x,y);_putch(c);}

void DrawScreen();
BOOL TestEnd();
void Move(int dir);

char ns[18][21];
int nStage;
int nx, ny;
int nMove;


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

void gotoxy(int x, int y)
{
	COORD Pos = { x, y };
	SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), Pos);
}

void main()
{
	int ch;
	int x, y;

	ShowCursor(FALSE);
	nStage = 0;

	for (; 1;) {
		memcpy(ns, arStage[nStage], sizeof(ns));
		for (y = 0; y < 18; y++) {
			for (x = 0; x < 20; x++) {
				if (ns[y][x] == '@') {
					nx = x;
					ny = y;
					ns[y][x] = ' ';
				}
			}
		}
		system("cls");
		nMove = 0;

		for (; 2;) {
			DrawScreen();
			ch = _getch();
			if (ch == 0xE0 || ch == 0) {
				ch = _getch();
				switch (ch) {
				case LEFT:
				case RIGHT:
				case UP:
				case DOWN:
					Move(ch);
					break;
				}
			}
			else {
				ch = tolower(ch);
				if (ch == 'r') {
					break;
				}
				if (ch == 'n') {
					if (nStage < MAXSTAGE - 1) {
						nStage++;
					}
					break;
				}
				if (ch == 'p') {
					if (nStage > 0) {
						nStage--;
					}
					break;
				}
				if (ch == 'q') {
					ShowCursor(TRUE);
					exit(0);
				}
			}

			if (TestEnd()) {
				system("cls");
				gotoxy(10, 10);
				printf("%d 스테이지를 풀었습니다. 다음 스테이지로 이동합니다",
					nStage + 1);
				Sleep(2000);
				if (nStage < MAXSTAGE - 1) {
					nStage++;
				}
				break;
			}
		}
	}
}

void DrawScreen()
{
	int x, y;
	for (y = 0; y < 18; y++) {
		for (x = 0; x < 20; x++) {
			putchxy(x, y, ns[y][x]);
		}
	}
	putchxy(nx, ny, '@');

	gotoxy(40, 2); puts("SOKOBAN");
	gotoxy(40, 4); puts("Q:종료, R:다시 시작");
	gotoxy(40, 6); puts("N:다음, P:이전");
	gotoxy(40, 8); printf("스테이지 : %d", nStage + 1);
	gotoxy(40, 10); printf("이동 회수 : %d", nMove);
}

BOOL TestEnd()
{
	int x, y;

	for (y = 0; y < 18; y++) {
		for (x = 0; x < 20; x++) {
			if (arStage[nStage][y][x] == '.' && ns[y][x] != 'O') {
				return FALSE;
			}
		}
	}
	return TRUE;
}

void Move(int dir)
{
	int dx = 0, dy = 0;

	switch (dir) {
	case LEFT:
		dx = -1;
		break;
	case RIGHT:
		dx = 1;
		break;
	case UP:
		dy = -1;
		break;
	case DOWN:
		dy = 1;
		break;
	}

	if (ns[ny + dy][nx + dx] != '#') {
		if (ns[ny + dy][nx + dx] == 'O') {
			if (ns[ny + dy * 2][nx + dx * 2] == ' ' || ns[ny + dy * 2][nx + dx * 2] == '.') {
				if (arStage[nStage][ny + dy][nx + dx] == '.') {
					ns[ny + dy][nx + dx] = '.';
				}
				else {
					ns[ny + dy][nx + dx] = ' ';
				}
				ns[ny + dy * 2][nx + dx * 2] = 'O';
			}
			else {
				return;
			}
		}
		nx += dx;
		ny += dy;
		nMove++;
	}
}