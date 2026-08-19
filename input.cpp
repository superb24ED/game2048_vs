#include "input.h"

void processInput(WPARAM wParam)
{
    switch (wParam)
    {
    case 'W':
    case VK_UP:
        moveUp();
        break;

    case 'A':
    case VK_LEFT:
        moveLeft();
        break;

    case 'S':
    case VK_DOWN:
        moveDown();
        break;
        
    case 'D':
    case VK_RIGHT:
        moveRight();
        break;

    default:
        break;
    }
}