#pragma once
#ifndef __GRAPHICS_H__
#define __GRAPHICS_H__
#include<windows.h>
#include "Resource.h"
#include "game.h"
#include "input.h"
#include "leaderboard.h"
#include<wchar.h>

#define MAX_LOADSTRING 100

// 全局变量:
extern HINSTANCE hInst;                                // 当前实例
extern WCHAR szTitle[MAX_LOADSTRING];                     // 标题栏文本
extern WCHAR szWindowClass[MAX_LOADSTRING];            // 主窗口类名

extern HWND mainWindow;

// 此代码模块中包含的函数的前向声明:
ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);


#define  TILE_SIZE  80
#define  TILE_PADDING  5


#define CELL_RECT_SIZE ( TILE_PADDING  * 2 + TILE_SIZE)

#define   WINDOW_WIDTH  (GRID_SIZE * CELL_RECT_SIZE +  TILE_PADDING * 7 )
#define   WINDOW_HEIGHT ((GRID_SIZE + 1) *  CELL_RECT_SIZE +   8 * TILE_PADDING)

typedef struct TileStyle {
    int tileValue;
    DWORD backgroundRgb;
    DWORD textRGB;
    DWORD fontSize;
}TileStyle;



//
//  函数: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  目标: 处理主窗口的消息。
//
//  WM_COMMAND  - 处理应用程序菜单
//  WM_PAINT    - 绘制主窗口
//  WM_DESTROY  - 发送退出消息并返回
//
//
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

// “关于”框的消息处理程序。
INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam);

void drawGameBoard(HDC hdc);


// 对话框过程声明
INT_PTR CALLBACK GameOverDialogProc(HWND hwndDlg, UINT uMsg, WPARAM wParam, LPARAM lParam);
INT_PTR CALLBACK firstWinDialogProc(HWND hwndDlg, UINT uMsg, WPARAM wParam, LPARAM lParam);
INT_PTR CALLBACK LeaderboardDialogProc(HWND hwndDlg, UINT uMsg, WPARAM wParam, LPARAM lParam);
INT_PTR CALLBACK InputNameDialogProc(HWND hwndDlg, UINT uMsg, WPARAM wParam, LPARAM lParam);
INT_PTR CALLBACK NewRecordDialogProc(HWND hwndDlg, UINT uMsg, WPARAM wParam, LPARAM lParam);

// 入榜流程（可复用）：提交分数 → 若入榜则弹输入姓名 → 弹庆祝弹窗
void TrySubmitAndCelebrate(HWND hwndParent, int score);

void centerWindow(HWND hwnd);
#endif