#include "graphics.h"
#include <string>

#define CELL_NUMBER_SIZE 5

#define TILE_STYLE_NUM 50
#define DEFAULT_STYLE 13

// 不同数值方块的视觉样式表：背景色 / 文字色 / 字号
struct TileStyle TileStyleList[TILE_STYLE_NUM] = {
    // 空格子（值为 0）
    {0,         static_cast<DWORD>(RGB(204, 192, 179)), static_cast<DWORD>(RGB(204, 192, 179)), static_cast<DWORD>(0.8 * TILE_SIZE)},
    //{2, RGB(237, 207, 114), RGB(119, 110, 101), 0.8 * TILE_SIZE},
    {2,         static_cast<DWORD>(RGB(240, 230, 220)), static_cast<DWORD>(RGB(120, 110, 100)), static_cast<DWORD>(0.8 * TILE_SIZE)},
    {4,         static_cast<DWORD>(RGB(230, 220, 200)), static_cast<DWORD>(RGB(120, 110, 100)), static_cast<DWORD>(0.8 * TILE_SIZE)},
    {8,         static_cast<DWORD>(RGB(230, 180, 130)), static_cast<DWORD>(RGB(240, 240, 240)), static_cast<DWORD>(0.8 * TILE_SIZE)},
    {16,        static_cast<DWORD>(RGB(220, 160, 100)), static_cast<DWORD>(RGB(240, 240, 240)), static_cast<DWORD>(0.8 * TILE_SIZE)},
    {32,        static_cast<DWORD>(RGB(230, 130, 100)), static_cast<DWORD>(RGB(240, 240, 240)), static_cast<DWORD>(0.8 * TILE_SIZE)},
    {64,        static_cast<DWORD>(RGB(230, 100,  70)), static_cast<DWORD>(RGB(240, 240, 240)), static_cast<DWORD>(0.8 * TILE_SIZE)},
    {128,       static_cast<DWORD>(RGB(230, 200, 120)), static_cast<DWORD>(RGB(240, 240, 240)), static_cast<DWORD>(0.6 * TILE_SIZE)},
    {256,       static_cast<DWORD>(RGB(235, 200, 100)), static_cast<DWORD>(RGB(240, 240, 240)), static_cast<DWORD>(0.6 * TILE_SIZE)},
    {512,       static_cast<DWORD>(RGB(240, 200,  70)), static_cast<DWORD>(RGB(240, 240, 240)), static_cast<DWORD>(0.6 * TILE_SIZE)},
    {1024,      static_cast<DWORD>(RGB( 95, 101, 105)), static_cast<DWORD>(RGB(240, 240, 240)), static_cast<DWORD>(0.4 * TILE_SIZE)},
    {2048,      static_cast<DWORD>(RGB( 82,  88,  91)), static_cast<DWORD>(RGB(240, 240, 240)), static_cast<DWORD>(0.4 * TILE_SIZE)},
    {4096,      static_cast<DWORD>(RGB( 67,  71,  74)), static_cast<DWORD>(RGB(240, 240, 240)), static_cast<DWORD>(0.4 * TILE_SIZE)},
    {8192,      static_cast<DWORD>(RGB( 44,  46,  48)), static_cast<DWORD>(RGB(250, 250, 250)), static_cast<DWORD>(0.4 * TILE_SIZE)},
    {16384,     static_cast<DWORD>(RGB(  3,   5,  10)), static_cast<DWORD>(RGB(250, 250, 250)), static_cast<DWORD>(0.3 * TILE_SIZE)},
};

// 根据数值查找样式，找不到则回退到默认样式（2048）
static struct TileStyle& getStyle(int value) {
    for (int i = 0; i < TILE_STYLE_NUM; i++) {
        if (TileStyleList[i].tileValue == value)
            return TileStyleList[i];
    }
    return TileStyleList[DEFAULT_STYLE];
}

// 绘制单个方块：填充背景色，若值非零再绘制居中数字
static void drawTile(HDC hdc, int x, int y, int value) {
    RECT rect = {};
    rect.left   = x + TILE_PADDING;
    rect.top    = y + TILE_PADDING;
    rect.right  = x + TILE_PADDING + TILE_SIZE;
    rect.bottom = y + TILE_PADDING + TILE_SIZE;

    struct TileStyle style = getStyle(value);
    HBRUSH hbr = CreateSolidBrush(style.backgroundRgb);
    FillRect(hdc, &rect, hbr);
    DeleteObject(hbr);

    if (value) {
        SetBkMode(hdc, TRANSPARENT);
        SetTextColor(hdc, style.textRGB);
        HFONT hFont = CreateFontW(
            style.fontSize,
            0,
            0, 0, FW_BOLD,
            FALSE, FALSE, FALSE, ANSI_CHARSET,
            OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
            DEFAULT_QUALITY, DEFAULT_PITCH | FF_SWISS,
            L"Arial"
        );
        HFONT oldFont = (HFONT)SelectObject(hdc, hFont);
        std::string text = std::to_string(value);
        DrawTextA(hdc, text.c_str(), -1, &rect, DT_CENTER | DT_VCENTER | DT_SINGLELINE);

        SelectObject(hdc, oldFont);
        DeleteObject(hFont);
    }
}

// 绘制整个游戏界面：棋盘背景 + 所有方块 + 分数文本，并在胜/负时弹出模态对话框
void drawGameBoard(HDC hdc)
{
    // 棋盘外层背景板
    RECT boardRect = {};
    boardRect.left   = TILE_PADDING;
    boardRect.top    = TILE_PADDING;
    boardRect.right  = TILE_PADDING * 3 + GRID_SIZE * (TILE_PADDING * 2 + TILE_SIZE);
    boardRect.bottom = TILE_PADDING * 3 + GRID_SIZE * (TILE_PADDING * 2 + TILE_SIZE);

    HBRUSH backgroundBrush = CreateSolidBrush(RGB(187, 173, 160));
    FillRect(hdc, &boardRect, backgroundBrush);
    DeleteObject(backgroundBrush);

    // 逐个绘制 4x4 方块
    for (int row = 0; row < GRID_SIZE; row++)
        for (int col = 0; col < GRID_SIZE; col++)
        {
            int value = gameBoard[row][col];
            int x = TILE_PADDING * 2 + col * (TILE_SIZE + 2 * TILE_PADDING);
            int y = TILE_PADDING * 2 + row * (TILE_SIZE + 2 * TILE_PADDING);
            drawTile(hdc, x, y, value);
        }

    // 分数显示区域
    RECT scoreRect = {};
    scoreRect.left   = TILE_PADDING;
    scoreRect.top    = TILE_PADDING + TILE_PADDING * 2 + GRID_SIZE * (TILE_SIZE + 2 * TILE_PADDING);
    scoreRect.right  = TILE_PADDING + GRID_SIZE * (TILE_SIZE + TILE_PADDING);
    scoreRect.bottom = static_cast<LONG>(TILE_PADDING + GRID_SIZE * (TILE_SIZE + TILE_PADDING) + 0.6 * TILE_SIZE);
    std::wstring scoreText = L"Score: " + std::to_wstring(score);
    DrawTextW(hdc, scoreText.c_str(), -1, &scoreRect, DT_CENTER | DT_VCENTER | DT_SINGLELINE);

    // 防重入：仅在游戏首次判定结束时弹一次，避免 WM_PAINT 每次重绘都重复弹 GameOver 对话框
    if (!game_over_flag && isGameOver()) {
        game_over_flag = true;
        DialogBox(GetModuleHandle(NULL), MAKEINTRESOURCE(IDD_GAMEOVER_DIALOG), mainWindow, GameOverDialogProc);
    }
    if (checkFirstWin()) {
        DialogBox(GetModuleHandle(NULL), MAKEINTRESOURCE(IDD_FIRSTWIN_DIALOG), mainWindow, firstWinDialogProc);
    }
}

// 游戏结束对话框：继续（重开一局） / 取消
INT_PTR CALLBACK GameOverDialogProc(HWND hwndDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    switch (uMsg) {
    case WM_INITDIALOG:
        return (INT_PTR)TRUE;

    case WM_COMMAND:
        if (LOWORD(wParam) == IDC_BUTTON_CONTINUE) {
            SendMessageW(mainWindow, WM_COMMAND, MAKEWPARAM(IDM_NEWGAME, 0), 0);
            EndDialog(hwndDlg, IDC_BUTTON_CONTINUE);
            return (INT_PTR)TRUE;
        }
        else if (LOWORD(wParam) == IDC_BUTTON_CANCEL) {
            EndDialog(hwndDlg, IDC_BUTTON_CANCEL);
            return (INT_PTR)TRUE;
        }
        break;
    }
    return (INT_PTR)FALSE;
}

// 首次胜利对话框：继续挑战 / 开始新游戏
INT_PTR CALLBACK firstWinDialogProc(HWND hwndDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    switch (uMsg) {
    case WM_INITDIALOG:
        return (INT_PTR)TRUE;

    case WM_COMMAND:
        if (LOWORD(wParam) == IDC_BUTTON_CONTINUE) {
            EndDialog(hwndDlg, LOWORD(wParam));
            return (INT_PTR)TRUE;
        }
        else if (LOWORD(wParam) == IDC_BUTTON_NEWGAME) {
            SendMessageW(mainWindow, WM_COMMAND, MAKEWPARAM(IDM_NEWGAME, 0), 0);
            EndDialog(hwndDlg, LOWORD(wParam));
            return (INT_PTR)TRUE;
        }
        break;
    }
    return (INT_PTR)FALSE;
}

// ============================================================
//  排行榜 Owner-Draw 辅助
// ============================================================

// 前三名的“奖牌色”（金 / 银 / 铜）；之后是阶梯灰蓝
static DWORD RankBkgColor(int rank)
{
    switch (rank) {
    case 1:  return RGB(255, 215,   0); // 金
    case 2:  return RGB(200, 200, 200); // 银
    case 3:  return RGB(205, 127,  50); // 铜
    case 4:  return RGB(224, 240, 255);
    case 5:  return RGB(230, 240, 245);
    default: return RGB(245, 248, 250);
    }
}
static DWORD RankTextColor(int rank)
{
    return (rank <= 3) ? RGB( 60,  40,  10) : RGB( 25,  35,  45);
}

// 将榜单加载进 ListBox（LBS_HASSTRINGS 存玩家名/分数/日期 合并串；另通过 WM_SETREDRAW + RESETCONTENT 避免闪烁）
static void RebuildLeaderboardList(HWND hList, int currentScore, HWND hStaticCurrent)
{
    SendMessageW(hList, WM_SETREDRAW, FALSE, 0);
    int count = (int)SendMessageW(hList, LB_GETCOUNT, 0, 0);
    for (int i = 0; i < count; i++) {
        ScoreEntry* p = (ScoreEntry*)SendMessageW(hList, LB_GETITEMDATA, i, 0);
        if (p) delete p;
    }
    SendMessageW(hList, LB_RESETCONTENT, 0, 0);

    std::vector<ScoreEntry> list = LoadLeaderboard();
    if (list.empty()) {
        SendMessageW(hList, LB_ADDSTRING, 0, (LPARAM)L"  (暂无记录，快去拿个第一名吧！)");
        SendMessageW(hList, LB_SETITEMDATA, 0, (LPARAM)nullptr);
    } else {
        for (size_t i = 0; i < list.size(); i++) {
            ScoreEntry* p = new ScoreEntry(list[i]);
            wchar_t display[128];
            // 仅用 HASSTRINGS 存一份“姓名 | 分数 | 日期”用于基础显示；Owner-draw 从 itemdata 取完整结构
            swprintf_s(display, L"%s  %d  %s", p->name, p->score, p->date);
            LRESULT idx = SendMessageW(hList, LB_ADDSTRING, 0, (LPARAM)display);
            if (idx != LB_ERR && idx != LB_ERRSPACE) {
                SendMessageW(hList, LB_SETITEMDATA, (WPARAM)idx, (LPARAM)p);
            } else {
                delete p;
            }
        }
    }
    SendMessageW(hList, WM_SETREDRAW, TRUE, 0);
    InvalidateRect(hList, nullptr, TRUE);

    // 更新“本局分数”提示
    if (hStaticCurrent) {
        std::wstring s;
        if (currentScore > 0) {
            int threshold = (int)list.size() < MAX_SCORES ? 0
                         : (list.empty() ? 0 : list.back().score);
            if (currentScore > threshold) {
                s = L"🎯 本局分数：" + std::to_wstring(currentScore) + L"   ⭐ 可入榜 Top " + std::to_wstring(MAX_SCORES) + L"！";
            } else {
                s = L"🎯 本局分数：" + std::to_wstring(currentScore);
            }
        } else {
            s = L"🎯 还没开始玩哦～";
        }
        SetWindowTextW(hStaticCurrent, s.c_str());
    }
}

static void CleanupLeaderboardList(HWND hList)
{
    int count = (int)SendMessageW(hList, LB_GETCOUNT, 0, 0);
    for (int i = 0; i < count; i++) {
        ScoreEntry* p = (ScoreEntry*)SendMessageW(hList, LB_GETITEMDATA, i, 0);
        if (p) delete p;
    }
}

// ============================================================
//  排行榜对话框（Owner-Draw + 清空 + 当前分数）
// ============================================================
INT_PTR CALLBACK LeaderboardDialogProc(HWND hwndDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    switch (uMsg) {
    case WM_INITDIALOG: {
        HWND hList = GetDlgItem(hwndDlg, IDC_LIST_SCORES);
        HWND hCur  = GetDlgItem(hwndDlg, IDC_STATIC_SCORE_CUR);
        // 设置 ListBox 等宽字体
        HFONT hFixed = CreateFontW(
            14, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE,
            DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
            CLEARTYPE_QUALITY, DEFAULT_PITCH | FF_MODERN, L"Consolas");
        if (hFixed) SendMessageW(hList, WM_SETFONT, (WPARAM)hFixed, TRUE);
        SetPropW(hwndDlg, L"LB_FONT", (HANDLE)hFixed);
        RebuildLeaderboardList(hList, score, hCur);
        return (INT_PTR)TRUE;
    }

    case WM_MEASUREITEM: {
        MEASUREITEMSTRUCT* mis = (MEASUREITEMSTRUCT*)lParam;
        if (mis->CtlType == ODT_LISTBOX) {
            mis->itemHeight = 32; // 每行 32px
        }
        return (INT_PTR)TRUE;
    }

    case WM_DRAWITEM: {
        DRAWITEMSTRUCT* dis = (DRAWITEMSTRUCT*)lParam;
        if (dis->CtlType != ODT_LISTBOX) return (INT_PTR)FALSE;
        HDC hdc = dis->hDC;
        RECT rc = dis->rcItem;
        int  idx = (int)dis->itemID;

        bool selected = (dis->itemState & ODS_SELECTED) != 0;
        // 底色（奖牌色，被选中则换为高亮蓝）
        COLORREF bkg = selected ? RGB(0, 120, 215) : RankBkgColor(idx + 1);
        COLORREF txt = selected ? RGB(255, 255, 255) : RankTextColor(idx + 1);

        HBRUSH hbr = CreateSolidBrush(bkg);
        FillRect(hdc, &rc, hbr);
        DeleteObject(hbr);

        ScoreEntry* p = (idx >= 0 && idx < (int)SendMessageW(dis->hwndItem, LB_GETCOUNT, 0, 0))
                      ? (ScoreEntry*)SendMessageW(dis->hwndItem, LB_GETITEMDATA, idx, 0)
                      : nullptr;

        // 1) 排名徽标（圆角色块 + 数字 + 奖牌文字）
        RECT rankRect = rc;
        rankRect.left += 6;
        rankRect.right = rankRect.left + 42;
        rankRect.top    += 4;
        rankRect.bottom -= 4;
        COLORREF rankBkg;
        const wchar_t* medal = L"";
        switch (idx + 1) {
        case 1:  rankBkg = RGB(255, 170,   0); medal = L"🥇"; break;
        case 2:  rankBkg = RGB(160, 160, 160); medal = L"🥈"; break;
        case 3:  rankBkg = RGB(184, 100,  30); medal = L"🥉"; break;
        default: rankBkg = RGB(120, 140, 160); medal = L"";   break;
        }
        HBRUSH hRank = CreateSolidBrush(selected ? RGB(255, 255, 255) : rankBkg);
        RoundRect(hdc, rankRect.left, rankRect.top, rankRect.right, rankRect.bottom, 10, 10);
        FillRect(hdc, &rankRect, hRank);
        DeleteObject(hRank);
        SetBkMode(hdc, TRANSPARENT);
        SetTextColor(hdc, selected ? rankBkg : RGB(255, 255, 255));

        RECT textRc = rankRect;
        wchar_t rankText[16];
        if (medal[0]) {
            swprintf_s(rankText, L"%s", medal);
        } else {
            swprintf_s(rankText, L"#%d", idx + 1);
        }
        HFONT hRankFont = CreateFontW(14, 0, 0, 0, FW_BOLD, FALSE, FALSE, FALSE,
            DEFAULT_CHARSET, 0, 0, CLEARTYPE_QUALITY, 0, L"Segoe UI Emoji");
        HFONT oldRF = (HFONT)SelectObject(hdc, hRankFont);
        DrawTextW(hdc, rankText, -1, &textRc, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
        SelectObject(hdc, oldRF);
        DeleteObject(hRankFont);

        if (p) {
            COLORREF normalTxt = selected ? RGB(255, 255, 255) : RGB(20, 30, 40);
            SetTextColor(hdc, normalTxt);

            // 2) 玩家名
            RECT nameRc = rc;
            nameRc.left = rankRect.right + 10;
            nameRc.right = nameRc.left + 130;  // 预留 130px 给名字
            nameRc.top    += 4;
            nameRc.bottom -= 4;
            HFONT hName = CreateFontW(14, 0, 0, 0, FW_SEMIBOLD, FALSE, FALSE, FALSE,
                DEFAULT_CHARSET, 0, 0, CLEARTYPE_QUALITY, 0, L"Microsoft YaHei UI");
            HFONT oldN = (HFONT)SelectObject(hdc, hName);
            DrawTextW(hdc, p->name, -1, &nameRc, DT_LEFT | DT_VCENTER | DT_SINGLELINE | DT_END_ELLIPSIS);

            // 3) 分数（右对齐、等宽大字）
            RECT scoreRc = rc;
            scoreRc.left  = nameRc.right + 4;
            scoreRc.right = scoreRc.left + 80;
            scoreRc.top    = nameRc.top;
            scoreRc.bottom = nameRc.bottom;
            COLORREF scoreColor = selected ? RGB(255, 230, 120) : RGB(180, 90, 0);
            SetTextColor(hdc, scoreColor);
            HFONT hScFont = CreateFontW(15, 0, 0, 0, FW_BLACK, FALSE, FALSE, FALSE,
                DEFAULT_CHARSET, 0, 0, CLEARTYPE_QUALITY, DEFAULT_PITCH | FF_MODERN, L"Consolas");
            HFONT oldSc = (HFONT)SelectObject(hdc, hScFont);
            wchar_t scoreBuf[32];
            swprintf_s(scoreBuf, L"%7d", p->score);
            DrawTextW(hdc, scoreBuf, -1, &scoreRc, DT_RIGHT | DT_VCENTER | DT_SINGLELINE);

            SelectObject(hdc, oldSc);
            DeleteObject(hScFont);

            // 4) 日期时间
            RECT dateRc = rc;
            dateRc.left  = scoreRc.right + 6;
            dateRc.right = rc.right - 8;
            dateRc.top    = nameRc.top;
            dateRc.bottom = nameRc.bottom;
            SetTextColor(hdc, selected ? RGB(240, 245, 255) : RGB(90, 105, 120));
            HFONT hD = CreateFontW(12, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE,
                DEFAULT_CHARSET, 0, 0, CLEARTYPE_QUALITY, DEFAULT_PITCH | FF_MODERN, L"Consolas");
            HFONT oldD = (HFONT)SelectObject(hdc, hD);
            DrawTextW(hdc, p->date, -1, &dateRc, DT_RIGHT | DT_VCENTER | DT_SINGLELINE);

            SelectObject(hdc, oldD);
            DeleteObject(hD);
            SelectObject(hdc, oldN);
            DeleteObject(hName);
        }

        // 若获得焦点/选中，绘制一圈 focus 虚线
        if (dis->itemState & ODS_FOCUS) {
            DrawFocusRect(hdc, &rc);
        }
        return (INT_PTR)TRUE;
    }

    case WM_COMMAND: {
        int id = LOWORD(wParam);
        if (id == IDOK || id == IDCANCEL) {
            HWND hList = GetDlgItem(hwndDlg, IDC_LIST_SCORES);
            CleanupLeaderboardList(hList);
            HFONT hF = (HFONT)RemovePropW(hwndDlg, L"LB_FONT");
            if (hF) DeleteObject(hF);
            EndDialog(hwndDlg, id);
            return (INT_PTR)TRUE;
        }
        if (id == IDC_BUTTON_CLEAR) {
            int r = MessageBoxW(hwndDlg,
                L"确定要清空全部排行榜记录吗？\n此操作不可撤销！",
                L"清空排行榜",
                MB_YESNO | MB_ICONWARNING | MB_DEFBUTTON2);
            if (r == IDYES) {
                if (ClearLeaderboard()) {
                    RebuildLeaderboardList(GetDlgItem(hwndDlg, IDC_LIST_SCORES), score,
                                           GetDlgItem(hwndDlg, IDC_STATIC_SCORE_CUR));
                    MessageBoxW(hwndDlg, L"排行榜已清空。", L"完成", MB_OK | MB_ICONINFORMATION);
                } else {
                    MessageBoxW(hwndDlg, L"清空失败，请稍后重试。", L"错误", MB_OK | MB_ICONERROR);
                }
            }
            return (INT_PTR)TRUE;
        }
        break;
    }

    case WM_DESTROY:
        // 兜底清理（若用户通过系统菜单关掉）
        CleanupLeaderboardList(GetDlgItem(hwndDlg, IDC_LIST_SCORES));
        break;
    }
    return (INT_PTR)FALSE;
}

// ============================================================
//  输入名字对话框（graphics.h 声明的导出版）：IDOK/IDCANCEL 仅负责结束对话框，
//  名字由调用方通过 GetDlgItemText 读取更稳妥的做法是由专用过程 NameGetterDlg
//  在过程内完成读取 → 写入共享缓存 → 调用 SetLastEntryName。
// ============================================================
INT_PTR CALLBACK InputNameDialogProc(HWND hwndDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    UNREFERENCED_PARAMETER(lParam);
    switch (uMsg) {
    case WM_INITDIALOG: {
        HWND hEdit = GetDlgItem(hwndDlg, IDC_EDIT_NAME);
        SendMessageW(hEdit, EM_SETLIMITTEXT, MAX_NAME_LEN, 0);
        SetFocus(hEdit);
        return (INT_PTR)FALSE;
    }
    case WM_COMMAND: {
        int id = LOWORD(wParam);
        if (id == IDOK || id == IDCANCEL) {
            EndDialog(hwndDlg, id);
            return (INT_PTR)TRUE;
        }
        break;
    }
    }
    return (INT_PTR)FALSE;
}

// ============================================================
//  新纪录庆祝对话框（导出版，给 DialogBoxParam 用）：
//  lParam 指向 CelebrateParams{rank, score}，在 WM_INITDIALOG 填充三处静态文本
// ============================================================
struct CelebrateParams { int rank; int score; };
INT_PTR CALLBACK NewRecordDialogProc(HWND hwndDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    switch (uMsg) {
    case WM_INITDIALOG: {
        CelebrateParams* p = (CelebrateParams*)lParam;
        if (p) {
            wchar_t rankText[128];
            if      (p->rank == 1) swprintf_s(rankText, L"恭喜获得：🥇 第 1 名！");
            else if (p->rank == 2) swprintf_s(rankText, L"恭喜获得：🥈 第 2 名！");
            else if (p->rank == 3) swprintf_s(rankText, L"恭喜获得：🥉 第 3 名！");
            else                   swprintf_s(rankText, L"✅ 成功进入 Top %d（第 %d 名）！", MAX_SCORES, p->rank);
            SetDlgItemTextW(hwndDlg, IDC_STATIC_RANK, rankText);

            wchar_t st[80];
            swprintf_s(st, L"🎯 分数：%d", p->score);
            SetDlgItemTextW(hwndDlg, IDC_STATIC_SCORE_CUR, st);

            wchar_t cg[120];
            if      (p->rank == 1) swprintf_s(cg, L"🌟 新的最高分！继续挑战自我！");
            else if (p->rank <= 3) swprintf_s(cg, L"🏆 强啊！下次争取拿下第一名！");
            else                   swprintf_s(cg, L"已经很棒啦～再玩一局挑战更高名次吧！");
            SetDlgItemTextW(hwndDlg, IDC_STATIC_CONGRATS, cg);
        }
        return (INT_PTR)TRUE;
    }
    case WM_COMMAND:
        if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL) {
            EndDialog(hwndDlg, LOWORD(wParam));
            return (INT_PTR)TRUE;
        }
        break;
    }
    return (INT_PTR)FALSE;
}

// ============================================================
//  输入名字 + 写回 CSV 的模态封装过程
// ============================================================
static wchar_t s_bufName[MAX_NAME_LEN + 1];
static INT_PTR CALLBACK NameGetterDlgProc(HWND h, UINT m, WPARAM w, LPARAM l)
{
    UNREFERENCED_PARAMETER(l);
    switch (m) {
    case WM_INITDIALOG: {
        SendDlgItemMessageW(h, IDC_EDIT_NAME, EM_SETLIMITTEXT, MAX_NAME_LEN, 0);
        s_bufName[0] = 0;
        HWND he = GetDlgItem(h, IDC_EDIT_NAME);
        if (he) { SetFocus(he); return (INT_PTR)FALSE; }
        return (INT_PTR)TRUE;
    }
    case WM_COMMAND: {
        int id = LOWORD(w);
        if (id == IDOK) {
            GetDlgItemTextW(h, IDC_EDIT_NAME, s_bufName, MAX_NAME_LEN + 1);
            EndDialog(h, IDOK);
            return (INT_PTR)TRUE;
        }
        if (id == IDCANCEL) {
            s_bufName[0] = 0;
            EndDialog(h, IDCANCEL);
            return (INT_PTR)TRUE;
        }
        break;
    }
    }
    return (INT_PTR)FALSE;
}

// ============================================================
//  主入榜流程：提交 → 输名字（跳过=匿名）→ 庆祝
// ============================================================
void TrySubmitAndCelebrate(HWND hwndParent, int curScore)
{
    if (curScore <= 0) return;
    int rank = SubmitScore(curScore);
    if (rank <= 0) return;

    // 1) 弹“输入名字”对话框（IDCANCEL=跳过，使用匿名）
    DialogBox(GetModuleHandle(NULL),
              MAKEINTRESOURCE(IDD_INPUTNAME_DIALOG),
              hwndParent, NameGetterDlgProc);
    SetLastEntryName(s_bufName); // 空串 → 内部默认“匿名玩家”

    // 2) 弹庆祝弹窗（参数化）
    CelebrateParams cp{rank, curScore};
    DialogBoxParamW(GetModuleHandle(NULL),
                    MAKEINTRESOURCE(IDD_NEWRECORD_DIALOG),
                    hwndParent, NewRecordDialogProc, (LPARAM)&cp);
}

// 注册窗口类
ATOM MyRegisterClass(HINSTANCE hInstance)
{
    WNDCLASSEXW wcex = {};

    wcex.cbSize        = sizeof(WNDCLASSEX);
    wcex.style         = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc   = WndProc;
    wcex.cbClsExtra    = 0;
    wcex.cbWndExtra    = 0;
    wcex.hInstance     = hInstance;
    wcex.hIcon         = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_GAME2048));
    wcex.hCursor       = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    wcex.lpszMenuName  = MAKEINTRESOURCEW(IDC_GAME2048);
    wcex.lpszClassName = szWindowClass;
    wcex.hIconSm       = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

    return RegisterClassExW(&wcex);
}

// 保存实例句柄并创建主窗口
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
    hInst = hInstance;

    mainWindow = CreateWindowW(szWindowClass, szTitle,
        WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU,
        CW_USEDEFAULT, CW_USEDEFAULT, WINDOW_WIDTH, WINDOW_HEIGHT,
        nullptr, nullptr, hInstance, nullptr);

    if (!mainWindow)
    {
        return FALSE;
    }

    ShowWindow(mainWindow, nCmdShow);
    UpdateWindow(mainWindow);

    return TRUE;
}

// 主窗口过程：处理菜单、绘制、键盘、销毁等消息
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
    case WM_CREATE:
        centerWindow(hWnd);
        break;
    case WM_COMMAND:
    {
        int wmId = LOWORD(wParam);
        switch (wmId)
        {
        case IDM_ABOUT:
            DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
            break;
        case IDM_LEADERBOARD:
            DialogBox(hInst, MAKEINTRESOURCE(IDD_LEADERBOARD_DIALOG), hWnd, LeaderboardDialogProc);
            break;
        case IDM_NEWGAME:
            // 新游戏前先尝试提交本局成绩 → 入榜则输名字 → 庆祝（失败/0分则静默）
            TrySubmitAndCelebrate(hWnd, score);
            initGameBoard();
            InvalidateRect(hWnd, NULL, TRUE);
            break;
        case IDM_EXIT:
            // 退出前提交本局成绩
            TrySubmitAndCelebrate(hWnd, score);
            DestroyWindow(hWnd);
            break;
        default:
            return DefWindowProc(hWnd, message, wParam, lParam);
        }
    }
    break;
    case WM_PAINT:
    {
        PAINTSTRUCT ps;
        HDC hdc = BeginPaint(hWnd, &ps);
        drawGameBoard(hdc);
        EndPaint(hWnd, &ps);
    }
    break;
    case WM_KEYDOWN:
        processInput(wParam);
        InvalidateRect(hWnd, NULL, TRUE);
        break;
    case WM_DESTROY:
        PostQuitMessage(0);
        break;
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}

// “关于”对话框过程
INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    UNREFERENCED_PARAMETER(lParam);
    switch (message)
    {
    case WM_INITDIALOG:
        return (INT_PTR)TRUE;

    case WM_COMMAND:
        if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
        {
            EndDialog(hDlg, LOWORD(wParam));
            return (INT_PTR)TRUE;
        }
        break;
    }
    return (INT_PTR)FALSE;
}

// 将窗口居中于屏幕
void centerWindow(HWND hwnd) {
    int screenWidth  = GetSystemMetrics(SM_CXSCREEN);
    int screenHeight = GetSystemMetrics(SM_CYSCREEN);

    RECT rect;
    GetWindowRect(hwnd, &rect);
    int windowWidth  = rect.right  - rect.left;
    int windowHeight = rect.bottom - rect.top;

    int posX = (screenWidth  - windowWidth)  / 2;
    int posY = (screenHeight - windowHeight) / 2;

    SetWindowPos(hwnd, NULL, posX, posY, 0, 0, SWP_NOZORDER | SWP_NOSIZE);
}
