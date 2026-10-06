#pragma once
#include <string>
#include <unordered_map>

// 敵キャラクターのマスターデータ構造 (DataTable)
struct EnemyData {
    std::string name;
    int maxHp;
    int attack;
    int expReward;
};

// ゲーム全体の固定データを保持するシングルトン (Singleton)
class GameData {
private:
    GameData(); // 外部からのインスタンス化を禁止
    std::unordered_map<int, EnemyData> enemyTable;

public:
    static GameData& GetInstance() {
        static GameData instance;
        return instance;
    }

    // コピー・代入の禁止
    GameData(const GameData&) = delete;
    GameData& operator=(const GameData&) = delete;

    const EnemyData& GetEnemyData(int typeId) const;
};
