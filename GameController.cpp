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
    controller.GetView().ShowBattleStart(controller.GetModel().GetEnemies().size());
}

// 戦闘の進行
void StateBattle::Update(GameController& controller) {
    GameModel& model = controller.GetModel();
    const GameView& view = controller.GetView();

    view.ShowBattleStatus(model.GetPlayer(), model.GetEnemies());
    view.ShowBattleMenu();
    int choice = view.ReadChoice();

    const SpellData* spell = nullptr;
    if (choice == 2) {
        const auto spells = GameData::GetInstance().GetSpells(model.GetPlayer().GetLevel());
        const int spellCount = static_cast<int>(spells.size());
        while (true) {
            view.ShowSpellMenu(spells, model.GetPlayer().GetAttackPower());
            const int spellChoice = view.ReadChoice();
            if (spellChoice == INPUT_CLOSED) {
                controller.ChangeState(nullptr);
                return;
            }
            if (spellChoice == spellCount + 1) {
                return;
            }
            if (spellChoice >= 1 && spellChoice <= spellCount) {
                spell = spells[spellChoice - 1];
                break;
            }
            view.ShowInvalidInput();
        }
    }

    switch (choice) {
    case 1:
    case 2: {
        const int enemyCount = static_cast<int>(model.GetEnemies().size());
        int targetChoice = 1;
        if (enemyCount > 1 && (!spell || spell->target != SpellTarget::All)) {
            view.ShowTargetPrompt(enemyCount);
            targetChoice = view.ReadChoice();
            if (targetChoice == INPUT_CLOSED) {
                controller.ChangeState(nullptr);
                return;
            }
            if (targetChoice == enemyCount + 1) return;
            if (targetChoice < 1 || targetChoice > enemyCount) {
                view.ShowInvalidInput();
                return;
            }
        }

        BattleTurnResult result = model.PlayerAttack(targetChoice - 1, spell);
        view.ShowBattleTurn(result);

        if (result.battleWon) {
            controller.ChangeState(std::make_unique<StateExplore>());
        }
        else if (result.playerDefeated) {
            controller.ChangeState(std::make_unique<StateGameOver>());
        }
        break;
    }
    case 3: {
        FleeResult result = model.Flee();
        view.ShowFlee(result);
        if (result.escaped) {
            controller.ChangeState(std::make_unique<StateExplore>());
        }
        else if (result.enemyAttack.targetDefeated) {
            controller.ChangeState(std::make_unique<StateGameOver>());
        }
        break;
    }
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