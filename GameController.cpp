#include "GameController.h"

// ---------- GameController ----------
GameController::GameController() : stateChangeRequested(false) {}

void GameController::ChangeState(std::unique_ptr<GameState> newState) {
    nextState = std::move(newState);
    stateChangeRequested = true;
}

void GameController::ApplyStateChange() {
    stateChangeRequested = false;
    currentState = std::move(nextState);
    if (currentState) {
        currentState->Enter(*this);
    }
}

// メインループ
void GameController::Run() {
    ChangeState(std::make_unique<StateExplore>());
    ApplyStateChange();
    while (currentState) {
        currentState->Update(*this);
        if (stateChangeRequested) {
            ApplyStateChange();
        }
    }
}

// ---------- StateExplore ----------
void StateExplore::Enter(GameController& controller) {
    controller.GetView().ShowExploreStart();
}

// プレイヤーの選択肢を表示し、入力を受け付ける
void StateExplore::Update(GameController& controller) {
    GameModel& model = controller.GetModel();
    const GameView& view = controller.GetView();

    view.ShowExploreMenu();
    int choice = view.ReadChoice();

    switch (choice) {
    case 1:
        if (model.StartEncounter()) {
            controller.ChangeState(std::make_unique<StateBattle>());
        }
        else {
            view.ShowNoEnemyAvailable();
        }
        break;
    case 2:
        model.Rest();
        view.ShowRest(model.GetPlayer());
        break;
    case 3:
    case INPUT_CLOSED:
        controller.ChangeState(nullptr); // ループ終了
        break;
    default:
        view.ShowInvalidInput();
        break;
    }
}

// ---------- StateBattle ----------
void StateBattle::Enter(GameController& controller) {
    controller.GetView().ShowBattleStart(*controller.GetModel().GetCurrentEnemy());
}

// 戦闘の進行
void StateBattle::Update(GameController& controller) {
    GameModel& model = controller.GetModel();
    const GameView& view = controller.GetView();

    view.ShowBattleStatus(model.GetPlayer(), *model.GetCurrentEnemy());
    view.ShowBattleMenu();
    int choice = view.ReadChoice();

    switch (choice) {
    case 1: {
        BattleTurnResult result = model.PlayerAttack();
        view.ShowBattleTurn(result);

        if (result.enemyDefeated) {
            controller.ChangeState(std::make_unique<StateExplore>());
        }
        else if (result.playerDefeated) {
            controller.ChangeState(std::make_unique<StateGameOver>());
        }
        break;
    }
    case 2:
        model.Flee();
        view.ShowFlee();
        controller.ChangeState(std::make_unique<StateExplore>());
        break;
    case INPUT_CLOSED:
        controller.ChangeState(nullptr); // 入力終了
        break;
    default:
        view.ShowInvalidInput();
        break;
    }
}

// ---------- StateGameOver ----------
void StateGameOver::Enter(GameController& controller) {
    controller.GetView().ShowGameOver();
}

void StateGameOver::Update(GameController& controller) {
    controller.ChangeState(nullptr); // ゲーム終了
}