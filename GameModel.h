#pragma once
#include "Character.h"
#include "EnemyPool.h"
#include <random>

// ============================================================
// [Model] ゲーム全体の状態とルール
//   ・プレイヤー / 敵 / 戦闘ルールを管理する
//   ・入出力 (cin / cout) には依存しない
// ============================================================

// 戦闘1ターン分の結果
struct BattleTurnResult {
    AttackResult playerAttack;
    bool enemyAttacked = false;   // 敵が反撃したか
    AttackResult enemyAttack;     // enemyAttacked が true のときのみ有効
    bool enemyDefeated = false;
    bool playerDefeated = false;
    ExpResult exp;                // enemyDefeated が true のときのみ有効
};

class GameModel {
private:
    Player player;
    EnemyPool enemyPool;
    Enemy* currentEnemy;
    std::mt19937 rng;

    void EndBattle();

public:
    GameModel();

    // 探索して敵と遭遇する。プールに空きがなければ nullptr
    const Enemy* StartEncounter();

    // 休憩して HP を全回復
    void Rest();

    // プレイヤーが攻撃し、敵が生きていれば反撃する
    BattleTurnResult PlayerAttack();

    // 戦闘から逃げる
    void Flee();

    const Player& GetPlayer() const { return player; }
    const Enemy* GetCurrentEnemy() const { return currentEnemy; }
    bool IsInBattle() const { return currentEnemy != nullptr; }
};