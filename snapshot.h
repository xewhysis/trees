#pragma once
#include <string>
#include <unordered_map>
#include <vector>

// Цвет узла. Plain — «обычный» узел (AVL, Treap, Splay, B-дерево).
enum class NodeColor { Plain, Red, Black };

// Один узел в снимке. id стабилен между кадрами: именно по нему сцена
// понимает, что «этот же» узел переехал при повороте, а не исчез и появился.
struct SnapNode {
  int id = -1;
  std::vector<int>
      keys; // у бинарных деревьев ровно один ключ, у B-дерева несколько
  NodeColor color = NodeColor::Plain;
  std::vector<int>
      children;    // id детей; -1 = пустой слот (нужно бинарным деревьям, чтобы
                   // отличать «только левый» от «только правый»)
  std::string tag; // мелкая подпись над узлом (баланс AVL, приоритет Treap)
};

// Снимок всего дерева на один кадр анимации.
struct Snapshot {
  int root = -1;
  std::vector<int>
      extraRoots; // «плавающие» деревья рядом с основным (split/merge у Treap)
  bool multiKey = false; // true для B-дерева (узел — прямоугольник с ячейками)
  std::unordered_map<int, SnapNode> nodes;
  bool empty() const { return root < 0; }
};
