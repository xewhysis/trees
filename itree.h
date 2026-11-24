#pragma once
#include "operationsteps.h"
#include "treetypes.h"
#include <vector>

// Интерфейс дерева. Деревья ничего не знают про Qt: операция просто
// складывает в `steps` последовательность шагов, а UI их проигрывает.
class ITree {
public:
  virtual ~ITree() = default;
  virtual TreeType type() const = 0;
  virtual void clear() = 0;
  virtual bool contains(int key) const = 0;
  // false — ключ уже есть / не найден (в steps всё равно будет шаг-пояснение).
  virtual bool insert(int key, std::vector<Step> &steps) = 0;
  virtual bool erase(int key, std::vector<Step> &steps) = 0;
  virtual Snapshot snapshot() const = 0;
};
