#include "GameData.h"

GameData::GameData() {
    // ID -> { 名前, 最大HP, 攻撃力, 獲得経験値 }
    enemyTable[1] = { "スライム", 30, 8, 15 };
    enemyTable[2] = { "オーク", 60, 14, 35 };
    enemyTable[3] = { "ドラゴン", 120, 22, 100 };
}

const EnemyData& GameData::GetEnemyData(int typeId) const {
    return enemyTable.at(typeId);
}