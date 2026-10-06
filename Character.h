#pragma once
#include <string>
#include <vector>
#include <iostream>

// キャラクター基底クラス
class Character  {
protected:
    std::string name;
    int hp;
    int maxHp;
    int attack;
    bool active;

public:
    Character(const std::string& n, int h, int a)
        : name(n), hp(h), maxHp(h), attack(a), active(true) {
    }
    virtual ~Character() = default;

    virtual void Attack(Character& target) {
        std::cout << name << " の攻撃！ " << target.GetName() << " に " << attack << " のダメージ！\n";
        target.TakeDamage(attack);
    }

    void TakeDamage(int dmg) {
        hp -= dmg;
        if (hp <= 0) {
            hp = 0;
            active = false;
            std::cout << name << " は倒れた！\n";
        }
    }

    void Reset(const std::string& n, int h, int a) {
        name = n;
        hp = h;
        maxHp = h;
        attack = a;
        active = true;
    }

    bool IsActive() const { return active; }
    int GetHp() const { return hp; }
    const std::string& GetName() const { return name; }
};

// プレイヤー（ファクトリー
class Player : public Character {

    // レベルと経験値管理
private:
    int level = 1;
    int currentExp = 0;
    int nextLevelExp = 30;

public:
    // コンストラクタで初期値を明確に指定する
    Player(const std::string& n, int jobType)
        : Character(n, 100, 15), level(1), currentExp(0), nextLevelExp(30) { 

        if (jobType == 1) { // 騎士
            maxHp = hp = 120;
            attack = 18;
        }
        else if (jobType == 2) { // 魔法使い
            maxHp = hp = 80;
            attack = 25;
        }
    }

    // 経験値獲得処理とレベルアップ判定
    void GainExp(int exp) {
        currentExp += exp;
        std::cout << exp << " ポイントの経験値を獲得！ (" << currentExp << " / " << nextLevelExp << ")\n";

        const int MAX_LEVEL = 99;

        // 必要経験値を満たしている間、繰り返しレベルアップ
        while (currentExp >= nextLevelExp && level < MAX_LEVEL) {
            currentExp -= nextLevelExp; // 消費した経験値を引く
            level++;

            // 次のレベルに必要な経験値を増やす
            nextLevelExp = static_cast<int>(nextLevelExp * 1.5);
            if (nextLevelExp <= 0) nextLevelExp = 10; 

            // ステータス上昇
            int hpIncrease = 15;
            int atkIncrease = 4;
            maxHp += hpIncrease;
            hp = maxHp; // レベルアップで全回復
            attack += atkIncrease;

            std::cout << "\n★ LEVEL UP! ★\n";
            std::cout << name << " は レベル " << level << " に上がった！\n";
            std::cout << "最大HP +" << hpIncrease << " (現在: " << maxHp << ")\n";
            std::cout << "攻撃力 +" << atkIncrease << " (現在: " << attack << ")\n\n";
        }
    }

    int GetLevel() const { return level; }
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

    int GetExpReward() const { return expReward; }
};

// エネミー専用のオブジェクトプール (Object Pool)
class EnemyPool {
private:
    std::vector<Enemy> pool;

public:
    EnemyPool(size_t size) {
        pool.resize(size);
    }

    // プールから未使用の敵を取得・初期化
    Enemy* SpawnEnemy(int enemyTypeId);

    // 敵をプールに返却（非アクティブ化）
    void DespawnEnemy(Enemy* enemy) {
        if (enemy) {
            // inactiveフラグを立てて再利用可能状態にする
            enemy->ResetEnemy("Dummy", 1, 1, 0);
        }
    }
};