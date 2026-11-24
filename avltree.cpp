#include "avltree.h"

using std::to_string;

std::string AvlTree::tagFor(const BNode*, int, int balance) const {
    return balance > 0 ? "+" + to_string(balance) : to_string(balance);
}

void AvlTree::rot(BNode* x, bool left, const std::string& text) {
    if (left) rotateLeft(x); else rotateRight(x);
    upd(x);
    upd(x->p);
    emit(StepKind::Rotate, text, {x, x->p});
}

void AvlTree::rebalanceFrom(BNode* v) {
    while (v) {
        upd(v);
        const int bf = ht(v->l) - ht(v->r);
        if (bf > 1) {
            emit(StepKind::Rebalance,
                 "Дисбаланс в узле " + to_string(v->key) + ": левое поддерево выше на 2", {v}, true);
            BNode* c = v->l;
            if (ht(c->l) < ht(c->r))
                rot(c, true, "LR-случай: сначала поворот влево вокруг " + to_string(c->key));
            rot(v, false, "Поворот вправо вокруг " + to_string(v->key));
            v = v->p;
        } else if (bf < -1) {
            emit(StepKind::Rebalance,
                 "Дисбаланс в узле " + to_string(v->key) + ": правое поддерево выше на 2", {v}, true);
            BNode* c = v->r;
            if (ht(c->r) < ht(c->l))
                rot(c, false, "RL-случай: сначала поворот вправо вокруг " + to_string(c->key));
            rot(v, true, "Поворот влево вокруг " + to_string(v->key));
            v = v->p;
        }
        v = v->p;
    }
}

bool AvlTree::insert(int key, std::vector<Step>& steps) {
    Scope scope(this, steps);
    BNode* n = bstInsert(key);
    if (!n) {
        emit(StepKind::Info, "Ключ " + to_string(key) + " уже есть в дереве");
        return false;
    }
    n->aux = 1;
    emit(StepKind::Insert, "AVL: вставляем ключ " + to_string(key) + " как в обычное BST", {n});
    rebalanceFrom(n->p);
    emit(StepKind::Done, "AVL: ключ " + to_string(key) + " вставлен, дерево сбалансировано");
    return true;
}

bool AvlTree::erase(int key, std::vector<Step>& steps) {
    Scope scope(this, steps);
    BNode* z = find(key);
    if (!z) {
        emit(StepKind::Info, "Ключа " + to_string(key) + " нет в дереве");
        return false;
    }
    emit(StepKind::Remove, "AVL: удаляем ключ " + to_string(key), {z}, true);
    BNode *x = nullptr, *xp = nullptr;
    NodeColor c;
    removeStructure(z, x, xp, c);
    emit(StepKind::Remove, "Узел удалён (при двух детях его место занял преемник)", {xp});
    rebalanceFrom(xp);
    emit(StepKind::Done, "AVL: ключ " + to_string(key) + " удалён, дерево сбалансировано");
    return true;
}
