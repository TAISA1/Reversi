#include <Siv3D.hpp> // OpenSiv3D v0.4.1
constexpr int BLACK = 1, WHITE = 2, NONE = 0;
constexpr int dx[8] = {0, 1, 0, -1, 1, 1, -1, -1};
constexpr int dy[8] = {1, 0, -1, 0, 1, -1, 1, -1};
constexpr int INF = (1 << 30) - 1;
constexpr int boardy = 40, boardx = 200, boardh = 800;
struct Rand {
    std::random_device rnd;
    std::mt19937 mt;
    void build();
    int gen(int ub);
};

int nextTurn(int now);
bool isValid(int y, int x);
struct Move {
    int y, x;
};
struct State {
    int turn, cnt;
    bool passed;
    int board[8][8];
    Array<Move> to;
    Array<Move> findMove();
    std::pair<int, int> checkResult();
    bool isEnd();
    bool isPass();
};

void initState(State &st);
State actMove(State &st, Move &mv);

struct MinMax {
    int turn, lim, aturn, pturn;
    int val[8][8] = {
        {10, -3, 3, 1, 1, 3, -3, 10},   {-3, -10, 0, 0, 0, 0, -10, -3},
        {3, 0, 0, 0, 0, 0, 0, 3},       {1, 0, 0, 0, 0, 0, 0, 1},
        {1, 0, 0, 0, 0, 0, 0, 1},       {3, 0, 0, 0, 0, 0, 0, 3},
        {-3, -10, 0, 0, 0, 0, -10, -3}, {10, -3, 3, 1, 1, 3, -3, 10}};
    int evalState(State &st, int t);
    Move minmaxMove(State &st);
    int dfs(State &st, int dep);
};

struct MonteCarlo {
    struct Node {};
};