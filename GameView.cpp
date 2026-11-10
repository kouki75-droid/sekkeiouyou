#include "GameView.h"
#include <iostream>
#include <limits>

// ---------- 探索 ----------
void GameView::ShowExploreStart() const {
    std::cout << "\n========================================\n";
    std::cout << "ダンジョンを探索中...\n";
}

void GameView::ShowExploreMenu() const {
    std::cout << "1: 進む   2: 休む (HP回復)   3: ゲーム終了\n> ";
}

void GameView::ShowRest(const Player& player) const {
    std::cout << "休憩してHPが全回復した！ (HP: " << player.GetHp() << " / " << player.GetMaxHp() << ")\n";
}

void GameView::ShowNoEnemyAvailable() const {
    std::cout << "辺りは静まり返っている...\n";
}

// ---------- 戦闘 ----------
void GameView::ShowBattleStart(const Enemy& enemy) const {
    std::cout << "\n--- 戦闘開始！ " << enemy.GetName() << " が現れた！ ---\n";
}

void GameView::ShowBattleStatus(const Player& player, const Enemy& enemy) const {
    std::cout << "\n[ Player HP: " << player.GetHp() << " ] VS [ "
        << enemy.GetName() << " HP: " << enemy.GetHp() << " ]\n";
}

void GameView::ShowBattleMenu() const {
    std::cout << "1: 攻撃   2: 逃げる\n> ";
}

void GameView::ShowBattleTurn(const BattleTurnResult& result) const {
    ShowAttack(result.playerAttack);

    if (result.enemyDefeated) {
        std::cout << result.playerAttack.targetName << " を倒した！勝利！\n";
        ShowExp(result.exp, result.playerAttack.attackerName);
        return;
    }

    if (result.enemyAttacked) {
        ShowAttack(result.enemyAttack);
    }
}

void GameView::ShowFlee() const {
    std::cout << "逃げ出した！\n";
}

// ---------- ゲームオーバー ----------
void GameView::ShowGameOver() const {
    std::cout << "\n========================================\n";
    std::cout << "GAME OVER...\n";
    std::cout << "========================================\n";
}

// ---------- 共通 ----------
void GameView::ShowInvalidInput() const {
    std::cout << "無効な入力です。もう一度選んでください。\n";
}

int GameView::ReadChoice() const {
    int choice;
    if (std::cin >> choice) {
        return choice;
    }
    if (std::cin.eof()) {
        return INPUT_CLOSED;
    }
    // 数値以外が入力された場合は入力ストリームを復旧する
    std::cin.clear();
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    return INPUT_INVALID;
}

// ---------- 内部ヘルパー ----------
void GameView::ShowAttack(const AttackResult& result) const {
    std::cout << result.attackerName << " の攻撃！ " << result.targetName
        << " に " << result.damage << " のダメージ！\n";
    if (result.targetDefeated) {
        std::cout << result.targetName << " は倒れた！\n";
    }
}

void GameView::ShowExp(const ExpResult& result, const std::string& playerName) const {
    std::cout << result.gainedExp << " ポイントの経験値を獲得！ ("
        << result.currentExp << " / " << result.nextLevelExp << ")\n";

    for (const LevelUpInfo& info : result.levelUps) {
        std::cout << "\n★ LEVEL UP! ★\n";
        std::cout << playerName << " は レベル " << info.newLevel << " に上がった！\n";
        std::cout << "最大HP +" << info.hpIncrease << " (現在: " << info.newMaxHp << ")\n";
        std::cout << "攻撃力 +" << info.atkIncrease << " (現在: " << info.newAttack << ")\n\n";
    }
}