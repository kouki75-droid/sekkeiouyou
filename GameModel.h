#pragma once
#include "Character.h"
#include "EnemyPool.h"
#include <random>
#include <vector>

// ============================================================
// [Model] ゲーム全体の状態とルール
//   ・プレイヤー / 敵 / 戦闘ルールを管理する
//   ・入出力 (cin / cout) には依存しない
// ============================================================

// 戦闘1ターン分の結果
struct BattleTurnResult {
    std::vector<AttackResult> playerAttacks;
    std::string spellName;
    bool insufficientMp = false;
    bool enemyAttacked = false;   // 敵が反撃したか
    AttackResult enemyAttack;     // enemyAttacked が true のときのみ有効
    bool battleWon = false;
    bool playerDefeated = false;
    ExpResult exp;
};

struct FleeResult {
    bool escaped = false;
    AttackResult enemyAttack;
};

class GameModel {
private:
    Player player;
    EnemyPool enemyPool;
    static constexpr int MAX_ENEMIES = 3;
    std::vector<Enemy*> enemies;
    std::mt19937 rng;

    AttackResult EnemyCounterattack(Enemy& enemy);
    void EndBattle();

public:
    GameModel();

    // 探索して敵と遭遇する。戦闘を開始できれば true
    bool StartEncounter();

    // 休憩して HP と MP を全回復
    void Rest();

    // プレイヤーが攻撃し、敵が生きていれば反撃する
    BattleTurnResult PlayerAttack(std::size_t targetIndex, const SpellData* spell = nullptr);

    // 戦闘から逃げる
    FleeResult Flee();

    const Player& GetPlayer() const { return player; }
    const std::vector<Enemy*>& GetEnemies() const { return enemies; }
    bool IsInBattle() const { return !enemies.empty(); }
};