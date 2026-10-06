#include "GameState.h"
#include "GameData.h"
#include <iostream>
#include <cstdlib>

//  EnemyPool 
Enemy* EnemyPool::SpawnEnemy(int enemyTypeId) {
    const EnemyData& data = GameData::GetInstance().GetEnemyData(enemyTypeId);
    for (auto& enemy : pool) {
        if (!enemy.IsActive()) {
            enemy.ResetEnemy(data.name, data.maxHp, data.attack, data.expReward);
            return &enemy;
        }
    }
    return nullptr; // プール空きなし
}

// GameStateManager 
GameStateManager::GameStateManager() : player("勇者", 1), enemyPool(3), currentEnemy(nullptr) {}

void GameStateManager::ChangeState(std::unique_ptr<GameState> newState) {
    currentState = std::move(newState);
    if (currentState) {                // null チェックを追加
        currentState->Enter(*this);
    }
}

void GameStateManager::Run() {
    ChangeState(std::make_unique<StateExplore>());
    while (currentState) {
        currentState->Update(*this);
    }
}

// StateExplore 
void StateExplore::Enter(GameStateManager& manager) {
    std::cout << "\n========================================\n";
    std::cout << "ダンジョンを探索中...\n";
}

void StateExplore::Update(GameStateManager& manager) {
    std::cout << "1: 進む   2: 休む (HP回復)   3: ゲーム終了\n> ";
    int choice;
    std::cin >> choice;

    if (choice == 1) {
        int enemyType = (rand() % 3) + 1; // 1~3の敵をランダム出現
        Enemy* enemy = manager.GetEnemyPool().SpawnEnemy(enemyType);
        if (enemy) {
            manager.SetCurrentEnemy(enemy);
            manager.ChangeState(std::make_unique<StateBattle>());
        }
    }
    else if (choice == 2) {
        std::cout << "休憩してHPが全回復した！\n";
        manager.GetPlayer().Reset(manager.GetPlayer().GetName(), 100, 15);
    }
    else {
        manager.ChangeState(nullptr); // ループ終了
    }
}

//StateBattle
void StateBattle::Enter(GameStateManager& manager) {
    std::cout << "\n--- 戦闘開始！ " << manager.GetCurrentEnemy()->GetName() << " が現れた！ ---\n";
}

void StateBattle::Update(GameStateManager& manager) {
    Player& player = manager.GetPlayer();
    Enemy* enemy = manager.GetCurrentEnemy();

    std::cout << "\n[ Player HP: " << player.GetHp() << " ] VS [ " << enemy->GetName() << " HP: " << enemy->GetHp() << " ]\n";
    std::cout << "1: 攻撃   2: 逃げる\n> ";
    int choice;
    std::cin >> choice;

    if (choice == 1) {
        player.Attack(*enemy);
        if (enemy->IsActive()) {
            enemy->Attack(player);
        }
        else {
			// 敵を倒した場合の処理
            std::cout << enemy->GetName() << " を倒した！勝利！\n";

            // プレイヤーに経験値を付与
            player.GainExp(enemy->GetExpReward());

            manager.GetEnemyPool().DespawnEnemy(enemy);
            manager.ChangeState(std::make_unique<StateExplore>());
            return;
        }

        if (!player.IsActive()) {
            manager.ChangeState(std::make_unique<StateGameOver>());
        }
    }
    else {
        std::cout << "逃げ出した！\n";
        manager.GetEnemyPool().DespawnEnemy(enemy);
        manager.ChangeState(std::make_unique<StateExplore>());
    }
}

// StateGameOver
void StateGameOver::Enter(GameStateManager& manager) {
    std::cout << "\n========================================\n";
    std::cout << "GAME OVER...\n";
    std::cout << "========================================\n";
}

void StateGameOver::Update(GameStateManager& manager) {
    manager.ChangeState(nullptr); // ゲーム終了
}