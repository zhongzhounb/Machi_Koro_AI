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
    void setAddNum(int num);
signals:
    //当抛新的骰子时，会返回新的骰子数（会同时产生dicesSumChanged信号）
    void dicesChanged(int newNum1,int newNum2);
    //当加的值变化时，会返回新的值（比如【港口】让结果+2而让骰子数不变）
    void dicesSumChanged(int newSum);


private:
    int firstNum;
    int secondNum;
    int addNum;
};

#endif // DICE_H
