#include "game.h"

int score = 0;
int gameBoard[GRID_SIZE][GRID_SIZE];
bool  move_flag = false;
bool win_flag = false;
bool game_over_flag = false;

void initGameBoard()
{
	memset(gameBoard, 0, sizeof(gameBoard));
	int x,y;
	generateRandomTile(x, y);
	generateRandomTile(x, y);
	score = 0;
	win_flag = false;
	game_over_flag = false;
}


void spawnNewTile()
{
	int x, y;
	if (move_flag) {
		generateRandomTile(x, y);
		move_flag = false;
	}

}


void slideAndCombine(int line[GRID_SIZE])
{
	int temp[GRID_SIZE] = {0};
	int index = 0;
	int first = - 1;
	for (int i = 0; i < GRID_SIZE; i++) {
		if (line[i] != 0)
		{
			temp[index++] = line[i];
			if (first != -1)
				move_flag = true;
		}
		else if (first == -1)
			first = i;
	}
	if(index)
	for (int i = 0; i < GRID_SIZE - 1; i++) {
		if (temp[i] != 0 && temp[i] == temp[i + 1]) {
			temp[i] *= 2;
			score += temp[i];
			temp[i + 1] = 0;
			move_flag = true;
		}
	}
	index = 0;
	for (int i = 0; i < GRID_SIZE; i++)
		if (temp[i] != 0)
			line[index++] = temp[i];
	while (index < GRID_SIZE)
		line[index++] = 0;
}


void moveUp()
{
	for (int col = 0; col < GRID_SIZE; col++) {
		int line[GRID_SIZE] = {};
		for (int row = 0; row < GRID_SIZE; row++) {
			line[row] = gameBoard[row][col];
		}
		slideAndCombine(line);
		for (int row = 0; row < GRID_SIZE; row++) {
			gameBoard[row][col]=line[row];
		}
	}
	spawnNewTile();


}

void moveDown()
{
	for (int col = 0; col < GRID_SIZE; col++) {
		int line[GRID_SIZE] = {};
		for (int row = 0; row < GRID_SIZE; row++) {
			line[row] = gameBoard[GRID_SIZE - 1 -  row][col];
		}
		slideAndCombine(line);
		for (int row = 0; row < GRID_SIZE; row++) {
			gameBoard[GRID_SIZE - 1 - row][col] = line[row];
		}
	}
	spawnNewTile();
}

void moveLeft()
{
	for (int row = 0; row < GRID_SIZE; row++) {
		int line[GRID_SIZE] = {};
		for (int col = 0; col < GRID_SIZE; col++) {
			line[col] = gameBoard[row][col];
		}
		slideAndCombine(line);
		for (int col = 0; col < GRID_SIZE; col++) {
			gameBoard[row][col] = line[col];
		}
	}
	spawnNewTile();
}

void moveRight()
{
	for (int row = 0; row < GRID_SIZE; row++) {
		int line[GRID_SIZE] = {};
		for (int col = 0; col < GRID_SIZE; col++) {
			line[col] = gameBoard[row][GRID_SIZE - 1 - col];
		}
		slideAndCombine(line);
		for (int col = 0; col < GRID_SIZE; col++) {
			gameBoard[row][GRID_SIZE - 1 - col] = line[col];
		}
	}
	spawnNewTile();
}


bool checkFirstWin()
{
	if(win_flag )return false;

	for(int i=0;i<GRID_SIZE;i++)
		for (int j = 0; j < GRID_SIZE; j++)
			if (gameBoard[i][j] >= 2048 ) {
				win_flag = true;
				return true;
			}
	return false;
}

bool isGameOver()
{

	for (int i = 0; i < GRID_SIZE; i++)
		for (int j = 0; j < GRID_SIZE; j++)
			if (gameBoard[i][j] == 0)
				return false;

	for (int i = 0; i < GRID_SIZE; i++)
		for (int j = 0; j < GRID_SIZE; j++) {
			if (j < GRID_SIZE - 1 && gameBoard[i][j] == gameBoard[i][j + 1])
				return false;
			if (i < GRID_SIZE - 1 && gameBoard[i][j] == gameBoard[i + 1][j])
				return false;
		}
	return true;
}
