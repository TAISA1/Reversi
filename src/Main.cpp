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
MonteCarlo mcts;
Move randomMove(State &st) { return st.to[rnd.gen((int)st.to.size() - 1)]; }
void Main() {
    State now;
    initState(now);
    rnd.build();
    Window::Resize(Size(1200, 1000));
    Window::SetPos(0, 0);
    Stopwatch ptime, atime, cdown;
    const Font font(40), title(80), tim(250), txt(20),
        fontb(20, Typeface::Bold);
    const Texture think(Emoji(U"🤔"));
    Scene::SetBackground(Palette::White);
    int scene = -1;
    int mode = 0;
    bool th = false, cd = true;
    while (System::Update()) {
        font(Cursor::Pos().x, U",", Cursor::Pos().y)
            .draw(Point(0, 0), Palette::Black);
        if (scene != 2) {
            Rect(0, 0, 1200, 500).draw(Palette::Black);
            Rect(0, 500, 1200, 500).draw(Palette::White);
        }
        if (scene == -1) {
            title(U"90秒リバーシ").draw(Point(340, 100), Palette::White);
            txt(U"自分の色のコマで相手の色のコマを挟むと自分の色にできます。最"
                U"終的にコマの数が多い方の色が勝ちです。")
                .draw(Point(100, 550), Palette::Black);
            fontb(U"持ち時間は90秒です。時間切れになると負けになります。")
                .draw(Point(330, 630), Palette::Red);
            fontb(U"AIと対戦!").draw(Point(530, 710), Palette::Black);
            font(U"クリックしてはじめる").draw(Point(400, 800), Palette::Black);
            if (MouseL.down()) {
                scene++;
                continue;
            }
        } else if (scene == 0) {
            font(U"相手のつよさを選んでください")
                .draw(Point(330, 200), Palette::White);
            if (SimpleGUI::Button(U"ふつう", Vec2(300, 550), 600)) {
                mode = 0;
                scene++;
            }
            if (SimpleGUI::Button(U"ちょっとつよい", Vec2(300, 700), 600)) {
                mode = 1;
                minimax.lim = 3;
                minimax.fl = false;
                scene++;
            }
            if (SimpleGUI::Button(U"つよい", Vec2(300, 850), 600)) {
                mode = 2;
                minimax.lim = 4;
                minimax.fl = true;
                scene++;
            }
        } else if (scene == 1) {
            if (SimpleGUI::Button(U"黒でスタート", Vec2(300, 200), 600)) {
                pturn = BLACK;
                aturn = WHITE;
                minimax.pturn = pturn, minimax.aturn = aturn;
                mcts.pturn = pturn, mcts.aturn = aturn;
                scene++;
                cdown.start();
            }
            if (SimpleGUI::Button(U"白でスタート", Vec2(300, 700), 600)) {
                pturn = WHITE;
                aturn = BLACK;
                minimax.pturn = pturn, minimax.aturn = aturn;
                mcts.pturn = pturn, mcts.aturn = aturn;
                scene++;
                cdown.start();
            }
        } else if (scene == 2) {
            drawboard(now);
            if ((90 - ptime.s()) % 60 < 10) {
                title((90 - ptime.s()) / 60, U":0", (90 - ptime.s()) % 60)
                    .draw(Point(995, 718), Palette::Black);
            } else {
                title((90 - ptime.s()) / 60, U":", (90 - ptime.s()) % 60)
                    .draw(Point(995, 718), Palette::Black);
            }
            RoundRect(980, 725, 200, 100, 8)
                .drawFrame(2.0, Color(60, 60, 60, 255));
            if ((90 - atime.s()) % 60 < 10) {
                title((90 - atime.s()) / 60, U":0", (90 - atime.s()) % 60)
                    .draw(Point(32, 183), Palette::Black);
            } else {
                title((90 - atime.s()) / 60, U":", (90 - atime.s()) % 60)
                    .draw(Point(32, 183), Palette::Black);
            }
            RoundRect(20, 190, 200, 100, 8)
                .drawFrame(2.0, Color(60, 60, 60, 255));
            if (cd) {
                if (cdown.s() >= 4) {
                    cdown.reset();
                    if (pturn == BLACK) {
                        ptime.start();
                    } else {
                        atime.start();
                    }
                    cd = false;
                    continue;
                }
                Rect(0, 0, 1200, 1000).draw(Color(255, 255, 255, 200));
                if (cdown.s() == 3) {
                    tim(U"Start!").draw(Point(250, 350),
                                        Color(60, 60, 60, 230));
                } else {
                    tim(3 - cdown.s())
                        .draw(Point(530, 350), Color(60, 60, 60, 230));
                }
                continue;
            }
            if (now.isEnd() || ptime.s() >= 91 || atime.s() >= 91) {
                ptime.pause();
                atime.pause();
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
                //  Move mv = minimax2.minmaxMove(now);
                // now = actMove(now, mv);
                if (now.turn == aturn) {
                    ptime.pause();
                    atime.start();
                }
            } else if (now.turn == aturn) {
                if (!th) {
                    // font(U"考え中...").draw(Point(50, 100), Palette::Black);
                    think.scaled(1.7).drawAt(100, 200);
                    th = true;
                    continue;
                }
                th = false;
                Move mv;
                if (mode == 0) {
                    mv = mcts.search(now);
                } else {
                    mv = minimax.minmaxMove(now);
                }
                System::Sleep(500);
                now = actMove(now, mv);
                atime.pause();
                ptime.start();
            }
        } else if (scene == 3) {
            std::pair<int, int> res = now.checkResult();
            if (ptime.s() <= 90 && atime.s() <= 90) {
                if ((res.first > res.second && pturn == BLACK) ||
                    (res.first < res.second && pturn == WHITE)) {
                    title(U"あなたの勝ち!")
                        .draw(Point(360, 400), Palette::White);
                } else if (res.first == res.second) {
                    title(U"引き分け...").draw(Point(360, 400), Palette::White);
                } else {
                    title(U"あなたの負け...")
                        .draw(Point(350, 400), Palette::White);
                }
                font(U"あなた: ", (pturn == BLACK ? res.first : res.second))
                    .draw(Point(800, 800), Palette::Black);
                font(U"AI: ", (aturn == BLACK ? res.first : res.second))
                    .draw(Point(850, 900), Palette::Black);
            } else {
                title(U"あなたの負け...").draw(Point(350, 400), Palette::White);
                font(U"時間切れ").draw(Point(800, 900), Palette::Black);
            }
            if (SimpleGUI::Button(U"ゲームをおわる", Vec2(300, 600), 600)) {
                System::Exit();
            }
        }
    }
}
