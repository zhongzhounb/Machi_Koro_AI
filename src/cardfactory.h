#ifndef CARDFACTORY_H
#define CARDFACTORY_H

#include "global.h"
class Card;

class CardFactory : public QObject
{
    Q_OBJECT
public:
    explicit CardFactory(QObject* parent = nullptr); // 构造函数，可以接收一个父对象
    ~CardFactory();

    // 核心工厂方法：根据卡牌ID创建新的卡牌实例
    // parent 参数用于 Qt 对象树的内存管理
    static Card* createCardInstanceById(int cardId, QObject* parent = nullptr);
};

#endif // CARDFACTORY_H
