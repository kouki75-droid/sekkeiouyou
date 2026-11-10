#pragma once
#include "Character.h"
#include "GameModel.h"

// ============================================================
// [View] 画面表示と入力受付
//   ・std::cout / std::cin を扱うのはこのクラスだけ
//   ・Model を読み取るだけで、状態は変更しない
// ============================================================

// ReadChoice() の特殊な戻り値
const int INPUT_INVALID = -1; // 数値以外が入力された
const int INPUT_CLOSED = 0;   // 入力が終了した (EOF)

class GameView {
public:
    // 探索
    void ShowExploreStart() const;
    void ShowExploreMenu() const;
    void ShowRest(const Player& player) const;
    void ShowNoEnemyAvailable() const;

    // 戦闘
    void ShowBattleStart(const Enemy& enemy) const;
    void ShowBattleStatus(const Player& player, const Enemy& enemy) const;
    void ShowBattleMenu() const;
    void ShowBattleTurn(const BattleTurnResult& result) const;
    void ShowFlee() const;

    // ゲームオーバー
    void ShowGameOver() const;

    // 共通
    void ShowInvalidInput() const;
    int ReadChoice() const;

private:
    void ShowAttack(const AttackResult& result) const;
    void ShowExp(const ExpResult& result, const std::string& playerName) const;
};