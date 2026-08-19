#pragma once
#ifndef __UTILS_H__
#define __UTILS_H__
#include "game.h"

void InitRandom();

void generateRandomTile(int &x, int & y);

int generate2or4(int mod, int mi);
#endif