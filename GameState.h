#pragma once
#include "Character.h"
#include <memory>

enum class StateType {
    Explore,
    Battle,
    GameOver,
    Victory,
    Exit
};

class GameStateManager;

// 状態の基底インターフェース
class GameState {
public:
    virtual ~GameState() = default;
    virtual void Enter(GameStateManager& manager) = 0;
    virtual void Update(GameStateManager& manager) = 0;
};

// FSM管理者
class GameStateManager {
private:
    std::unique_ptr<GameState> currentState;
    Player player;
    EnemyPool enemyPool;
    Enemy* currentEnemy;

public:
    GameStateManager();
    void ChangeState(std::unique_ptr<GameState> newState);
    void Run();

    Player& GetPlayer() { return player; }
    EnemyPool& GetEnemyPool() { return enemyPool; }
    void SetCurrentEnemy(Enemy* enemy) { currentEnemy = enemy; }
    Enemy* GetCurrentEnemy() { return currentEnemy; }
};

// 各状態クラス
class StateExplore : public GameState {
public:
    void Enter(GameStateManager& manager) override;
    void Update(GameStateManager& manager) override;
};

class StateBattle : public GameState {
public:
    void Enter(GameStateManager& manager) override;
    void Update(GameStateManager& manager) override;
};

class StateGameOver : public GameState {
public:
    void Enter(GameStateManager& manager) override;
    void Update(GameStateManager& manager) override;
};
