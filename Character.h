
#pragma once
#include "GameData.h"
#include <string>
#include <vector>

// ============================================================
// [Model] キャラクター関連
//   ・画面出力 (std::cout) は一切行わない
//   ・処理結果は構造体で返し、表示は View に任せる
// ============================================================

// 攻撃1回分の結果
struct AttackResult {
    std::string attackerName;
    std::string targetName;
    int damage = 0;
    bool targetDefeated = false;  // この攻撃で相手が倒れたか
};

// レベルアップ1回分の情報
struct LevelUpInfo {
    int newLevel = 0;
    int hpIncrease = 0;
    int atkIncrease = 0;
    int newMaxHp = 0;
    int newAttack = 0;
};

// 経験値獲得の結果
struct ExpResult {
    int gainedExp = 0;
    int currentExp = 0;    // 獲得直後(レベルアップ判定前)の経験値
    int nextLevelExp = 0;  // 獲得直後の必要経験値
    std::vector<LevelUpInfo> levelUps;
};

// キャラクター基底クラス
class Character {
protected:
    std::string name;
    int hp;
    int maxHp;
    int attack;
    bool active;

public:
    // コンストラクタで初期値を明確に指定する
    Character(const std::string& n, int h, int a)
        : name(n), hp(h), maxHp(h), attack(a), active(true) {
    }
    virtual ~Character() = default;

    virtual AttackResult Attack(Character& target) {
        return DealDamage(target, attack);
    }

protected:
    AttackResult DealDamage(Character& target, int damage) {
        AttackResult result;
        result.attackerName = name;
        result.targetName = target.GetName();
        result.damage = damage;
        result.targetDefeated = target.TakeDamage(damage);
        return result;
    }

public:

    // ダメージを受ける。この攻撃で倒れた場合 true を返す
    bool TakeDamage(int dmg) {
        if (!active) return false;
        hp -= dmg;
        if (hp <= 0) {
            hp = 0;
            active = false;
            return true;
        }
        return false;
    }

    // HPを最大まで回復（レベルで上がった最大HPを維持する）
    void FullHeal() { hp = maxHp; }

    void Reset(const std::string& n, int h, int a) {
        name = n;
        hp = h;
        maxHp = h;
        attack = a;
        active = true;
    }

    bool IsActive() const { return active; }
    int GetHp() const { return hp; }
    int GetMaxHp() const { return maxHp; }
    int GetAttackPower() const { return attack; }
    const std::string& GetName() const { return name; }
};

// 職業
enum class JobType {
    Default = 0,
    Knight = 1,  // 騎士
    Mage = 2     // 魔法使い
};

// プレイヤー
class Player : public Character {
private:
    // レベルと経験値管理
    int level;
    int currentExp;
    int nextLevelExp;
    static constexpr int MAX_MP = 15;
    int mp = MAX_MP;

public:
    bool TrySpendMp(int cost) {
        if (mp < cost) return false;
        mp -= cost;
        return true;
    }

    AttackResult MagicAttack(Character& target, const SpellData& spell) {
        return DealDamage(target, attack + spell.bonusDamage);
    }

    void RestoreMp() { mp = MAX_MP; }
    int GetMp() const { return mp; }
    int GetMaxMp() const { return MAX_MP; }

    Player(const std::string& n, JobType job)
        : Character(n, 100, 15), level(1), currentExp(0), nextLevelExp(30) {

        if (job == JobType::Knight) {
            maxHp = hp = 120;
            attack = 18;
        }
        else if (job == JobType::Mage) {
            maxHp = hp = 80;
            attack = 25;
        }
    }

    // 経験値獲得処理とレベルアップ判定
    ExpResult GainExp(int exp) {
        ExpResult result;
        currentExp += exp;
        result.gainedExp = exp;
        result.currentExp = currentExp;
        result.nextLevelExp = nextLevelExp;

        const int MAX_LEVEL = 99;
        const int HP_INCREASE = 15;
        const int ATK_INCREASE = 4;

        // 必要経験値を満たしている間、繰り返しレベルアップ
        while (currentExp >= nextLevelExp && level < MAX_LEVEL) {
            currentExp -= nextLevelExp; // 消費した経験値を引く
            level++;

            // 次のレベルに必要な経験値を増やす
            nextLevelExp = static_cast<int>(nextLevelExp * 1.5);
            if (nextLevelExp <= 0) nextLevelExp = 10;

            // ステータス上昇
            maxHp += HP_INCREASE;
            hp = maxHp; // レベルアップで全回復
            attack += ATK_INCREASE;

            LevelUpInfo info;
            info.newLevel = level;
            info.hpIncrease = HP_INCREASE;
            info.atkIncrease = ATK_INCREASE;
            info.newMaxHp = maxHp;
            info.newAttack = attack;
            result.levelUps.push_back(info);
        }
        return result;
    }

    int GetLevel() const { return level; }
    int GetCurrentExp() const { return currentExp; }
    int GetNextLevelExp() const { return nextLevelExp; }
};

// 敵モンスター
class Enemy : public Character {
private:
    // 敵の経験値報酬
    int expReward;

public:
    // デフォルトコンストラクタで非アクティブ状態にする
    Enemy() : Character("Dummy", 1, 1), expReward(0) { active = false; }

    // 敵の初期化
    void ResetEnemy(const std::string& n, int h, int a, int exp) {
        Reset(n, h, a);
        expReward = exp;
    }

    // プールに戻すために非アクティブ化
    void Deactivate() { active = false; }

    int GetExpReward() const { return expReward; }
};