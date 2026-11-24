#pragma once
#include "snapshot.h"
#include <string>
#include <vector>

enum class StepKind {
  Info,
  Insert,
  Remove,
  Rotate,
  Recolor,
  Split,
  Merge,
  Rebalance,
  Done
};

// Один шаг операции = подпись + полное состояние дерева ПОСЛЕ этого шага.
// Сцена сама интерполирует положение узлов между соседними шагами.
struct Step {
  StepKind kind = StepKind::Info;
  std::string text; // что сейчас происходит (показывается в окне)
  Snapshot snap;
  std::vector<int> focus;     // id узлов, которые подсвечиваем
  std::vector<int> focusKeys; // B-дерево: конкретные ключи внутри узла
  bool danger = false;        // красная подсветка (удаление, дисбаланс)
};
