#include<iostream>
#include<Windows.h>
#include<conio.h>
#include<stdio.h>
#include<stdlib.h>
#include<time.h>
#include<vector>

typedef std::vector<std::vector<int>> matrix;
typedef std::vector<std::vector<char>> matrixChar;

// Draws the board
void drawBoard(HANDLE hConsole, matrixChar board, int x, int y) {
	for (int i = 0; i < board.size(); i++) {
		std::cout << "\n    ";
		for (int j = 0; j < board[0].size(); j++) {
			int fg = 0x0008;
			int bg = 0x0000;
			if (board[j][i] == '1')
				fg = 0x0001;
			else if (board[j][i] == '2')
				fg = 0x0002;
			else if (board[j][i] == '3')
				fg = 0x0006;
			else if (board[j][i] == '4')
				fg = 0x0005;
			else if (board[j][i] == '5')
				fg = 0x0009;
			else if (board[j][i] == '6')
				fg = 0x0003;
			else if (board[j][i] == '7')
				fg = 0x0016;
			else if (board[j][i] == '8')
				fg = 0x0018;
			if (board[j][i] == 'P') {
				fg = 0x0004;
			}
			if (board[j][i] == '@') {
				bg = 0x0040;
				fg = 0x0004;
			}
			else if (j == x && i == y)
				bg = 0x0060 | 0x0080;
			SetConsoleTextAttribute(hConsole, fg | bg);
			std::cout << board[j][i];
			SetConsoleTextAttribute(hConsole, 8);
			std::cout << ' ';
		}
	}
	std::cout << "\n\n";
}

bool breakTile(int x, int y, matrix& board, matrixChar& visibleBoard) {
	int boardWidth = board.size();
	int boardHeight = board[0].size();
	if (board[x][y] == 9) {
		visibleBoard[x][y] = '@';
		return true;
	}
	else if (board[x][y] == 0) {
		visibleBoard[x][y] = '_';
		for (int i = -1; i < 2; i++)
			for (int j = -1; j < 2; j++)
				if (x + i >= 0 && x + i < boardWidth)
					if (y + j >= 0 && y + j < boardHeight)
						if(visibleBoard[x + i][y + j] != '_' && visibleBoard[x + i][y + j] != 'P')
							breakTile(x + i, y + j, board, visibleBoard);
	}
	else
		visibleBoard[x][y] = board[x][y] + 48;
	return false;
}

// Keyboard handling
bool moveHandle(char ch, int* x, int* y, matrix& board, matrixChar& visibleBoard) {
	// Movement
	if (ch == 'w' || ch == 'W') {  // Up
		*y -= 1;
	}
	else if (ch == 's' || ch == 'S') {  // Down
		*y += 1;
	}
	if (ch == 'a' || ch == 'A') {  // Left
		*x -= 1;
	}
	else if (ch == 'd' || ch == 'D') {  // Right
		*x += 1;
	}
	// Movement cap (prevents player from escaping the board)
	if (*x >= 10) *x = 0;
	else if (*x < 0) *x = 9;
	if (*y >= 10) *y = 0;
	else if (*y < 0) *y = 9;

	// Other events
	if (ch == 'f' || ch == 'G') {  // Flag
		if (visibleBoard[*x][*y] == '#') visibleBoard[*x][*y] = 'P';
		else if (visibleBoard[*x][*y] == 'P') visibleBoard[*x][*y] = '#';
	}
	else if (ch == ' ') {  // Uncover
		return breakTile(*x, *y, board, visibleBoard);
	}
	return false;
}

int main() {
	const unsigned int boardWidth = 10, boardHeight = 10;
	int numOfMines = 10;
	int x = 0, y = 0;
	matrix board(boardWidth, std::vector<int>(boardHeight, 0));
	matrixChar visibleBoard(boardWidth, std::vector<char>(boardHeight, '#'));
	bool isRunning = true;
	HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);

	srand((unsigned int)time(NULL));

	for (int i = 0; i <= numOfMines; i++) {
		int ranX = rand() % 10;
		int ranY = rand() % 10;
		if (board[ranX][ranY] != 9) board[ranX][ranY] = 9;
		else {
			i--;
			continue;
		};
		for (int i = -1; i < 2; i++)
			for (int j = -1; j < 2; j++)
				if (ranX + i >= 0 && ranX + i <= boardWidth - 1)
					if (ranY + j >= 0 && ranY + j <= boardHeight - 1)
						if (board[ranX + i][ranY + j] != 9)
							board[ranX + i][ranY + j]++;
	}

	while (isRunning) {
		Sleep(5);
		if (_kbhit()) {
			char ch = _getch();
			system("cls");
			if (moveHandle(ch, &x, &y, board, visibleBoard)) {
				isRunning = false;
			}
			drawBoard(hConsole, visibleBoard, x, y);
		}
	}
	SetConsoleTextAttribute(hConsole, 12);
	std::cout << "       You Lost!\n";
	Sleep(1000);
	SetConsoleTextAttribute(hConsole, 7);
}