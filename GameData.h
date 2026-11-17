
#pragma once
#include <array>
#include <string>
#include <unordered_map>
#include <vector>

// ============================================================
// [Model] マスターデータ
// ============================================================

// 敵キャラクターのマスターデータ構造 (DataTable)
struct EnemyData {
    std::string name;
    int maxHp;
    int attack;
    int expReward;
    int minLevel;
    bool uniquePerEncounter = false;
};

enum class SpellTarget { Single, Group, All };

struct SpellData {
    std::string name;
    int mpCost;
    int bonusDamage;
    SpellTarget target;
};

// ゲーム全体の固定データを保持するシングルトン (Singleton)
class GameData {
public:
    using SpellList = std::array<const SpellData*, 7>;
private:
    GameData(); // 外部からのインスタンス化を禁止
    std::unordered_map<int, EnemyData> enemyTable;
    const std::array<std::vector<SpellData>, 7> spellFamilies;

public:
    static GameData& GetInstance() {
        static GameData instance;
        return instance;
    }

    // コピー・代入の禁止
    GameData(const GameData&) = delete;
    GameData& operator=(const GameData&) = delete;

    const EnemyData& GetEnemyData(int typeId) const;
    SpellList GetSpells(int playerLevel) const;

    // 登録されている敵の種類数（ID は 1 から連番）
    int GetEnemyTypeCount() const { return static_cast<int>(enemyTable.size()); }
};