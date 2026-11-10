#include "EnemyPool.h"
#include "GameData.h"

Enemy* EnemyPool::SpawnEnemy(int enemyTypeId) {
    const EnemyData& data = GameData::GetInstance().GetEnemyData(enemyTypeId);
    for (auto& enemy : pool) {
        if (!enemy.IsActive()) {
            enemy.ResetEnemy(data.name, data.maxHp, data.attack, data.expReward);
            return &enemy;
        }
    }
    return nullptr; // ƒv[ƒ‹‹ó‚«‚È‚µ
}