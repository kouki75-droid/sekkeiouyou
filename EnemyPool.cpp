#include "EnemyPool.h"

Enemy* EnemyPool::SpawnEnemy(const EnemyData& data) {
    for (auto& enemy : pool) {
        if (!enemy.IsActive()) {
            enemy.ResetEnemy(data.name, data.maxHp, data.attack, data.expReward);
            return &enemy;
        }
    }
    return nullptr; // ƒv[ƒ‹‹ó‚«‚È‚µ
}