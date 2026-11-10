#pragma once
#include "GameModel.h"
#include "GameView.h"
#include <memory>

// ============================================================
// [Controller] 入力を受けて Model を操作し、結果を View に渡す
//   ・画面遷移は State パターン (FSM) で管理する
// ============================================================

class GameController;

// 状態の基底インターフェース
class GameState {
public:
    virtual ~GameState() = default;
    virtual void Enter(GameController& controller) = 0;
    virtual void Update(GameController& controller) = 0;
};

// FSM管理者 兼 コントローラー
class GameController {
private:
    GameModel model;
    GameView view;
    std::unique_ptr<GameState> currentState;
    std::unique_ptr<GameState> nextState;
    bool stateChangeRequested;

    void ApplyStateChange();

public:
    GameController();
    void Run();

    // 状態遷移を予約する（nullptr でゲーム終了）
    // 実際の切り替えは Update 終了後に行うので、Update 中に呼んでも安全
    void ChangeState(std::unique_ptr<GameState> newState);

    GameModel& GetModel() { return model; }
    const GameView& GetView() const { return view; }
};

// 各状態クラス
class StateExplore : public GameState {
public:
    void Enter(GameController& controller) override;
    void Update(GameController& controller) override;
};

class StateBattle : public GameState {
public:
    void Enter(GameController& controller) override;
    void Update(GameController& controller) override;
};

class StateGameOver : public GameState {
public:
    void Enter(GameController& controller) override;
    void Update(GameController& controller) override;
};