#include "randomutils.h"
#include <chrono> // 用于生成初始种子

RandomUtils::RandomUtils(QObject* parent)
    : QObject(parent)
    , m_seed(0) {
    setSeed(generateRandomSeed());
}

RandomUtils& RandomUtils::instance() {
    static RandomUtils s_instance; // 静态局部变量，只在第一次调用时创建
    return s_instance;
}

void RandomUtils::setSeed(unsigned int seed) {
    m_seed = seed;
    m_rngEngine.seed(m_seed); // 使用种子初始化引擎
}

int RandomUtils::generateInt(int min, int max) {
    // 创建一个均匀分布器，范围是 [min, max]
    std::uniform_int_distribution<int> distribution(min, max);
    return distribution(m_rngEngine); // 使用引擎生成随机数
}

unsigned int RandomUtils::generateRandomSeed() {
    // 使用当前时间作为种子，确保每次程序运行的初始序列不同
    return static_cast<unsigned int>(std::chrono::high_resolution_clock::now().time_since_epoch().count());
}
