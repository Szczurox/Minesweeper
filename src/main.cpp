#include<iostream>
#include<conio.h>
#include<stdio.h>
#include<stdlib.h>
#include<time.h>
#include<vector>
#include<cstdlib>

typedef std::vector<std::vector<int> > matrix;
typedef std::vector<std::vector<char> > matrixChar;

// Check if the board is solved
bool checkBoard(matrix board, matrixChar visibleBoard) {
	int boardWidth = board.size();
	int boardHeight = board[0].size();
	// Iterate through the visible and the true board and see if all the flags match all the bombs
	for (int i = 0; i < boardWidth; i++) {
		for (int j = 0; j < boardHeight; j++) {
			if (board[i][j] == 9 && visibleBoard[i][j] != 'P') return false;
		}
	}
	return true;
}

// Draws one row of the board
void drawRow(matrixChar board, int x, int i, int boardWidth, bool playerRow) {
	for (int j = 0; j < boardWidth; j++) {
		unsigned char fg = 90;
		unsigned char bg = 0;
		switch (board[j][i]) {
		case '1':
			fg = 34;
			break;
		case '2':
			fg = 32;
			break;
		case '3':
			fg = 33;
			break;
		case '4':
			fg = 35;
			break;
		case '5':
			fg = 36;
			break;
		case '6':
			fg = 94;
			break;
		case '7':
			fg = 92;
			break;
		case '8':
			fg = 93;
			break;
		}

		if (board[j][i] == 'P') {
			fg = 31;
		}
		if (board[j][i] == '@') {
			bg = 41;
			fg = 31;
		}
		else if (j == x && playerRow)
			bg = 103;

		printf("\033[%d;%dm%c\033[m ", bg, fg, board[j][i]);
	}
}

// Update only one row of the board
void updateBoard(matrixChar board, int x, int y, bool lastMoveDown, int minesLeft) {
	int boardWidth = board.size();
	int boardHeight = board[0].size();

	// Move cursor to 0, 0 and then to y=0 on the board
	std::cout << "\033[H\033[3E";

	// Move cursor to the row the player is currently on
	std::cout << "\033[" << y + 1 << "E";

	// 4 spaces to add some left margin
	std::cout << "    ";

	// Draw the row
	drawRow(board, x, y, boardWidth, true);

	// Player moved down (update the row above)
	if (lastMoveDown && y > 0) {
		std::cout << "\033[F";
		y--;
	}
	// Player moved up (update the row below)
	else if(!lastMoveDown && y < boardHeight - 1) {
		std::cout << "\033[E";
		y++;
	}
	// Player teleported from the bottom to the top (update the last row of the board)
	else if (lastMoveDown && y == 0) {
		// Move cursor to the bottom of the board
		std::cout << "\033[" << boardHeight - 1 << "E";
		y = boardHeight - 1;
	}
	// Player teleported from the top to the bottom (update the first row of the board)
	else {
		std::cout << "\033[H\033[4E";
		y = 0;
	}

	// 4 spaces to add some left margin
	std::cout << "    ";

	// Update the row player was previously on (to remove the player cursor)
	drawRow(board, x, y, boardWidth, false);
}

// Draws the entire board
void drawEntireBoard(matrixChar board, int x, int y, int minesLeft) {
	int boardWidth = board.size();
	int boardHeight = board[0].size();

	system("cls");

	// Default left margin of 4
	std::cout << "   ";

	// Add the left margin to center the title
	for (int i = 0; i <= boardWidth - 8; i++)
		std::cout << " ";

	std::cout << "\033[96;6mMINESWEEPER\033[m\n\n";  // Title
	// Amount of mines left
	std::cout << "\033[96;6m    Mines Left: " << minesLeft << "\033[m\n";
	// Iterate through all the rows
	for (int i = 0; i < boardHeight; i++) {
		// Move to the next line and add margin
		std::cout << "\n    ";
		// Draw a row and the player cursor if it is the row with the player
		if (i == y) drawRow(board, x, i, boardWidth, true);
		else drawRow(board, x, i, boardWidth, false);
	}
	// Save the end position for the console cursor 
	std::cout << "\n\n\033[s";
}

// Draws the main menu
void drawMenu(char option) {
	unsigned char chosenBg = 103;
	unsigned char chosen[4] = { 0, 0, 0, 0 };
	chosen[option] = chosenBg;

	std::cout << "      \033[96;6mMINESWEEPER\033[m\n\n";    // Title
	printf("         \033[%d;32mEASY\033[m\n", chosen[0]);   // Easy
	printf("        \033[%d;33mMEDIUM\033[m\n", chosen[1]);  // Medium
	printf("         \033[%d;31mHARD\033[m\n", chosen[2]);   // Hard
	printf("        \033[%d;35mCUSTOM\033[m\n", chosen[3]);  // Custom
}

// Breaks a single tile
char breakTile(int x, int y, matrix& board, matrixChar& visibleBoard) {
	int boardWidth = board.size();
	int boardHeight = board[0].size();
	// If a mine has been uncovered
	if (board[x][y] == 9) {
		visibleBoard[x][y] = '@';
		// Game over
		return 1;
	}
	// If there are no bombs surrounding a tile
	else if (board[x][y] == 0) {
		visibleBoard[x][y] = '_';
		// Recursively break all surrounding tiles
		for (int i = -1; i < 2; i++)
			for (int j = -1; j < 2; j++)
				if (x + i >= 0 && x + i < boardWidth)
					if (y + j >= 0 && y + j < boardHeight)
						if(visibleBoard[x + i][y + j] != '_' && visibleBoard[x + i][y + j] != 'P')
							breakTile(x + i, y + j, board, visibleBoard);
		// Update entire board
		return 2;
	}
	// Tile with number showing how many bombs are nearby
	else
		visibleBoard[x][y] = board[x][y] + 48;
	// Continue normally
	return 0;
}

// Keyboard handling
char moveHandle(char ch, int* x, int* y, bool* lastMoveDown, matrix& board, matrixChar& visibleBoard, int* minesLeft) {
	int boardWidth = board.size();
	int boardHeight = board[0].size();

	// Movement
	if (ch == 'w' || ch == 'W' || ch == 72) {       // Up
		*y -= 1;
		*lastMoveDown = false;
	}
	else if (ch == 's' || ch == 'S' || ch == 80) {  // Down
		*y += 1;
		*lastMoveDown = true;
	}
	if (ch == 'a' || ch == 'A' || ch == 75)         // Left
		*x -= 1;
	else if (ch == 'd' || ch == 'D' || ch == 77)    // Right
		*x += 1;
	// Movement cap (prevents player from escaping the board)
	if (*x >= boardWidth) *x = 0;
	else if (*x < 0) *x = boardWidth - 1;
	if (*y >= boardHeight) *y = 0;
	else if (*y < 0) *y = boardHeight - 1;

	// Other events
	if (ch == 'f' || ch == 'F') {  // Flag
		if (visibleBoard[*x][*y] == '#') {
			*minesLeft -= 1;
			visibleBoard[*x][*y] = 'P';
		}
		else if (visibleBoard[*x][*y] == 'P') {
			*minesLeft += 1;
			visibleBoard[*x][*y] = '#';
		}
	}
	else if (ch == '\r' || ch == ' ')  // Break
		return breakTile(*x, *y, board, visibleBoard);

	return 0;
}

int gameLoop() {
	int boardWidth = 10, boardHeight = 10;
	int numOfMines = 10;
	int x = 0, y = 0;
	bool isRunning = true, isWon = false;

	char choice = -1;
	char option = 0;

	// Hide the console cursor
	std::cout << "\033[?25l";

	// Clear the entire screen
	system("cls");

	/* Main Menu */

	drawMenu(option);

	while (choice == -1) {
		if (_kbhit()) {
			char ch = _getch();

			// Esc
			if (ch == 27) return 0;

			system("cls");

			if (ch == 'w' || ch == 'W' || ch == 72)  // Up
				option--;
			else if (ch == 's' || ch == 'S' || ch == 80)  // Down
				option++;

			// Menu cap (prevents player from choosing mode that doesn't exist)
			if (option < 0) option = 0;
			if (option > 3) option = 3;

			if (ch == '\r' || ch == ' ')
				choice = option;

			drawMenu(option);
		}
	}

	system("cls");

	switch (choice) {
	case 0:   // Easy
		// Defaults of the variables are already set to easy
		break;
	case 1:   // Medium
		boardWidth = 16;
		boardHeight = 16;
		numOfMines = 30;
		break;
	case 2:   // Hard
		boardWidth = 24;
		boardHeight = 24;
		numOfMines = 90;
		break;
	default:  // Custom
		bool pass = false;
		while (!pass) {
			std::cout << "\033[0;31mEntire board (including the title) has to fit in the window or else glitches may occur\033[m\n";
			std::cout << "\033[0;37mboard width: \033[m";      // board width
			std::cin >> boardWidth;
			system("cls");
			std::cout << "\033[0;37mboard height: \033[m";     // board height
			std::cin >> boardHeight;
			system("cls");
			std::cout << "\033[0;37mnumber of mines: \033[m";  // number of mines
			std::cin >> numOfMines;
			system("cls");
			// Check if all the data is correct, if it's not then show proper error message
			if (numOfMines > (boardHeight * boardWidth)) {
				std::cout << "\033[0;31mYou can't have more mines than spaces on the board.\033[m\n";
				system("pause");
			}
			else if (boardHeight < 2 || boardWidth < 2) {
				std::cout << "\033[0;31mMinium board size is 2x2.\033[m\n";
				system("pause");
			}
			else pass = true;
			system("cls");
		}
		break;
	}

	/* Generating The Board */

	matrix board(boardWidth, std::vector<int>(boardHeight, 0));
	matrixChar visibleBoard(boardWidth, std::vector<char>(boardHeight, '#'));

	srand((unsigned int)time(NULL));

	for (int i = 0; i < numOfMines; i++) {
		int ranX = rand() % boardWidth;
		int ranY = rand() % boardHeight;
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

	drawEntireBoard(visibleBoard, x, y, numOfMines);

	bool lastMoveDown = false;

	/* Main Game Loop */

	while (isRunning) {
		if (_kbhit()) {
			char ch = _getch();
			// Esc
			if (ch == 27) return 1;
			char moveResult = moveHandle(ch, &x, &y, &lastMoveDown, board, visibleBoard, &numOfMines);
			if (moveResult == 1)
				isRunning = false;
			if (numOfMines == 0)
				if (checkBoard(board, visibleBoard)) {
					isRunning = false;
					isWon = true;
				}

			if (moveResult != 2) updateBoard(visibleBoard, x, y, lastMoveDown, numOfMines);
			else drawEntireBoard(visibleBoard, x, y, numOfMines);
		}
	}

	/* Game Over */

	// Move cursor to the saved position at the end + left margin 4
	std::cout << "\033[u    ";

	// Margin to put win/lose message in the center
	for (int i = 0; i <= boardWidth - 6; i++)
		std::cout << " ";

	if (isWon)
		std::cout << "\033[0;92mYou Won!\033[m\n\n";
	else 
		std::cout << "\033[0;31mYou Lost!\033[m\n\n";

	// Wait for 2 seconds
	system("ping -n 3 127.0.0.1 > nul");
	// Cut off user input until sleeping is finished 
	while (_kbhit())
		int _ = _getch();

	system("pause");

	return 2;
}

int main() {
	int result = -1;

	while (true) {
		result = gameLoop();
		if (result == 0)
			break;
	}

	return 0;
}