#pragma once
#include "nodebase.h"

// Красно-чёрное дерево (схема CLRS). Каждый шаг перекраски и поворота — отдельный кадр.
class RbTree : public BinaryTreeBase {
public:
    TreeType type() const override { return TreeType::Rb; }
    bool insert(int key, std::vector<Step>& steps) override;
    bool erase(int key, std::vector<Step>& steps) override;

private:
    static bool isBlack(const BNode* n) { return !n || n->color == NodeColor::Black; }
    void insertFixup(BNode* z);
    void eraseFixup(BNode* x, BNode* xp);
};
