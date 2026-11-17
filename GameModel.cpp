#include "GameModel.h"
#include "GameData.h"
#include <algorithm>

GameModel::GameModel()
    : player("—EŽÒ", JobType::Knight),
    enemyPool(MAX_ENEMIES),
    rng(std::random_device{}()) {
}

bool GameModel::StartEncounter() {
    if (IsInBattle()) return false;

    const auto& data = GameData::GetInstance();
    const int level = player.GetLevel();
    std::vector<const EnemyData*> candidates;
    for (int typeId = 1; typeId <= data.GetEnemyTypeCount(); ++typeId) {
        const EnemyData& enemy = data.GetEnemyData(typeId);
        if (level >= enemy.minLevel) {
            candidates.push_back(&enemy);
        }
    }

    const int minCount = level >= 5 ? 2 : 1;
    const int maxCount = level >= 5 ? MAX_ENEMIES : (level >= 3 ? 2 : 1);
    const int count = std::uniform_int_distribution<int>(minCount, maxCount)(rng);
    enemies.reserve(count);
    for (int i = 0; i < count && !candidates.empty(); ++i) {
        const auto index = std::uniform_int_distribution<std::size_t>(0, candidates.size() - 1)(rng);
        EnemyData enemyData = *candidates[index];
        const int levelsSinceAppearance = level - enemyData.minLevel;
        enemyData.maxHp += levelsSinceAppearance * 6;
        enemyData.attack += levelsSinceAppearance * 2;
        enemyData.expReward += levelsSinceAppearance * 4;

        Enemy* enemy = enemyPool.SpawnEnemy(enemyData);
        if (!enemy) break;
        enemies.push_back(enemy);
        if (enemyData.uniquePerEncounter) {
            candidates.erase(candidates.begin() + index);
        }
    }
    return IsInBattle();
}

void GameModel::Rest() {
    player.FullHeal();
    player.RestoreMp();
}

BattleTurnResult GameModel::PlayerAttack(std::size_t targetIndex, const SpellData* spell) {
    BattleTurnResult result;
    if (targetIndex >= enemies.size()) return result;

    const Enemy* target = enemies[targetIndex];
    const SpellTarget scope = spell ? spell->target : SpellTarget::Single;
    if (spell) {
        if (!player.TrySpendMp(spell->mpCost)) {
            result.insufficientMp = true;
            return result;
        }
        result.spellName = spell->name;
    }

    int expReward = 0;
    Enemy* counterattacker = nullptr;
    for (Enemy* enemy : enemies) {
        const bool hit = scope == SpellTarget::All || enemy == target ||
            (scope == SpellTarget::Group && enemy->GetName() == target->GetName());
        if (!hit) continue;

        result.playerAttacks.push_back(spell ? player.MagicAttack(*enemy, *spell) : player.Attack(*enemy));
        if (!enemy->IsActive()) {
            expReward += enemy->GetExpReward();
        }
        else if (!counterattacker) {
            counterattacker = enemy;
        }
    }

    enemies.erase(std::remove_if(enemies.begin(), enemies.end(),
        [](const Enemy* enemy) { return !enemy->IsActive(); }), enemies.end());
    result.battleWon = enemies.empty();
    if (expReward > 0) {
        result.exp = player.GainExp(expReward);
    }

    if (counterattacker) {
        result.enemyAttacked = true;
        result.enemyAttack = EnemyCounterattack(*counterattacker);
        result.playerDefeated = result.enemyAttack.targetDefeated;
    }
    return result;
}

FleeResult GameModel::Flee() {
    FleeResult result;
    if (enemies.empty()) return result;

    Enemy& strongestEnemy = **std::max_element(enemies.begin(), enemies.end(),
        [](const Enemy* first, const Enemy* second) {
            return first->GetAttackPower() < second->GetAttackPower();
        });

    constexpr double BASE_FLEE_RATE = 0.75;
    constexpr double FLEE_RATE_PER_ATTACK_POINT = 0.01;
    constexpr double MIN_FLEE_RATE = 0.20;
    constexpr double MAX_FLEE_RATE = 0.95;

    const double attackDifference =
        static_cast<double>(player.GetAttackPower()) - strongestEnemy.GetAttackPower();
    const double fleeRate = BASE_FLEE_RATE + attackDifference * FLEE_RATE_PER_ATTACK_POINT;
    const double boundedFleeRate = std::max(MIN_FLEE_RATE, std::min(MAX_FLEE_RATE, fleeRate));
    std::bernoulli_distribution escapeRoll(boundedFleeRate);
    result.escaped = escapeRoll(rng);
    if (result.escaped) {
        EndBattle();
    }
    else {
        result.enemyAttack = EnemyCounterattack(strongestEnemy);
    }
    return result;
}

AttackResult GameModel::EnemyCounterattack(Enemy& enemy) {
    AttackResult result = enemy.Attack(player);
    if (result.targetDefeated) {
        EndBattle();
    }
    return result;
}

void GameModel::EndBattle() {
    for (Enemy* enemy : enemies) {
        enemyPool.DespawnEnemy(enemy);
    }
    enemies.clear();
}