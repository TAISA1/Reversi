#include "Reversi.hpp"
MonteCarlo::Node::Node(State &st) {
    cnt = 0;
    win = 0;
    pri = 0;
    now = st;
    unvis = st.to;
    par = nullptr;
}
MonteCarlo::Node &MonteCarlo::Node::operator=(const Node &nd) {
    cnt = nd.cnt;
    win = nd.win;
    pri = nd.pri;
    now = nd.now;
    unvis = nd.unvis;
    child = nd.child;
    par = nd.par;
    return *this;
}
bool MonteCarlo::cmppri(const Node &n1, const Node &n2) {
    return n1.pri < n2.pri;
}
bool MonteCarlo::cmpcnt(const Node &n1, const Node &n2) {
    return n1.cnt < n2.cnt;
}

MonteCarlo::Node *MonteCarlo::selection(Node *nd) {
    return &(*std::max_element(nd->child.begin(), nd->child.end(), cmppri));
}
void MonteCarlo::expand(Node *nd) {
    int id = rnd.gen((int)nd->unvis.size() - 1);
    Move mv = nd->unvis[id];
    std::swap(nd->unvis[id], nd->unvis.back());
    nd->unvis.pop_back();
    State st = actMove(nd->now, mv);
    Node nnode(st);
    nnode.par = nd;
    nd->idx.push_back(id);
    nd->child.push_back(nnode);
}
int MonteCarlo::playout(Node *nd) {
    State now = nd->now;
    while (!now.isEnd()) {
        if (now.isPass()) {
            now.turn = nextTurn(now.turn);
            now.passed = 1;
            now.to = now.findMove();
            continue;
        }
        now = actMove(now, now.to[rnd.gen((int)now.to.size() - 1)]);
    }
    std::pair<int, int> res = now.checkResult();
    if ((res.first > res.second && aturn == BLACK) ||
        (res.first < res.second && aturn == WHITE)) {
        return 3;
    } else if (res.first == res.second) {
        return 1;
    } else {
        return 0;
    }
}
void MonteCarlo::backprop(Node *nd, int res) {
    Node *node = nd;
    while (1) {
        if (node->par == nullptr) {
            break;
        }
        node->win += res;
        node->cnt++;
        node = node->par;
    }
}
Move MonteCarlo::search(State &st) {
    Node root(st);
    for (int c = 0; c < 1000; c++) {
        Node *node = &root;
        while (node->unvis.empty() && !node->child.empty()) {
            node = selection(node);
        }
        if (!node->unvis.empty()) {
            expand(node);
        }
        if(node->now.cnt==1){}
        }
        backprop(node, playout(node));
    }
    for (auto e : root.child) {
        std::cerr << e.cnt << " ";
    }
    std::cerr << std::endl;
    int t = std::max_element(root.child.begin(), root.child.end(), cmpcnt) -
            root.child.begin();
    int id = root.idx[t];
    return st.to[id];
}