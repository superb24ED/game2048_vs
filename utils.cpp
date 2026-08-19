#include "utils.h"

#define RANDOM_MOD 4
#define RANDOM_MIDDLE 2

extern int gameBoard[GRID_SIZE][GRID_SIZE];
void InitRandom()
{
	srand(time(NULL));
}
void generateRandomTile(int& x, int& y) {

	int emptyCells[GRID_SIZE * GRID_SIZE][2] = {};
	int count = 0;
	for (int i = 0; i < GRID_SIZE; i++)
		for (int j = 0; j < GRID_SIZE; j++)
			if (!gameBoard[i][j]) {
				emptyCells[count][0] = i;
				emptyCells[count][1] = j;
				count++;
			}
	if (count == 0) return;

	int randIndex = rand() % count;
	x = emptyCells[randIndex][0];
	y = emptyCells[randIndex][1];
	int randValue = generate2or4( RANDOM_MOD, RANDOM_MIDDLE);
	gameBoard[x][y] = randValue;
}


int generate2or4(int mod, int mi) {
	if (mod <= 0) return 2;
	int num = rand() % mod;
	if (num > mi) return 4;
	else return 2;
}
