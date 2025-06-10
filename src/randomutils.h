#ifndef RANDOMUTILS_H
#define RANDOMUTILS_H

#include <QRandomGenerator> // 引入 QRandomGenerator

namespace RandomUtils { // 使用命名空间避免全局命名冲突

// 生成一个在 [min, max] 范围内的随机整数 (包含 min 和 max)
// QRandomGenerator::bounded(low, high) 生成 [low, high) 范围的数 (不包含 high)
inline int getInt(int min, int max) {
    return QRandomGenerator::global()->bounded(min, max + 1);
}

// 生成一个在 [min, max) 范围内的随机浮点数 (包含 min，不包含 max)
// QRandomGenerator::generateDouble() 生成 [0.0, 1.0) 范围的 double
inline double getDouble(double min, double max) {
    return min + (max - min) * QRandomGenerator::global()->generateDouble();
}

} // namespace RandomUtils

#endif // RANDOMUTILS_H
