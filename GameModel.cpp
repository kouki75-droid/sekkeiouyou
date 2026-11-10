#include "GameModel.h"
#include "GameData.h"

GameModel::GameModel()
    : player("—EŽÒ", JobType::Knight),
    enemyPool(3),
    currentEnemy(nullptr),
    rng(std::random_device{}()) {
}

const Enemy* GameModel::StartEncounter() {
    std::uniform_int_distribution<int> dist(1, GameData::GetInstance().GetEnemyTypeCount());
    int enemyType = dist(rng); // 1~3‚Ì“G‚ðƒ‰ƒ“ƒ_ƒ€oŒ»
    currentEnemy = enemyPool.SpawnEnemy(enemyType);
    return currentEnemy;
}

void GameModel::Rest() {
    player.FullHeal();
}

BattleTurnResult GameModel::PlayerAttack() {
    BattleTurnResult result;
    if (!currentEnemy) return result;

    result.playerAttack = player.Attack(*currentEnemy);

    if (result.playerAttack.targetDefeated) {
        // “G‚ð“|‚µ‚½ê‡FŒoŒ±’l‚ð•t—^‚µ‚Äí“¬I—¹
        result.enemyDefeated = true;
        result.exp = player.GainExp(currentEnemy->GetExpReward());
        EndBattle();
        return result;
    }

    // “G‚Ì”½Œ‚
    result.enemyAttacked = true;
    result.enemyAttack = currentEnemy->Attack(player);
    result.playerDefeated = result.enemyAttack.targetDefeated;
    if (result.playerDefeated) {
        EndBattle();
    }
    return result;
}

void GameModel::Flee() {
    EndBattle();
}

void GameModel::EndBattle() {
    enemyPool.DespawnEnemy(currentEnemy);
    currentEnemy = nullptr;
}