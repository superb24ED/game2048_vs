#pragma once
#ifndef __LEADERBOARD_H__
#define __LEADERBOARD_H__

#include <windows.h>
#include <vector>

#define MAX_SCORES     10
#define MAX_NAME_LEN   15   // 玩家名最多 15 个字符（含结尾 '\0' 前）

struct ScoreEntry {
    int     score;
    wchar_t name[MAX_NAME_LEN + 1]; // 玩家名（UTF-16）
    wchar_t date[24];               // 格式：YYYY-MM-DD HH:MM
};

// 加载排行榜（按分数降序，最多 MAX_SCORES 条）
std::vector<ScoreEntry> LoadLeaderboard();

// 提交分数（仅保存“姓名+时间”占位，返回入榜名次 1..MAX_SCORES，未入榜返回 0）
// 需要在调用后用 SetEntryName(index) 更新真实姓名；若用户跳过则已存为“匿名玩家”
int  SubmitScore(int score);

// 为最后一次 SubmitScore 入榜的条目（通过内部索引）设置玩家名；空串 == 使用默认
void SetLastEntryName(const wchar_t* name);

// 清空全部排行榜（删除 CSV 文件），返回是否成功
bool ClearLeaderboard();

#endif
