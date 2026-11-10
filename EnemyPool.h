#pragma once
#include "Character.h"
#include <vector>
#include <cstddef>

// ============================================================
// [Model] エネミー専用のオブジェクトプール (Object Pool)
// ============================================================
class EnemyPool {
private:
    std::vector<Enemy> pool;

public:
    explicit EnemyPool(std::size_t size) : pool(size) {}

    // プールから未使用の敵を取得・初期化（空きがなければ nullptr）
    Enemy* SpawnEnemy(int enemyTypeId);

    // 敵をプールに返却（非アクティブ化して再利用可能にする）
    void DespawnEnemy(Enemy* enemy) {
        if (enemy) {
            enemy->Deactivate();
        }
    }
};