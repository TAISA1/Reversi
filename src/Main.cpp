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
    const Font font(40), title(80), tim(250), txt(25),
        fontb(35, Typeface::Bold), think(60);
    // const Texture think(Emoji(U"🤔"));
    Scene::SetBackground(Palette::White);
    int scene = -1;
    int mode = 0;
    bool th = false, cd = true, tl = false, written = false, psd = false;
    BinaryReader breader(U"data/win.bin");
    int32 wc1 = 0, dc1 = 0, lc1 = 0, wc2 = 0, dc2 = 0, lc2 = 0, wc3 = 0,
          dc3 = 0, lc3 = 0;
    breader.read(wc1);
    breader.read(dc1);
    breader.read(lc1);
    breader.read(wc2);
    breader.read(dc2);
    breader.read(lc2);
    breader.read(wc3);
    breader.read(dc3);
    breader.read(lc3);
    while (System::Update()) {
        if (psd) {
            System::Sleep(500);
            psd = false;
        }
        if (scene != 2) {
            Rect(0, 0, 1200, 500).draw(Palette::Black);
            Rect(0, 500, 1200, 500).draw(Palette::White);
        }
        // font(Cursor::Pos().x, U",", Cursor::Pos().y)
        //.draw(Point(0, 0), Palette::White);
        if (scene == -1) {
            title(U"90秒リバーシ").draw(Point(340, 150), Palette::White);
            txt(U"・自分の色のコマで相手の色のコマを挟むと、自分の色にできます"
                U"。")
                .draw(Point(130, 520), Palette::Black);
            txt(U"・最終的にコマの数が多い方の色が勝ちです。")
                .draw(Point(130, 570), Palette::Black);
            fontb(U"・持ち時間は90秒です。時間切れになると負けになります。")
                .draw(Point(130, 630), Palette::Red);
            font(U"画面をクリックしてはじめる")
                .draw(Point(350, 800), Palette::Black);
            if (MouseL.down()) {
                scene++;
                continue;
            }
        } else if (scene == 0) {
            font(U"相手のつよさを選んでください")
                .draw(Point(330, 200), Palette::White);
            int mx = Cursor::Pos().x, my = Cursor::Pos().y;
            if (550 <= my && my <= 850) {
                if (50 <= mx && mx <= 350) {
                    RoundRect(50, 550, 300, 300, 10)
                        .draw(Color(220, 220, 220, 220));
                    if (MouseL.down()) {
                        mode = 0;
                        scene++;
                    }
                } else if (450 <= mx && mx <= 750) {
                    RoundRect(450, 550, 300, 300, 10)
                        .draw(Color(220, 220, 220, 200));
                    if (MouseL.down()) {
                        mode = 1;
                        minimax.lim = 3;
                        minimax.fl = false;
                        scene++;
                    }
                } else if (850 <= mx && mx <= 1150) {
                    RoundRect(850, 550, 300, 300, 10)
                        .draw(Color(220, 220, 220, 200));
                    if (MouseL.down()) {
                        mode = 2;
                        minimax.lim = 4;
                        minimax.fl = true;
                        scene++;
                    }
                }
            }
            RoundRect(50, 550, 300, 300, 10).drawFrame(2, 0, Palette::Black);
            RoundRect(450, 550, 300, 300, 10).drawFrame(2, 0, Palette::Black);
            RoundRect(850, 550, 300, 300, 10).drawFrame(2, 0, Palette::Black);
            font(U"ふつう").draw(Point(137, 670), Palette::Black);
            font(U"ちょっとつよい").draw(Point(460, 670), Palette::Black);
            font(U"つよい").draw(Point(937, 670), Palette::Red);
            txt(U"AI勝利:", lc1, U"回").draw(Point(130, 320), Palette::White);
            txt(U"対戦:", wc1 + dc1 + lc1, U"回")
                .draw(Point(130, 370), Palette::White);
            if (wc1 + dc1 + lc1 == 0) {
                txt(U"AI勝率:", 0, U"%").draw(Point(130, 420), Palette::White);
            } else {
                int p1 = 100.0 * (double)lc1 / (wc1 + dc1 + lc1);
                txt(U"勝率:", p1, U"%").draw(Point(130, 420), Palette::White);
            }
            txt(U"AI勝利:", lc2, U"回").draw(Point(530, 320), Palette::White);
            txt(U"対戦:", wc2 + dc2 + lc2, U"回")
                .draw(Point(530, 370), Palette::White);
            if (wc2 + dc2 + lc2 == 0) {
                txt(U"AI勝率:", 0, U"%").draw(Point(530, 420), Palette::White);
            } else {
                int p1 = 100.0 * (double)lc2 / (wc2 + dc2 + lc2);
                txt(U"AI勝率:", p1, U"%").draw(Point(530, 420), Palette::White);
            }
            txt(U"AI勝利:", lc3, U"回").draw(Point(930, 320), Palette::White);
            txt(U"対戦:", wc3 + dc3 + lc3, U"回")
                .draw(Point(930, 370), Palette::White);
            if (wc3 + dc3 + lc3 == 0) {
                txt(U"AI勝率:", 0, U"%").draw(Point(930, 420), Palette::White);
            } else {
                int p1 = 100.0 * (double)lc3 / (wc3 + dc3 + lc3);
                txt(U"AI勝率:", p1, U"%").draw(Point(930, 420), Palette::White);
            }
        } else if (scene == 1) {
            int mx = Cursor::Pos().x, my = Cursor::Pos().y;
            if (200 <= mx && mx <= 1000 && 100 <= my && my <= 400) {
                RoundRect(200, 150, 800, 200, 10).draw(Color(30, 30, 30, 200));
                if (MouseL.down()) {
                    pturn = BLACK;
                    aturn = WHITE;
                    minimax.pturn = pturn, minimax.aturn = aturn;
                    mcts.pturn = pturn, mcts.aturn = aturn;
                    scene++;
                    cdown.start();
                    continue;
                }
            } else if (200 <= mx && mx <= 800 && 600 <= my && my <= 900) {
                RoundRect(200, 650, 800, 200, 10)
                    .draw(Color(220, 220, 220, 200));
                if (MouseL.down()) {
                    pturn = WHITE;
                    aturn = BLACK;
                    minimax.pturn = pturn, minimax.aturn = aturn;
                    mcts.pturn = pturn, mcts.aturn = aturn;
                    scene++;
                    cdown.start();
                    continue;
                }
            }
            font(U"黒ではじめる").draw(Point(480, 215), Palette::White);
            font(U"白ではじめる").draw(Point(480, 715), Palette::Black);
            RoundRect(200, 150, 800, 200, 10).drawFrame(3, 0, Palette::White);
            RoundRect(200, 650, 800, 200, 10).drawFrame(3, 0, Palette::Black);
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
            if (now.turn != aturn || th || cd) {
                if ((90 - atime.s()) % 60 < 10) {
                    title((90 - atime.s()) / 60, U":0", (90 - atime.s()) % 60)
                        .draw(Point(32, 183), Palette::Black);
                } else {
                    title((90 - atime.s()) / 60, U":", (90 - atime.s()) % 60)
                        .draw(Point(32, 183), Palette::Black);
                }
            }
            RoundRect(20, 190, 200, 100, 8)
                .drawFrame(2.0, Color(60, 60, 60, 255));
            if (31 <= ptime.s() && ptime.s() <= 33) {
                title(U"あと 1 分です").draw(Point(350, 10), Palette::Red);
            } else if (61 <= ptime.s() && ptime.s() <= 62) {
                title(U"あと 30 秒です").draw(Point(300, 10), Palette::Red);
            }
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
                if (ptime.s() >= 91 || atime.s() >= 91) {
                    tl = true;
                } else {
                    tl = false;
                }
                ptime.pause();
                atime.pause();
                System::Sleep(2000);
                scene++;
                continue;
            } else if (now.isPass()) {
                if (now.turn == pturn) {
                    ptime.pause();
                    atime.start();
                    title(U"パスです").draw(Point(450, 10), Palette::Black);
                } else {
                    atime.pause();
                    ptime.start();
                    title(U"パスです").draw(Point(450, 10), Palette::Black);
                }
                now.turn = nextTurn(now.turn);
                now.passed = 1;
                now.to = now.findMove();
                psd = true;
                continue;
            }
            if (now.turn == pturn) {
                for (auto &mv : now.to) {
                    int y = boardy + (2 * mv.y) * (boardh / 16),
                        x = boardx + (2 * mv.x) * (boardh / 16);
                    Rect(x + 35, y + 35, boardh / 8 - 70, boardh / 8 - 70)
                        .draw(Color(0, 160, 0, 200));
                }
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
                    think(U"考え中").draw(Point(32, 191),
                                          Color(134, 145, 32, 200));
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
            if (!tl) {
                if ((res.first > res.second && pturn == BLACK) ||
                    (res.first < res.second && pturn == WHITE)) {
                    title(U"あなたの勝ち!")
                        .draw(Point(360, 300), Palette::White);
                    if (!written) {
                        if (mode == 0) {
                            wc1++;
                        } else if (mode == 1) {
                            wc2++;
                        } else {
                            wc3++;
                        }
                    }
                    if (mode == 0) {
                        txt(U"あなたは", wc1, U"人目の勝者です！")
                            .draw(Point(50, 700), Palette::Black);
                    } else if (mode == 1) {
                        txt(U"あなたは", wc2, U"人目の勝者です！")
                            .draw(Point(50, 700), Palette::Black);
                    } else {
                        txt(U"あなたは", wc3, U"人目の勝者です！")
                            .draw(Point(50, 700), Palette::Black);
                    }
                } else if (res.first == res.second) {
                    title(U"引き分け...").draw(Point(360, 300), Palette::White);
                    if (!written) {
                        if (mode == 0) {
                            dc1++;
                        } else if (mode == 1) {
                            dc2++;
                        } else {
                            dc3++;
                        }
                    }
                } else {
                    title(U"あなたの負け...")
                        .draw(Point(350, 300), Palette::White);
                    if (!written) {
                        if (mode == 0) {
                            lc1++;
                        } else if (mode == 1) {
                            lc2++;
                        } else {
                            lc3++;
                        }
                    }
                }
                font(U"あなた: ", (pturn == BLACK ? res.first : res.second))
                    .draw(Point(800, 800), Palette::Black);
                font(U"AI: ", (aturn == BLACK ? res.first : res.second))
                    .draw(Point(875, 900), Palette::Black);
            } else {
                title(U"あなたの負け...").draw(Point(350, 300), Palette::White);
                font(U"時間切れ").draw(Point(800, 900), Palette::Black);
                if (!written) {
                    if (mode == 0) {
                        lc1++;
                    } else if (mode == 1) {
                        lc2++;
                    } else {
                        lc3++;
                    }
                }
            }
            if (!written) {
                BinaryWriter bwriter(U"data/win.bin");
                bwriter.write(wc1);
                bwriter.write(dc1);
                bwriter.write(lc1);
                bwriter.write(wc2);
                bwriter.write(dc2);
                bwriter.write(lc2);
                bwriter.write(wc3);
                bwriter.write(dc3);
                bwriter.write(lc3);
                written = true;
            }
            if (SimpleGUI::Button(U"ゲームをおわる", Vec2(300, 600), 600)) {
                System::Exit();
            }
        }
    }
}