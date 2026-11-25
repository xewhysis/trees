#include "splaytree.h"

using std::to_string;

void SplayTree::splayTo(BNode* x, BNode* stop) {
    while (x->p != stop) {
        BNode* p = x->p;
        BNode* g = p->p;
        if (g == stop) {
            rotateUp(x);
            emit(StepKind::Rotate, "Zig: поворот " + to_string(x->key) + " над родителем", {x, p});
        } else if ((g->l == p) == (p->l == x)) {
            rotateUp(p);
            emit(StepKind::Rotate, "Zig-zig: сначала поворачиваем родителя над дедом", {x, p, g});
            rotateUp(x);
            emit(StepKind::Rotate, "Zig-zig: затем поворачиваем " + to_string(x->key) + " над родителем", {x, p});
        } else {
            rotateUp(x);
            emit(StepKind::Rotate, "Zig-zag: первый поворот " + to_string(x->key), {x, p, g});
            rotateUp(x);
            emit(StepKind::Rotate, "Zig-zag: второй поворот " + to_string(x->key), {x, g});
        }
    }
}

bool SplayTree::insert(int key, std::vector<Step>& steps) {
    Scope scope(this, steps);
    BNode* n = bstInsert(key);
    if (!n) {
        BNode* e = find(key);
        emit(StepKind::Info, "Ключ " + to_string(key) + " уже есть — поднимаем его в корень", {e});
        splayTo(e, nullptr);
        emit(StepKind::Done, "Splay: ключ поднят в корень");
        return false;
    }
    emit(StepKind::Insert, "Splay: вставляем " + to_string(key) + " как в обычное BST", {n});
    splayTo(n, nullptr);
    emit(StepKind::Done, "Splay: " + to_string(key) + " теперь в корне");
    return true;
}

bool SplayTree::erase(int key, std::vector<Step>& steps) {
    Scope scope(this, steps);
    BNode* z = find(key);
    if (!z) {
        emit(StepKind::Info, "Ключа " + to_string(key) + " нет в дереве");
        return false;
    }
    emit(StepKind::Remove, "Splay: удаляем " + to_string(key) + " — сначала поднимаем в корень", {z}, true);
    splayTo(z, nullptr);
    if (!z->l) {
        transplant(z, z->r);
    } else {
        // Максимум левого поддерева поднимаем прямо под z — у него не будет правого ребёнка.
        BNode* m = maximum(z->l);
        splayTo(m, z);
        transplant(z, m);
        m->r = z->r;
        if (m->r) m->r->p = m;
    }
    emit(StepKind::Remove, "Корень удалён, поддеревья объединены");
    emit(StepKind::Done, "Splay: ключ " + to_string(key) + " удалён");
    return true;
}
