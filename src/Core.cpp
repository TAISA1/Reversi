#include "Reversi.hpp"
#include <Siv3D.hpp>

void Rand::build() { mt.seed(rnd()); }
int Rand::gen(int ub) { return mt() % (ub + 1); } //[0,ub]

int nextTurn(int now) { return now == BLACK ? WHITE : BLACK; }
bool isValid(int y, int x) { return 0 <= x && x < 8 && 0 <= y && y < 8; }

Array<Move> State::findMove() {
    Array<Move> res;
    for (int my = 0; my < 8; my++) {
        for (int mx = 0; mx < 8; mx++) {
            if (board[my][mx] == NONE) {
                bool f = false;
                for (int i = 0; i < 8; i++) {
                    int y = my, x = mx;
                    y += dy[i];
                    x += dx[i];
                    while (isValid(y, x) && board[y][x] == nextTurn(turn)) {
                        y += dy[i];
                        x += dx[i];
                    }
                    if (isValid(y, x) && board[y][x] == turn &&
                        (y != my + dy[i] || x != mx + dx[i])) {
                        f = true;
                        break;
                    }
                }
                if (f) {
                    res.emplace_back(Move{my, mx});
                }
            }
        }
    }
    return res;
}
std::pair<int, int> State::checkResult() {
    std::pair<int, int> p;
    for (int i = 0; i < 8; i++) {
        for (int j = 0; j < 8; j++) {
            if (board[i][j] == BLACK) {
                p.first++;
            } else if (board[i][j] == WHITE) {
                p.second++;
            }
        }
    }
    return p;
}
bool State::isEnd() { return to.isEmpty() && passed; }
bool State::isPass() { return to.isEmpty(); }

void initState(State &st) {
    st.cnt = 0;
    st.turn = BLACK;
    st.passed = false;
    memset(st.board, 0, sizeof(st.board));
    st.board[3][3] = WHITE;
    st.board[3][4] = BLACK;
    st.board[4][3] = BLACK;
    st.board[4][4] = WHITE;
    st.to = st.findMove();
}

State actMove(State &st, Move &mv) {
    State nst = st;
    assert(st.board[mv.y][mv.x] == NONE);
    for (int i = 0; i < 8; i++) {
        int y = mv.y, x = mv.x;
        y += dy[i];
        x += dx[i];
        while (isValid(y, x) && st.board[y][x] == nextTurn(st.turn)) {
            y += dy[i];
            x += dx[i];
        }
        if (isValid(y, x) && st.board[y][x] == st.turn &&
            (y != mv.y + dy[i] || x != mv.x + dx[i])) {
            y = mv.y + dy[i];
            x = mv.x + dx[i];
            while (isValid(y, x) && st.board[y][x] == nextTurn(st.turn)) {
                nst.board[y][x] = st.turn;
                y += dy[i];
                x += dx[i];
            }
        }
    }
    nst.board[mv.y][mv.x] = st.turn;
    nst.turn = nextTurn(st.turn);
    nst.passed = 0;
    nst.to = nst.findMove();
    nst.cnt++;
    return nst;
}