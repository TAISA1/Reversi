#include "Reversi.hpp"
int MinMax::evalState(State &st, int t) {
    int res = 0;
    for (int i = 0; i < 8; i++) {
        for (int j = 0; j < 8; j++) {
            if (st.board[i][j] == aturn) {
                if (t == 0) {
                    res += val[i][j];
                } else {
                    res++;
                }
            } else if (st.board[i][j] == pturn) {
                if (t == 0) {
                    res -= val[i][j];
                } else {
                    res--;
                }
            }
        }
    }
    if (fl && t == 0) {
        if (st.turn == aturn) {
            res += st.to.size() * rnd.gen(6);
        } else {
            res -= st.to.size() * rnd.gen(6);
        }
    }
    return res;
}
Move MinMax::minmaxMove(State &st) {
    Move mv;
    int ma = -INF;
    for (auto &m : st.to) {
        State nst = actMove(st, m);
        int sc;
        if (nst.cnt >= 56) {
            sc = dfs(nst, 100);
        } else {
            sc = dfs(nst, lim);
        }
        if (sc > ma) {
            ma = sc;
            mv = m;
        }
    }
    return mv;
}
int MinMax::dfs(State &st, int dep) {
    /*  if (dep == 0) {
          return evalState(st, 0);
      } else if (st.isEnd()) {
          return evalState(st, 1);
      }
      if (st.isPass()) {
          State nst = st;
          nst.turn = nextTurn(st.turn);
          nst.passed = 1;
          nst.to = nst.findMove();
          return dfs(nst, dep - 1);
      }
      if (st.turn == aturn) {
          int ma = -INF;
          for (auto &m : st.to) {
              State nst = actMove(st, m);
              int sc = dfs(nst, dep - 1);
              if (sc > ma) {
                  ma = sc;
              }
          }
          return ma;
      } else {
          int mi = INF;
          for (auto &m : st.to) {
              State nst = actMove(st, m);
              int sc = dfs(nst, dep - 1);
              if (sc < mi) {
                  mi = sc;
              }
          }
          return mi;
      }*/
    return alphabeta(st, dep, -INF, INF);
}
int MinMax::alphabeta(State &st, int dep, int alpha, int beta) {
    if (dep == 0) {
        return evalState(st, 0);
    } else if (st.isEnd()) {
        return evalState(st, 1);
    }
    if (st.isPass()) {
        State nst = st;
        nst.turn = nextTurn(st.turn);
        nst.passed = 1;
        nst.to = nst.findMove();
        return dfs(nst, dep - 1);
    }
    if (st.turn == aturn) {
        for (auto &m : st.to) {
            State nst = actMove(st, m);
            int sc = alphabeta(nst, dep - 1, alpha, beta);
            if (sc > alpha) {
                alpha = sc;
                if (alpha >= beta) {
                    break;
                }
            }
        }
        return alpha;
    } else {
        for (auto &m : st.to) {
            State nst = actMove(st, m);
            int sc = alphabeta(nst, dep - 1, alpha, beta);
            if (sc < beta) {
                beta = sc;
                if (alpha >= beta) {
                    break;
                }
            }
        }
        return beta;
    }
}
