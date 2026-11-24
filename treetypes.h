#pragma once

// Типы деревьев, доступные в интерфейсе.
enum class TreeType { Avl, Rb, Treap, Splay, BTree };

inline constexpr TreeType kAllTreeTypes[] = {TreeType::Avl, TreeType::Rb,
                                             TreeType::Treap, TreeType::Splay,
                                             TreeType::BTree};

inline const char *treeTitle(TreeType t) {
  switch (t) {
  case TreeType::Avl:
    return "AVL-дерево";
  case TreeType::Rb:
    return "Красно-чёрное дерево";
  case TreeType::Treap:
    return "Декартово дерево (Treap)";
  case TreeType::Splay:
    return "Splay-дерево";
  case TreeType::BTree:
    return "B-дерево";
  }
  return "";
}
