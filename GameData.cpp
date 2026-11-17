#include "GameData.h"
#include <algorithm>

GameData::GameData()
    : spellFamilies{{
        {
            { "メラ", 3, 10, SpellTarget::Single },
            { "メラミ", 5, 24, SpellTarget::Single },
            { "メラゾーマ", 7, 40, SpellTarget::Single },
            { "メラガイアー", 9, 62, SpellTarget::Single }
        },
        {
            { "ギラ", 4, 12, SpellTarget::Group },
            { "ベギラマ", 6, 26, SpellTarget::Group },
            { "ベギラゴン", 8, 44, SpellTarget::Group },
            { "ギラグレイド", 11, 72, SpellTarget::Group }
        },
        {
            { "ヒャド", 5, 18, SpellTarget::Group },
            { "ヒャダルコ", 7, 32, SpellTarget::Group },
            { "ヒャダイン", 9, 50, SpellTarget::Group },
            { "マヒャデドス", 11, 72, SpellTarget::Group }
        },
        {
            { "イオ", 6, 26, SpellTarget::All },
            { "イオラ", 8, 40, SpellTarget::All },
            { "イオナズン", 10, 58, SpellTarget::All },
            { "イオグランデ", 12, 80, SpellTarget::All }
        },
        {
            { "デイン", 7, 36, SpellTarget::All },
            { "ライデイン", 9, 50, SpellTarget::All },
            { "ギガデイン", 11, 68, SpellTarget::All },
            { "ミナデイン", 13, 90, SpellTarget::All }
        },
        {
            { "ドルマ", 8, 48, SpellTarget::All },
            { "ドルクマ", 10, 62, SpellTarget::All },
            { "ドルモーア", 12, 80, SpellTarget::All },
            { "ドルマドン", 14, 102, SpellTarget::All }
        },
        {
            { "バギ", 6, 30, SpellTarget::Group },
            { "バギマ", 8, 44, SpellTarget::Group },
            { "バギクロス", 10, 62, SpellTarget::Group },
            { "バギムーチョ", 12, 84, SpellTarget::Group }
        }
    }} {
    // 名前、最大HP、攻撃力、経験値、出現レベル、1戦に1体まで
    enemyTable[1] = { "スライム", 30, 8, 15, 1 };
    enemyTable[2] = { "オーク", 60, 14, 35, 3 };
    enemyTable[3] = { "ドラゴン", 180, 32, 120, 7, true };
}

// EnemyDataを取得するメソッド
const EnemyData& GameData::GetEnemyData(int typeId) const {
    return enemyTable.at(typeId);
}
GameData::SpellList GameData::GetSpells(int playerLevel) const {
    constexpr int LEVELS_PER_UPGRADE = 2;
    const std::size_t tier = (std::max(1, playerLevel) - 1) / LEVELS_PER_UPGRADE;
    SpellList availableSpells{};
    for (std::size_t i = 0; i < spellFamilies.size(); ++i) {
        const auto& family = spellFamilies[i];
        availableSpells[i] = &family[std::min(tier, family.size() - 1)];
    }
    return availableSpells;
}
