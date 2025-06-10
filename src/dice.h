#ifndef DICE_H
#define DICE_H
#include "global.h"

class Dice: public QObject{
    Q_OBJECT
public:
    explicit Dice(QObject* parent);
    ~Dice();

    int getFirstNum () const { return firstNum; }
    int getSecondNum() const { return secondNum; }
    int getSum() const { return firstNum + secondNum + addNum ;}

    //抛骰子
    void rollDice(int diceNum);
    //重置
    void clearDice();
    //使用港口
    void addTwo();

private:
    int firstNum;
    int secondNum;
    int addNum;
};

#endif // DICE_H
