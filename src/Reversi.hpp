#include <Siv3D.hpp> // OpenSiv3D v0.4.1
constexpr int BLACK = 1, WHITE = 2, NONE = 0;
constexpr int dx[8] = {0, 1, 0, -1, 1, 1, -1, -1};
constexpr int dy[8] = {1, 0, -1, 0, 1, -1, 1, -1};
constexpr int INF = (1 << 30) - 1;
constexpr int boardy = 140, boardx = 240, boardh = 720;
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
    int lim, aturn, pturn;
    bool fl;
    int val[8][8] = {{100, -40, 20, 5, 5, 20, -40, 100},
                     {-40, -80, -1, -1, -1, -1, -80, -40},
                     {20, -1, 5, 1, 1, 5, -1, 20},
                     {5, -1, 1, 0, 0, 1, -1, 5},
                     {5, -1, 1, 0, 0, 1, -1, 5},
                     {20, -1, 5, 1, 1, 5, -1, 20},
                     {-40, -80, -1, -1, -1, -1, -80, -40},
                     {100, -40, 20, 5, 5, 20, -40, 100}};
    int evalState(State &st, int t);
    Move minmaxMove(State &st);
    int dfs(State &st, int dep);
    int alphabeta(State &st, int dep, int alpha, int beta);
};

struct MonteCarlo {
    Rand rnd;
    int aturn, pturn;
    struct Node {
        int cnt, win;
        State now;
        Node *par;
        Array<Node> child;
        Array<Move> unvis;
        Array<int> idx;
        Node(State &st);
        Node &operator=(const Node &nd);
    };
    static bool cmppri(const Node &n1, const Node &n2);
    static bool cmpcnt(const Node &n1, const Node &n2);
    inline static double calcpri(const Node &nd);
    Node *selection(Node *nd);
    Node *expand(Node *nd);
    int playout(Node *nd);
    void backprop(Node *nd, int res);
    Move search(State &st);
};