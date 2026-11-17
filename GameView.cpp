#include "GameView.h"
#include <iostream>
#include <limits>

// ---------- 探索 ----------
void GameView::ShowExploreStart() const {
    std::cout << "\n========================================\n";
    std::cout << "ダンジョンを探索中...\n";
}

void GameView::ShowExploreMenu() const {
    std::cout << "1: 進む   2: 休む (HP・MP回復)   3: ゲーム終了\n> ";
}

void GameView::ShowRest(const Player& player) const {
    std::cout << "休憩してHPとMPが全回復した！ (HP: " << player.GetHp()
        << " / " << player.GetMaxHp() << ", MP: " << player.GetMp()
        << " / " << player.GetMaxMp() << ")\n";
}

void GameView::ShowNoEnemyAvailable() const {
    std::cout << "辺りは静まり返っている...\n";
}

// ---------- 戦闘 ----------
void GameView::ShowBattleStart(std::size_t enemyCount) const {
    std::cout << "\n--- 戦闘開始！ 敵が " << enemyCount << " 体現れた！ ---\n";
}

void GameView::ShowBattleStatus(const Player& player, const std::vector<Enemy*>& enemies) const {
    std::cout << "\n[ Player HP: " << player.GetHp()
        << " MP: " << player.GetMp() << " / " << player.GetMaxMp() << " ]\n";
    for (std::size_t i = 0; i < enemies.size(); ++i) {
        const Enemy& enemy = *enemies[i];
        std::cout << i + 1 << ": " << enemy.GetName() << " (HP: " << enemy.GetHp() << ")\n";
    }
}

void GameView::ShowTargetPrompt(int enemyCount) const {
    std::cout << "攻撃する敵の番号を選んでください (1～" << enemyCount
        << "、" << enemyCount + 1 << ": 戻る)\n> ";
}

void GameView::ShowBattleMenu() const {
    std::cout << "1: 攻撃   2: 魔法   3: 逃げる\n> ";
}

void GameView::ShowSpellMenu(const GameData::SpellList& spells, int attackPower) const {
    std::cout << "\n使う魔法を選んでください。\n";
    for (size_t i = 0; i < spells.size(); ++i) {
        const SpellData& spell = *spells[i];
        const char* scope = "単体";
        if (spell.target == SpellTarget::Group) scope = "グループ";
        if (spell.target == SpellTarget::All) scope = "全体";
        std::cout << i + 1 << ": " << spell.name << " (" << scope << " / MP " << spell.mpCost
            << " / ダメージ " << attackPower + spell.bonusDamage << ")\n";
    }
    std::cout << spells.size() + 1 << ": 戻る\n> ";
}

void GameView::ShowBattleTurn(const BattleTurnResult& result) const {
    if (result.insufficientMp) {
        std::cout << "MPが足りない！\n";
        return;
    }

    if (result.playerAttacks.empty()) return;

    const std::string& attackerName = result.playerAttacks.front().attackerName;
    if (result.spellName.empty()) {
        ShowAttack(result.playerAttacks.front());
    }
    else {
        std::cout << attackerName << " は " << result.spellName << " を唱えた！\n";
        for (const AttackResult& attack : result.playerAttacks) {
            ShowDamage(attack);
        }
    }

    if (result.battleWon) {
        std::cout << "敵をすべて倒した！勝利！\n";
    }
    if (result.exp.gainedExp > 0) {
        ShowExp(result.exp, attackerName);
    }
    if (result.enemyAttacked) {
        ShowAttack(result.enemyAttack);
    }
}

void GameView::ShowFlee(const FleeResult& result) const {
    if (result.escaped) {
        std::cout << "逃げ出した！\n";
        return;
    }

    std::cout << "しかしまわりこまれてしまった！\n";
    ShowAttack(result.enemyAttack);
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
    std::cout << result.attackerName << " の攻撃！ ";
    ShowDamage(result);
}

void GameView::ShowDamage(const AttackResult& result) const {
    std::cout << result.targetName
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