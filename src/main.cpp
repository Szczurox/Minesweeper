#include<iostream>
#include<Windows.h>
#include<conio.h>
#include<stdio.h>
#include<stdlib.h>
#include<time.h>
#include<vector>
#include<cstdlib>

typedef std::vector<std::vector<int>> matrix;
typedef std::vector<std::vector<char>> matrixChar;

bool checkBoard(matrix board, matrixChar visibleBoard) {
	int boardWidth = board.size();
	int boardHeight = board[0].size();
	for (int i = 0; i < boardWidth; i++) {
		for (int j = 0; j < boardHeight; j++) {
			if (board[i][j] == 9 && visibleBoard[i][j] != 'P') return false;
		}
	}
	return true;
}

// Draws the board
void drawBoard(HANDLE hConsole, matrixChar board, int x, int y, int minesLeft) {
	int boardWidth = board.size();
	int boardHeight = board[0].size();
	SetConsoleTextAttribute(hConsole, 11);
	for (int i = 0; i <= boardWidth - 4; i++)
		std::cout << " ";
	std::cout << "MINESWEEPER\n\n";

	std::cout << "    Mines Left: " << minesLeft << "\n";
	for (int i = 0; i < boardHeight; i++) {
		std::cout << "\n    ";
		for (int j = 0; j < boardWidth; j++) {
			int fg = 0x0008;
			int bg = 0x0000;

			switch (board[j][i]) {
				case '1':
					fg = 0x0001;
					break;
				case '2':
					fg = 0x0002;
					break;
				case '3':
					fg = 0x0006;
					break;
				case '4':
					fg = 0x0005;
					break;
				case '5':
					fg = 0x0009;
					break;
				case '6':
					fg = 0x0003;
					break;
				case '7':
					fg = 0x0016;
					break;
				case '8':
					fg = 0x0018;
					break;
			}

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

void drawMenu(HANDLE hConsole, char option) {
	int chosenBg = 0x0060 | 0x0080;
	int colEasy = 0x0002;
	int colMedium = 0x0006;
	int colHard = 0x0004;
	int colCustom = 0x0005;

	switch (option) {
	case 0:
		colEasy = 0x0002 | chosenBg;
		break;
	case 1:
		colMedium = 0x0006 | chosenBg;
		break;
	case 2:
		colHard = 0x0004 | chosenBg;
		break;
	default:
		colCustom = 0x0005 | chosenBg;
		break;
	}

	SetConsoleTextAttribute(hConsole, 11);
	std::cout << "        MINESWEEPER\n\n";
	SetConsoleTextAttribute(hConsole, 15);
	std::cout << "           ";
	SetConsoleTextAttribute(hConsole, colEasy);
	std::cout << "EASY\n";
	SetConsoleTextAttribute(hConsole, 15);
	std::cout << "          ";
	SetConsoleTextAttribute(hConsole, colMedium);
	std::cout << "MEDIUM\n";
	SetConsoleTextAttribute(hConsole, 15);
	std::cout << "           ";
	SetConsoleTextAttribute(hConsole, colHard);
	std::cout << "HARD\n";
	SetConsoleTextAttribute(hConsole, 15);
	std::cout << "          ";
	SetConsoleTextAttribute(hConsole, colCustom);
	std::cout << "CUSTOM\n";
	SetConsoleTextAttribute(hConsole, 15);
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
bool moveHandle(char ch, int* x, int* y, matrix& board, matrixChar& visibleBoard, int* minesLeft) {
	int boardWidth = board.size();
	int boardHeight = board[0].size();
	// Movement
	if (ch == 'w' || ch == 'W')  // Up
		*y -= 1;
	else if (ch == 's' || ch == 'S')  // Down
		*y += 1;
	if (ch == 'a' || ch == 'A')  // Left
		*x -= 1;
	else if (ch == 'd' || ch == 'D')  // Right
		*x += 1;
	// Movement cap (prevents player from escaping the board)
	if (*x >= boardWidth) *x = 0;
	else if (*x < 0) *x = 9;
	if (*y >= boardHeight) *y = 0;
	else if (*y < 0) *y = 9;

	// Other events
	if (ch == 'f' || ch == 'F' || ch == 'p' || ch == 'P') {  // Flag
		if (visibleBoard[*x][*y] == '#') {
			*minesLeft -= 1;
			visibleBoard[*x][*y] = 'P';
		}
		else if (visibleBoard[*x][*y] == 'P') {
			*minesLeft += 1;
			visibleBoard[*x][*y] = '#';
		}
	}
	else if (ch == '\r' || ch == ' ')  // Uncover
		return breakTile(*x, *y, board, visibleBoard);

	return false;
}

int main() {
	int boardWidth = 10, boardHeight = 10;
	int numOfMines = 10;
	int x = 0, y = 0;
	bool isRunning = true, isWon = false;
	HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);

	char choice = -1;
	char option = 0;

	drawMenu(hConsole, option);

	while (choice == -1) {
		Sleep(5);
		if (_kbhit()) {
			char ch = _getch();

			// Esc
			if (ch == 27) return 1;

			system("cls");

			if (ch == 'w' || ch == 'W')  // Up
				option--;
			else if (ch == 's' || ch == 'S')  // Down
				option++;

			// Menu cap (prevents player from choosing mode that doesn't exist)
			if (option < 0) option = 0;
			if (option > 3) option = 3;

			if (ch == '\r' || ch == ' ')
				choice = option;

			drawMenu(hConsole, option);
		}
	}

	system("cls");

	switch (choice) {
		case 0:   // Easy
			// Defaults of the variables are already set to easy
			break;
		case 1:   // Medium
			boardWidth = 20;
			boardHeight = 20;
			numOfMines = 30;
			break;
		case 2:   // Hard
			boardWidth = 25;
			boardHeight = 25;
			numOfMines = 100;
			break;
		default:  // Custom
			bool pass = false;
			while (!pass) {
				SetConsoleTextAttribute(hConsole, 0x0007 | 0x0000);
				std::cout << "board width: ";
				std::cin >> boardWidth;
				system("cls");
				std::cout << "board height: ";
				std::cin >> boardHeight;
				system("cls");
				std::cout << "number of mines: ";
				std::cin >> numOfMines;
				system("cls");
				if (numOfMines > (boardHeight * boardWidth)) {
					SetConsoleTextAttribute(hConsole, 4);
					std::cout << "You can't have more mines than spaces on the board.\n";
					SetConsoleTextAttribute(hConsole, 7);
					system("pause");
				}
				else if (boardHeight < 2 || boardWidth < 2) {
					SetConsoleTextAttribute(hConsole, 4);
					std::cout << "Minium board size is 2x2.\n";
					SetConsoleTextAttribute(hConsole, 7);
					system("pause");
				}
				else pass = true;
				system("cls");
			}
			break;
	}

	matrix board(boardWidth, std::vector<int>(boardHeight, 0));
	matrixChar visibleBoard(boardWidth, std::vector<char>(boardHeight, '#'));

	srand((unsigned int)time(NULL));

	for (int i = 0; i < numOfMines; i++) {
		int ranX = rand() % (boardWidth - 1);
		int ranY = rand() % (boardHeight - 1);
		if (board[ranX][ranY] != 9) board[ranX][ranY] = 9;
		else {
			i--;
			continue;
		};
		for (int i = -1; i < 2; i++)
			for (int j = -1; j < 2; j++)
				if (ranX + i >= 0 && ranX + i < boardWidth)
					if (ranY + j >= 0 && ranY + j < boardHeight)
						if (board[ranX + i][ranY + j] != 9)
							board[ranX + i][ranY + j]++;
	}

	drawBoard(hConsole, visibleBoard, x, y, numOfMines);

	while (isRunning) {
		if (_kbhit()) {
			char ch = _getch();
			// Esc
			if (ch == 27) return 1;
			system("cls");
			if (moveHandle(ch, &x, &y, board, visibleBoard, &numOfMines))
				isRunning = false;
			if (numOfMines == 0) 
				if (checkBoard(board, visibleBoard)) {
					isRunning = false;
					isWon = true;
				}

			drawBoard(hConsole, visibleBoard, x, y,numOfMines);
		}
	}

	if (isWon) {
		SetConsoleTextAttribute(hConsole, 2);
		for (int i = 0; i <= boardWidth - 2; i++)
			std::cout << " ";
		std::cout << "You Won!\n";
	}
	else {
		SetConsoleTextAttribute(hConsole, 12);
		for (int i = 0; i <= boardWidth - 2; i++)
			std::cout << " ";
		std::cout << "You Lost!\n";
	}

	Sleep(2000);
	SetConsoleTextAttribute(hConsole, 7);
}