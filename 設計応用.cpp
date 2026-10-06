#include "GameState.h"
#include <ctime>
#include <cstdlib>

int main() {
    // 乱数の初期化
    std::srand(static_cast<unsigned int>(std::time(nullptr)));

    // ゲーム状態管理クラスを立ち上げて実行
    GameStateManager game;
    game.Run();

    return 0;
}