#ifndef RANDOMUTILS_H
#define RANDOMUTILS_H

#include"global.h"
#include <random> // 包含 C++11 的随机数库

class RandomUtils : public QObject { // 可以继承 QObject 以便信号槽或父子管理
    Q_OBJECT
public:
    // 获取单例实例的静态方法
    static RandomUtils& instance();

    // 获取当前使用的种子 (用于序列化/回放)
    unsigned int getSeed() const { return m_seed; }
    // 用于洗牌 std::shuffle
    std::mt19937& getEngine() { return m_rngEngine; }

    // 默认生成了基于时间的种子，建议别自己setSeed()，可以通过getSeed()查看当前种子
    void setSeed(unsigned int seed);
    // 生成指定范围内的整数随机数 [min, max]
    int generateInt(int min, int max);


private:
    //单例模式
    explicit RandomUtils(QObject* parent = nullptr);
    RandomUtils(const RandomUtils&) = delete;
    RandomUtils& operator=(const RandomUtils&) = delete;

    std::mt19937 m_rngEngine; // Mersenne Twister 引擎
    unsigned int m_seed;      // 当前使用的种子
    // 生成一个随机种子 (基于时间)
    static unsigned int generateRandomSeed();
};

#endif // RANDOMUTILS_H
