#include "Reversi.hpp"
#include <Siv3D.hpp>
Rand rnd;
int pturn, aturn;
void drawboard(State &st) {
    Rect(boardx, boardy, boardh, boardh)
        .draw(Palette::Darkgreen)
        .drawFrame(0, 3, Palette::Black);
    for (int i = 1; i < 8; i++) {
        int x = boardx + i * (boardh / 8);
        Line(x, boardy, x, boardy + boardh).draw(2, Palette::Black);
    }
    for (int i = 1; i < 8; i++) {
        int y = boardy + i * (boardh / 8);
        Line(boardx, y, boardx + boardh, y).draw(2, Palette::Black);
    }
    for (int i = 0; i < 8; i++) {
        for (int j = 0; j < 8; j++) {
            int y = boardy + (2 * i + 1) * (boardh / 16),
                x = boardx + (2 * j + 1) * (boardh / 16);
            if (st.board[i][j] == BLACK) {
                Circle(x, y, boardh / 16 - 5).draw(Palette::Black);
            } else if (st.board[i][j] == WHITE) {
                Circle(x, y, boardh / 16 - 5).draw(Palette::White);
            }
        }
    }
}
State changeBoard(Point p, State &st) {
    Move mv = {(p.y - boardy) / (boardh / 8), (p.x - boardx) / (boardh / 8)};
    // std::cerr << mv.y << " " << mv.x << std::endl;
    for (auto e : st.to) {
        if (e.x == mv.x && e.y == mv.y) {
            return actMove(st, e);
        }
    }
    return st;
}
MinMax minimax;
Move randomMove(State &st) { return st.to[rnd.gen((int)st.to.size() - 1)]; }
void Main() {
    State now;
    initState(now);
    rnd.build();
    Window::Resize(Size(1200, 1000));
    Window::SetPos(0, 0);
    minimax.turn = WHITE;
    Stopwatch ptime, atime;
    const Font font(40);
    Scene::SetBackground(Palette::Honeydew);
    int scene = 0;
    int mode = 0;
    while (System::Update()) {
        if (scene == 0) {
            if (SimpleGUI::Button(U"ふつう", Vec2(100, 100))) {
                mode = 0;
                scene++;
            }
            if (SimpleGUI::Button(U"むずかしい", Vec2(100, 200))) {
                mode = 1;
                minimax.lim = 3;
                scene++;
            }
            if (SimpleGUI::Button(U"とてもむずかしい", Vec2(100, 300))) {
                mode = 2;
                minimax.lim = 4;
                scene++;
            }
        } else if (scene == 1) {
            if (SimpleGUI::Button(U"黒でスタート", Vec2(100, 100))) {
                ptime.start();
                pturn = BLACK;
                aturn = WHITE;
                minimax.turn = aturn;
                scene++;
            }
            if (SimpleGUI::Button(U"白でスタート", Vec2(100, 200))) {
                ptime.start();
                pturn = WHITE;
                aturn = BLACK;
                minimax.turn = aturn;
                scene++;
            }
        } else if (scene == 2) {
            minimax.pturn = pturn, minimax.aturn = aturn;
            drawboard(now);
            font(120 - ptime.s(), U"s").draw(Point(1000, 900), Palette::Black);
            font(120 - atime.s(), U"s").draw(Point(100, 100), Palette::Black);
            if (now.isEnd() || ptime.s() >= 120 || atime.s() >= 120) {
                System::Sleep(3000);
                scene++;
            }
            if (now.isPass()) {
                if (now.turn == pturn) {
                    ptime.pause();
                    atime.start();
                } else {
                    atime.pause();
                    ptime.start();
                }
                now.turn = nextTurn(now.turn);
                now.passed = 1;
                now.to = now.findMove();
                continue;
            }
            if (MouseL.down() && now.turn == pturn) {
                now = changeBoard(Cursor::Pos(), now);
                drawboard(now);
                if (now.turn == aturn) {
                    ptime.pause();
                    atime.start();
                }
            } else if (now.turn == aturn) {
                Move mv;
                if (mode == 0) {
                    mv = randomMove(now);
                } else if (mode == 1) {
                    mv = minimax.minmaxMove(now);
                }
                now = actMove(now, mv);
                atime.pause();
                ptime.start();
            }
        } else if (scene == 3) {
            std::pair<int, int> res = now.checkResult();
            if ((res.first > res.second && pturn == BLACK) ||
                (res.first < res.second && pturn == WHITE)) {
                font(U"あなたの勝ち!").draw(Point(400, 400), Palette::Black);
            } else if (res.first == res.second) {
                font(U"引き分け...").draw(Point(400, 400), Palette::Black);
            } else {
                font(U"あなたの負け...").draw(Point(400, 400), Palette::Black);
            }
            font(U"あなた: ", (pturn == BLACK ? res.first : res.second))
                .draw(Point(800, 800), Palette::Black);
            font(U"AI: ", (aturn == BLACK ? res.first : res.second))
                .draw(Point(800, 900), Palette::Black);
        }
    }
}
