#pragma once
#include "nodebase.h"

// AVL: после каждой вставки/удаления идём вверх и поворотами
// восстанавливаем |баланс| <= 1. Над узлом показывается баланс (h_left - h_right).
class AvlTree : public BinaryTreeBase {
public:
    TreeType type() const override { return TreeType::Avl; }
    bool insert(int key, std::vector<Step>& steps) override;
    bool erase(int key, std::vector<Step>& steps) override;

protected:
    std::string tagFor(const BNode*, int height, int balance) const override;

private:
    static int ht(const BNode* n) { return n ? n->aux : 0; }
    static void upd(BNode* n) { n->aux = 1 + std::max(ht(n->l), ht(n->r)); }
    void rot(BNode* x, bool left, const std::string& text);
    void rebalanceFrom(BNode* v);
};
