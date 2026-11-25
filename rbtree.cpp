#include "rbtree.h"

using std::to_string;

bool RbTree::insert(int key, std::vector<Step>& steps) {
    Scope scope(this, steps);
    BNode* z = bstInsert(key);
    if (!z) {
        emit(StepKind::Info, "Ключ " + to_string(key) + " уже есть в дереве");
        return false;
    }
    z->color = NodeColor::Red;
    emit(StepKind::Insert, "КЧ: вставляем " + to_string(key) + " — новый узел всегда красный", {z});
    insertFixup(z);
    emit(StepKind::Done, "КЧ: ключ " + to_string(key) + " вставлен");
    return true;
}

void RbTree::insertFixup(BNode* z) {
    while (z->p && z->p->color == NodeColor::Red) {
        BNode* p = z->p;
        BNode* g = p->p;   // у красного родителя дед есть: корень всегда чёрный
        const bool left = (p == g->l);
        BNode* u = left ? g->r : g->l;
        if (u && u->color == NodeColor::Red) {
            p->color = NodeColor::Black;
            u->color = NodeColor::Black;
            g->color = NodeColor::Red;
            emit(StepKind::Recolor,
                 "Дядя красный: отец и дядя становятся чёрными, дед — красным", {p, u, g});
            z = g;
        } else {
            if (left && z == p->r) {
                z = p;
                rotateLeft(z);
                emit(StepKind::Rotate, "Дядя чёрный, «колено»: поворот влево вокруг отца", {z, z->p});
                p = z->p;
            } else if (!left && z == p->l) {
                z = p;
                rotateRight(z);
                emit(StepKind::Rotate, "Дядя чёрный, «колено»: поворот вправо вокруг отца", {z, z->p});
                p = z->p;
            }
            p->color = NodeColor::Black;
            g->color = NodeColor::Red;
            emit(StepKind::Recolor, "Отец становится чёрным, дед — красным", {p, g});
            if (left) rotateRight(g); else rotateLeft(g);
            emit(StepKind::Rotate, "Поворот вокруг деда", {g, g->p});
            break;
        }
    }
    if (root_->color == NodeColor::Red) {
        root_->color = NodeColor::Black;
        emit(StepKind::Recolor, "Корень всегда чёрный", {root_});
    }
}

bool RbTree::erase(int key, std::vector<Step>& steps) {
    Scope scope(this, steps);
    BNode* z = find(key);
    if (!z) {
        emit(StepKind::Info, "Ключа " + to_string(key) + " нет в дереве");
        return false;
    }
    emit(StepKind::Remove, "КЧ: удаляем ключ " + to_string(key), {z}, true);
    BNode *x = nullptr, *xp = nullptr;
    NodeColor removed;
    removeStructure(z, x, xp, removed);
    emit(StepKind::Remove, "Узел вырезан из дерева", {xp});
    if (removed == NodeColor::Black) eraseFixup(x, xp);
    emit(StepKind::Done, "КЧ: ключ " + to_string(key) + " удалён");
    return true;
}

void RbTree::eraseFixup(BNode* x, BNode* xp) {
    while (x != root_ && isBlack(x)) {
        const bool left = (x == xp->l);
        BNode* w = left ? xp->r : xp->l;   // брат (у «двойного чёрного» он всегда есть)
        if (w->color == NodeColor::Red) {
            w->color = NodeColor::Black;
            xp->color = NodeColor::Red;
            emit(StepKind::Recolor, "Брат красный: красим брата в чёрный, отца в красный", {w, xp});
            if (left) rotateLeft(xp); else rotateRight(xp);
            emit(StepKind::Rotate, "Поворот вокруг отца", {xp, xp->p});
            w = left ? xp->r : xp->l;
        }
        BNode* nearC = left ? w->l : w->r;
        BNode* farC = left ? w->r : w->l;
        if (isBlack(nearC) && isBlack(farC)) {
            w->color = NodeColor::Red;
            emit(StepKind::Recolor, "Оба племянника чёрные: брат становится красным, проблема уходит вверх",
                 {w, xp});
            x = xp;
            xp = x->p;
        } else {
            if (isBlack(farC)) {
                nearC->color = NodeColor::Black;
                w->color = NodeColor::Red;
                emit(StepKind::Recolor, "Дальний племянник чёрный: перекраска перед поворотом", {nearC, w});
                if (left) rotateRight(w); else rotateLeft(w);
                emit(StepKind::Rotate, "Поворот вокруг брата", {w, w->p});
                w = left ? xp->r : xp->l;
                farC = left ? w->r : w->l;
            }
            w->color = xp->color;
            xp->color = NodeColor::Black;
            if (farC) farC->color = NodeColor::Black;
            emit(StepKind::Recolor, "Брат берёт цвет отца, отец и дальний племянник — чёрные", {w, xp, farC});
            if (left) rotateLeft(xp); else rotateRight(xp);
            emit(StepKind::Rotate, "Поворот вокруг отца: баланс восстановлен", {xp, xp->p});
            x = root_;
            xp = nullptr;
        }
    }
    if (x && x->color == NodeColor::Red) {
        x->color = NodeColor::Black;
        emit(StepKind::Recolor, "Узел становится чёрным", {x});
    }
}
