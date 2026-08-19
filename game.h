#pragma once
#ifndef __GAME_H__
#define __GAME_H__

#include <windows.h>
#include<stdlib.h>  //rand()
#include<time.h>
#include<stdbool.h>
#include <memory.h>
#include "utils.h"

#define GRID_SIZE 4


extern int score;
extern int gameBoard[GRID_SIZE][GRID_SIZE];

extern bool game_over_flag;

void initGameBoard();

void slideAndCombine(int line[GRID_SIZE]);

void spawnNewTile();

void moveUp();

void moveDown();

void moveLeft();

void moveRight();

bool checkFirstWin();
bool isGameOver();

#endif
