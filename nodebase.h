#pragma once
#include <algorithm>
#include <deque>
#include <initializer_list>
#include <string>
#include "itree.h"

// Общий узел бинарных деревьев (AVL, RB, Treap, Splay).
struct BNode {
    int id = 0;                          // стабильный идентификатор для анимации
    int key = 0;
    NodeColor color = NodeColor::Plain;  // RB
    int aux = 0;                         // AVL: высота, Treap: приоритет
    BNode* l = nullptr;
    BNode* r = nullptr;
    BNode* p = nullptr;
};

// Базовый класс: хранение узлов, повороты, снимки, запись шагов.
class BinaryTreeBase : public ITree {
public:
    void clear() override { root_ = nullptr; pool_.clear(); }
    bool contains(int key) const override { return find(key) != nullptr; }

    Snapshot snapshot() const override {
        Snapshot s;
        s.multiKey = false;
        if (root_) {
            s.root = root_->id;
            build(root_, s);
        }
        return s;
    }

protected:
    // Включает запись шагов на время операции.
    struct Scope {
        BinaryTreeBase* t;
        Scope(BinaryTreeBase* tree, std::vector<Step>& steps) : t(tree) { t->steps_ = &steps; }
        ~Scope() { t->steps_ = nullptr; }
    };

    virtual std::string tagFor(const BNode*, int /*height*/, int /*balance*/) const { return {}; }

    BNode* find(int key) const {
        BNode* c = root_;
        while (c && c->key != key) c = key < c->key ? c->l : c->r;
        return c;
    }
    BNode* newNode(int key) {
        pool_.emplace_back();
        BNode& n = pool_.back();
        n.id = nextId_++;
        n.key = key;
        return &n;
    }
    static BNode* minimum(BNode* n) { while (n->l) n = n->l; return n; }
    static BNode* maximum(BNode* n) { while (n->r) n = n->r; return n; }

    void transplant(BNode* u, BNode* v) {
        if (!u->p) root_ = v;
        else if (u == u->p->l) u->p->l = v;
        else u->p->r = v;
        if (v) v->p = u->p;
    }
    void rotateLeft(BNode* x) {
        BNode* y = x->r;
        x->r = y->l;
        if (y->l) y->l->p = x;
        y->p = x->p;
        if (!x->p) root_ = y;
        else if (x == x->p->l) x->p->l = y;
        else x->p->r = y;
        y->l = x;
        x->p = y;
    }
    void rotateRight(BNode* x) {
        BNode* y = x->l;
        x->l = y->r;
        if (y->r) y->r->p = x;
        y->p = x->p;
        if (!x->p) root_ = y;
        else if (x == x->p->r) x->p->r = y;
        else x->p->l = y;
        y->r = x;
        x->p = y;
    }
    // Поднять x над его родителем.
    void rotateUp(BNode* x) {
        if (x == x->p->l) rotateRight(x->p);
        else rotateLeft(x->p);
    }

    // Обычная вставка в BST. nullptr — такой ключ уже есть.
    BNode* bstInsert(int key) {
        BNode* parent = nullptr;
        BNode* cur = root_;
        while (cur) {
            if (key == cur->key) return nullptr;
            parent = cur;
            cur = key < cur->key ? cur->l : cur->r;
        }
        BNode* n = newNode(key);
        n->p = parent;
        if (!parent) root_ = n;
        else if (key < parent->key) parent->l = n;
        else parent->r = n;
        return n;
    }

    // Выкинуть z из дерева (схема CLRS: при двух детях на место z
    // ПЕРЕВЕШИВАЕТСЯ преемник — узел сохраняет id, и в анимации «едет» вверх).
    // x — узел, занявший освободившееся место (может быть nullptr),
    // xp — его родитель, removedColor — цвет фактически убранного узла.
    BNode* removeStructure(BNode* z, BNode*& x, BNode*& xp, NodeColor& removedColor) {
        BNode* y = z;
        removedColor = y->color;
        if (!z->l) {
            x = z->r; xp = z->p; transplant(z, z->r);
        } else if (!z->r) {
            x = z->l; xp = z->p; transplant(z, z->l);
        } else {
            y = minimum(z->r);
            removedColor = y->color;
            x = y->r;
            if (y->p == z) {
                xp = y;
            } else {
                xp = y->p;
                transplant(y, y->r);
                y->r = z->r;
                y->r->p = y;
            }
            transplant(z, y);
            y->l = z->l;
            y->l->p = y;
            y->color = z->color;
            y->aux = z->aux;
        }
        return y;
    }

    void emit(StepKind kind, const std::string& text,
              std::initializer_list<const BNode*> focus = {}, bool danger = false) {
        if (!steps_) return;
        Step st;
        st.kind = kind;
        st.text = text;
        st.danger = danger;
        st.snap = snapshot();
        for (const BNode* n : focus)
            if (n) st.focus.push_back(n->id);
        steps_->push_back(std::move(st));
    }

    BNode* root_ = nullptr;
    std::vector<Step>* steps_ = nullptr;
    int nextId_ = 1;
    std::deque<BNode> pool_;   // deque: указатели на элементы не инвалидируются

protected:
    int build(const BNode* n, Snapshot& s) const {
        SnapNode sn;
        sn.id = n->id;
        sn.keys = {n->key};
        sn.color = n->color;
        const int hl = n->l ? build(n->l, s) : 0;
        const int hr = n->r ? build(n->r, s) : 0;
        if (n->l || n->r) sn.children = {n->l ? n->l->id : -1, n->r ? n->r->id : -1};
        const int h = 1 + std::max(hl, hr);
        sn.tag = tagFor(n, h, hl - hr);
        s.nodes[n->id] = std::move(sn);
        return h;
    }
};
