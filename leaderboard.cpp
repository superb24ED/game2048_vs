#include "leaderboard.h"

#include <algorithm>
#include <cstdio>
#include <string>

namespace {

// 跟踪最后一次 SubmitScore 入榜后该条目在文件中的位置（按分数降序索引，0-based）。
// -1 表示最近一次没入榜。
int g_lastInsertedIndex = -1;

const wchar_t* kDefaultName = L"匿名玩家";

std::wstring GetLeaderboardFilePath()
{
    wchar_t pathBuf[MAX_PATH] = {};
    DWORD len = GetEnvironmentVariableW(L"LOCALAPPDATA", pathBuf, MAX_PATH);
    if (len == 0 || len >= MAX_PATH) {
        return L"scores.csv";
    }
    std::wstring dir(pathBuf, len);
    dir += L"\\Game2048";
    CreateDirectoryW(dir.c_str(), nullptr);
    return dir + L"\\scores.csv";
}

std::wstring GenerateDateString()
{
    SYSTEMTIME st{};
    GetLocalTime(&st);
    wchar_t buf[24];
    swprintf_s(buf, L"%04d-%02d-%02d %02d:%02d",
        st.wYear, st.wMonth, st.wDay, st.wHour, st.wMinute);
    return std::wstring(buf);
}

} // namespace

std::vector<ScoreEntry> LoadLeaderboard()
{
    std::vector<ScoreEntry> list;
    std::wstring path = GetLeaderboardFilePath();

    FILE* fp = nullptr;
    if (_wfopen_s(&fp, path.c_str(), L"r, ccs=UTF-8") != 0 || !fp) {
        return list;
    }

    wchar_t line[256];
    while (fgetws(line, (int)_countof(line), fp)) {
        // 新格式：name,score,date
        // 旧格式：score,date（无姓名）
        wchar_t name[MAX_NAME_LEN + 1] = {};
        int score = 0;
        wchar_t date[24] = {};

        int matched = swscanf_s(
            line, L"%15[^,\r\n\t],%d,%23[^\r\n]",
            name, (unsigned)_countof(name),
            &score,
            date, (unsigned)_countof(date));

        if (matched == 3) {
            // 新格式（name,score,date）→ 正常
        } else if (matched == 0) {
            // 可能是“score,date”旧格式：前面第一个字段是数字起始
            matched = swscanf_s(
                line, L"%d,%23[^\r\n]",
                &score,
                date, (unsigned)_countof(date));
            if (matched == 2) {
                wcsncpy_s(name, kDefaultName, _TRUNCATE);
            }
        }

        if ((matched == 3 || matched == 2) && score > 0) {
            ScoreEntry e{};
            e.score = score;
            wcsncpy_s(e.name, name[0] ? name : kDefaultName, _TRUNCATE);
            wcsncpy_s(e.date, date, _TRUNCATE);
            list.push_back(e);
        }
    }
    fclose(fp);

    std::sort(list.begin(), list.end(), [](const ScoreEntry& a, const ScoreEntry& b) {
        return a.score > b.score;
    });
    if (list.size() > MAX_SCORES)
        list.resize(MAX_SCORES);
    return list;
}

int SubmitScore(int score)
{
    g_lastInsertedIndex = -1;
    if (score <= 0)
        return 0;

    std::vector<ScoreEntry> list = LoadLeaderboard();

    bool qualifies = false;
    if ((int)list.size() < MAX_SCORES)
        qualifies = true;
    else if (!list.empty() && score > list.back().score)
        qualifies = true;

    if (!qualifies)
        return 0;

    ScoreEntry newEntry{};
    newEntry.score = score;
    wcsncpy_s(newEntry.name, kDefaultName, _TRUNCATE);
    std::wstring dateStr = GenerateDateString();
    wcsncpy_s(newEntry.date, dateStr.c_str(), _TRUNCATE);
    list.push_back(newEntry);

    std::sort(list.begin(), list.end(), [](const ScoreEntry& a, const ScoreEntry& b) {
        return a.score > b.score;
    });
    if (list.size() > MAX_SCORES)
        list.resize(MAX_SCORES);

    // 找到新插入条目的位置（0-based）：首个 name == 默认匿名玩家 且 分数相同 且 日期相同的条目
    int rank = -1;
    for (size_t i = 0; i < list.size(); i++) {
        if (list[i].score == score &&
            wcscmp(list[i].name, kDefaultName) == 0 &&
            wcscmp(list[i].date, dateStr.c_str()) == 0) {
            rank = (int)i + 1;
            g_lastInsertedIndex = (int)i;
            break;
        }
    }
    // 找不到匹配（例如同分匿名条目）：退而求其次找到分数匹配的第一个位置
    if (rank < 0) {
        for (size_t i = 0; i < list.size(); i++) {
            if (list[i].score == score) {
                rank = (int)i + 1;
                g_lastInsertedIndex = (int)i;
                break;
            }
        }
    }

    // 写回
    std::wstring path = GetLeaderboardFilePath();
    FILE* fp = nullptr;
    if (_wfopen_s(&fp, path.c_str(), L"w, ccs=UTF-8") != 0 || !fp) {
        return 0;
    }
    for (const auto& e : list) {
        fwprintf(fp, L"%s,%d,%s\n", e.name, e.score, e.date);
    }
    fclose(fp);

    return (rank > 0) ? rank : 0;
}

void SetLastEntryName(const wchar_t* name)
{
    if (g_lastInsertedIndex < 0)
        return;

    std::wstring displayName = (name && name[0]) ? name : kDefaultName;

    std::vector<ScoreEntry> list = LoadLeaderboard();
    if (g_lastInsertedIndex >= 0 && g_lastInsertedIndex < (int)list.size()) {
        wcsncpy_s(list[g_lastInsertedIndex].name, displayName.c_str(), _TRUNCATE);
    } else {
        // 索引不匹配：遍历找到“同名=匿名 且 分数与之前写入一致”的第一个位置兜底
        // 这里仅使用默认名覆盖以防万一，实际多数情况下 g_lastInsertedIndex 有效
        g_lastInsertedIndex = -1;
    }

    std::wstring path = GetLeaderboardFilePath();
    FILE* fp = nullptr;
    if (_wfopen_s(&fp, path.c_str(), L"w, ccs=UTF-8") != 0 || !fp) {
        g_lastInsertedIndex = -1;
        return;
    }
    for (const auto& e : list) {
        fwprintf(fp, L"%s,%d,%s\n", e.name, e.score, e.date);
    }
    fclose(fp);
    g_lastInsertedIndex = -1;
}

bool ClearLeaderboard()
{
    g_lastInsertedIndex = -1;
    std::wstring path = GetLeaderboardFilePath();
    // 直接删除文件即可；下次 Load 会返回空、Submit 会重新生成
    return (DeleteFileW(path.c_str()) != FALSE) ||
           (GetLastError() == ERROR_FILE_NOT_FOUND);
}
