#include<iostream>
#include<conio.h>
#include<stdio.h>
#include<stdlib.h>
#include<time.h>
#include<vector>
#include<cstdlib>

typedef std::vector<std::vector<int> > matrix;
typedef std::vector<std::vector<char> > matrixChar;
typedef std::vector<std::vector<bool> > matrixBool;

// Directional arrays
// Used for checking tiles surrounding a main tile in this order:
// 3 1 6
// 4   7
// 5 2 8
int dx[8] = { -1, 1, -1,  0,  1, -1, 0, 1 };
int dy[8] = {  0, 0, -1, -1, -1,  1, 1, 1 };


// Check if the board is solved
bool checkBoard(matrix board, matrixChar visibleBoard) {
	int boardWidth = board.size();
	int boardHeight = board[0].size();
	// Iterate through the visible and the true board and see if all the flags match all the bombs
	for (int i = 0; i < boardWidth; i++)
		for (int j = 0; j < boardHeight; j++)
			if (board[i][j] == 9 && visibleBoard[i][j] != 'P') return false;
	return true;
}


// Draws one row of the board
void drawRow(matrixChar board, int x, int i, int boardWidth, bool playerRow) {
	// 4 spaces to add some left margin
	std::cout << "    ";

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

		if (board[j][i] == 'P')
			fg = 31;
		if (board[j][i] == '@') {
			bg = 41;
			fg = 31;
		}
		else if (j == x && playerRow)
			bg = 103;

		printf("\033[%d;%dm%c\033[m ", bg, fg, board[j][i]);
	}
}


// Update only current and previous row of the board
void updateBoard(matrixChar board, int x, int y, bool lastMoveDown, int minesLeft) {
	int boardWidth = board.size();
	int boardHeight = board[0].size();

	// Move cursor to 0, 0 and then to y=0 on the board
	std::cout << "\033[H\033[2E";
	std::cout << "\033[96;6m    Mines Left: " << minesLeft << " \033[m\n";

	// Move cursor to the row the player is currently on
	std::cout << "\033[" << y + 1 << "E";

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

	// Update the row player was previously on (to remove the player cursor)
	drawRow(board, x, y, boardWidth, false);
}


// Update 3 rows of the board that AI has been using and the previous row with AI's pointer 
void updateBoardAI(matrixChar board, int x, int y, int lastMoveY, int minesLeft) {
	int boardWidth = board.size();
	int boardHeight = board[0].size();

	// Move cursor to 0, 0 and then to y=0 on the board
	std::cout << "\033[H\033[2E";
	std::cout << "\033[96;6m    Mines Left: " << minesLeft << " \033[m\n";

	// Move cursor to the row the player is currently on
	std::cout << "\033[" << y + 1 << "E";

	// Draw the row
	drawRow(board, x, y, boardWidth, true);

	if (y - 1 > 0) {
		y--;
		std::cout << "\033[F";
		drawRow(board, x, y, boardWidth, false);
		y++;
		std::cout << "\033[E";
	}

	if (y + 1 < boardHeight) {
		std::cout << "\033[E";
		y++;
		drawRow(board, x, y, boardWidth, false);
	}

	std::cout << "\033[H\033[" << lastMoveY + 4 << "E";

	drawRow(board, x, lastMoveY, boardWidth, false);
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
		std::cout << "\n";
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


// Draws the main menu
void drawMenuPlayerAI(char option) {
	unsigned char chosenBg = 103;
	unsigned char chosen[2] = { 0, 0 };
	chosen[option] = chosenBg;

	std::cout << "      \033[96;6mMINESWEEPER\033[m\n\n";     // Title
	printf("        \033[%d;32mPLAYER\033[m\n", chosen[0]);  // Player
	printf("          \033[%d;33mAI\033[m\n", chosen[1]);       // AI
}


// Checks if a tile is within the board borders
bool isTileExisting(int x, int y, int boardWidth, int boardHeight) {
	return x >= 0 && y >= 0 && x < boardWidth && y < boardHeight;
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
		for (int i = 0; i < 9; i++)
			if (isTileExisting(x + dx[i], y + dy[i], boardWidth, boardHeight))
				if(visibleBoard[x + dx[i]][y + dy[i]] != '_' && visibleBoard[x + dx[i]][y + dy[i]] != 'P')
					breakTile(x + dx[i], y + dy[i], board, visibleBoard);
		// Update entire board
		return 2;
	}
	// Tile with number showing how many bombs are nearby
	else
		visibleBoard[x][y] = board[x][y] + 48;
	// Continue normally
	return 0;
}


// AI move handling
char AImoveHandle(int& x, int& y, matrix& board, matrixChar& visibleBoard, int& minesLeft) {
	int boardWidth = board.size();
	int boardHeight = board[0].size();

	// Final move result
	// 0 - normal move
	// 1 - mine
	// 2 - update entire board
	char result = -1;

	// Traverse the board
	for(int i = 0; i < boardWidth; i++)
		for (int j = 0; j < boardHeight; j++)
			// If a tile with number has been found
			if (visibleBoard[i][j] != '#' && visibleBoard[i][j] != 'P' && visibleBoard[i][j] != '_') {
				int num = visibleBoard[i][j] - 48;
				int count = 0;

				// Is there at least one empty unknown field surrounding the tile
				bool isOneEmpty = false;

				// Check if there is at least one empty unknown field surrounding the tile
				for (int k = 0; k < 8; k++)
					if (isTileExisting(i + dx[k], j + dy[k], boardWidth, boardHeight))
						if (visibleBoard[i + dx[k]][j + dy[k]] == '#') {
							isOneEmpty = true;
							break;
						}

				// If all fields surrounding the tile are known - continue
				if (!isOneEmpty) continue;

				// Count flags surrounding the tile
				for (int k = 0; k < 8; k++)
					if (isTileExisting(i + dx[k], j + dy[k], boardWidth, boardHeight))
						if (visibleBoard[i + dx[k]][j + dy[k]] == 'P')
							count++;

				// If the amount of flags is the same as the amount of mines surrounding the tile
				if (count == num && isOneEmpty) {
					x = i;
					y = j;

					// Break all tiles that are not flagged
					for (int k = 0; k < 8; k++)
						if (isTileExisting(i + dx[k], j + dy[k], boardWidth, boardHeight))
							if (visibleBoard[i + dx[k]][j + dy[k]] == '#') {
								char moveResult = breakTile(i + dx[k], j + dy[k], board, visibleBoard);
								if (moveResult == 1)
									return 1;
								else if (result != 1)
									if (moveResult == 2)
										return 2;
									else if (result != 2)
										return 0;
							}
				}
				else {
					count = 0;

					// Count flags and unkown tiles surrounding the tile
					for (int k = 0; k < 8; k++)
						if (isTileExisting(i + dx[k], j + dy[k], boardWidth, boardHeight))
							if (visibleBoard[i + dx[k]][j + dy[k]] == '#' || visibleBoard[i + dx[k]][j + dy[k]] == 'P')
								count++;

					// If the amount of flags and unkown tiles is the same as the amount of mines surrounding the tile
					if (count == num && isOneEmpty) {
						x = i;
						y = j;

						// Flag unknown tiles
						for (int k = 0; k < 8; k++)
							if (isTileExisting(i + dx[k], j + dy[k], boardWidth, boardHeight))
								if (visibleBoard[i + dx[k]][j + dy[k]] == '#') {
									minesLeft -= 1;
									visibleBoard[i + dx[k]][j + dy[k]] = 'P';
								}

						return 0;
					}
				
				}

			}

	// If the 2 rules don't apply for any tile on the board
	if (result == -1) {
		// Choose random tile out of the remaining 
		for (int i = 0; i < boardWidth; i++) {
			int ranX = rand() % boardWidth;
			int ranY = rand() % boardHeight;

			if (visibleBoard[ranX][ranY] == '#') {
				x = ranX;
				y = ranY;
				return breakTile(ranX, ranY, board, visibleBoard);
			}
		}
	}
	// If there are too many uncovered tiles for a random choice to be made
	if (result == -1) {
		// Choose first encountered unkown tile
		for (int i = 0; i < boardWidth; i++)
			for (int j = 0; j < boardHeight; j++)
				if (visibleBoard[i][j] == '#') {
					x = i;
					y = j;

					return breakTile(i, j, board, visibleBoard);
				}
	}
	// There are no more unkown tiles and the game didn't end (error?)
	if (result == -1)
		return 1;
}


// Keyboard handling
char moveHandle(char ch, int& x, int& y, bool& lastMoveDown, matrix& board, matrixChar& visibleBoard, int& minesLeft) {
	int boardWidth = board.size();
	int boardHeight = board[0].size();

	// Movement
	if (ch == 'w' || ch == 'W' || ch == 72) {       // Up
		y -= 1;
		lastMoveDown = false;
	}
	else if (ch == 's' || ch == 'S' || ch == 80) {  // Down
		y += 1;
		lastMoveDown = true;
	}
	if (ch == 'a' || ch == 'A' || ch == 75)         // Left
		x -= 1;
	else if (ch == 'd' || ch == 'D' || ch == 77)    // Right
		x += 1;
	// Movement cap (prevents player from escaping the board)
	if (x >= boardWidth) x = 0;
	else if (x < 0) x = boardWidth - 1;
	if (y >= boardHeight) y = 0;
	else if (y < 0) y = boardHeight - 1;

	// Other events
	if (ch == 'f' || ch == 'F') {  // Flag
		if (visibleBoard[x][y] == '#' && minesLeft > 0) {
			minesLeft -= 1;
			visibleBoard[x][y] = 'P';
		}
		else if (visibleBoard[x][y] == 'P') {
			minesLeft += 1;
			visibleBoard[x][y] = '#';
		}
	}
	else if (ch == '\r' || ch == ' ')  // Break
		return breakTile(x, y, board, visibleBoard);

	return 0;
}


// Main game loop
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


	/* Difficulty Level */

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
			std::cout << "\033[0;37mboard width: \033[m";      // Board width
			std::cin >> boardWidth;
			system("cls");
			std::cout << "\033[0;37mboard height: \033[m";     // Board height
			std::cin >> boardHeight;
			system("cls");
			std::cout << "\033[0;37mnumber of mines: \033[m";  // Number of mines
			std::cin >> numOfMines;
			system("cls");
			// Check if all the data is correct, if it's not then show proper error message
			if (numOfMines > (boardHeight * boardWidth)) {
				std::cout << "\033[0;31mYou can't have more mines than tile on the board.\033[m\n";
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
		// Random coordinates for a mine
		int ranX = rand() % boardWidth;
		int ranY = rand() % boardHeight;
		// Set mine's coordinates if a mine doesn't already exist in this place
		if (board[ranX][ranY] != 9) board[ranX][ranY] = 9;
		// If a mine exists in this place, go back in loop (generate new random coordinates)
		else {
			i--;
			continue;
		};
		// Increase mine count of every tile surrouding the mine (if a tile isn't a mine itself)
		for (int i = 0; i < 8; i++)
			if (isTileExisting(ranX + dx[i], ranY + dy[i], boardWidth, boardHeight))
				if (board[ranX + dx[i]][ranY + dy[i]] != 9)
					board[ranX + dx[i]][ranY + dy[i]]++;
	}


	/* Player or AI */

	choice = -1;
	option = 0;
	drawMenuPlayerAI(option);

	while (choice == -1) {
		// Get user input
		if (_kbhit()) {
			char ch = _getch();

			// Esc
			if (ch == 27) return 1;

			system("cls");

			if (ch == 'w' || ch == 'W' || ch == 72)  // Up
				option--;
			else if (ch == 's' || ch == 'S' || ch == 80)  // Down
				option++;

			// Menu cap (prevents player from choosing mode that doesn't exist)
			if (option < 0) option = 0;
			if (option > 1) option = 1;

			if (ch == '\r' || ch == ' ')
				choice = option;

			drawMenuPlayerAI(option);
		}
	}

	bool isPlayer = true;

	if (choice == 1) isPlayer = false;


	/* Main Game Loop */

	drawEntireBoard(visibleBoard, x, y, numOfMines);

	// Player
	if (isPlayer) {
		bool lastMoveDown = false;

		while (isRunning) {
			if (_kbhit()) {
				// Get user input
				char ch = _getch();
				// Esc
				if (ch == 27) return 1;
				// Handle user input
				// Move result
				// 0 - normal move
				// 1 - mine
				// 2 - update entire board
				char moveResult = moveHandle(ch, x, y, lastMoveDown, board, visibleBoard, numOfMines);
				// Player exploded
				if (moveResult == 1)
					isRunning = false;
				// Player used all the flags
				if (numOfMines == 0)
					// Player won
					if (checkBoard(board, visibleBoard)) {
						isRunning = false;
						isWon = true;
					}

				// Update entire board only if player found empty space
				if (moveResult != 2) updateBoard(visibleBoard, x, y, lastMoveDown, numOfMines);
				else drawEntireBoard(visibleBoard, x, y, numOfMines);
			}
		}
	}
	// AI
	else {
		int lastMoveY = 0;
		while (isRunning) {
			// Get move from AI
			char moveResult = AImoveHandle(x, y, board, visibleBoard, numOfMines);
			// AI lost
			if (moveResult == 1)
				isRunning = false;
			// AI used all the flags
			if (numOfMines == 0)
				// AI won
				if (checkBoard(board, visibleBoard)) {
					isRunning = false;
					isWon = true;
				}
			// Update entire board only if AI found empty space
			if (moveResult != 2) updateBoardAI(visibleBoard, x, y, lastMoveY, numOfMines);
			else drawEntireBoard(visibleBoard, x, y, numOfMines);
			// Store last move (to update AI's pointer)
			lastMoveY = y;
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