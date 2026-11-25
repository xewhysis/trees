#pragma once
#include "nodebase.h"

// Splay-дерево: любой доступ к ключу поднимает его в корень через zig / zig-zig / zig-zag.
class SplayTree : public BinaryTreeBase {
public:
    TreeType type() const override { return TreeType::Splay; }
    bool insert(int key, std::vector<Step>& steps) override;
    bool erase(int key, std::vector<Step>& steps) override;

private:
    // Поднимает x, пока его родителем не станет stop (nullptr = до корня).
    void splayTo(BNode* x, BNode* stop);
};
