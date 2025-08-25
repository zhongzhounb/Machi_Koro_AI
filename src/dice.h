#ifndef DICE_H
#define DICE_H
#include "global.h"

class Dice: public QObject{
    Q_OBJECT
public:
    explicit Dice(QObject* parent);
    ~Dice();

    int getFirstNum () const { return m_firstNum; }
    int getSecondNum() const { return m_secondNum; }
    int getSum() const { return m_firstNum + m_secondNum + m_addNum ;}

    void setFirstNum(int num){m_firstNum=num;}
    void setSecondNum(int num){m_secondNum=num;}

    //抛骰子（禁用）
    void rollDice(int diceNum);
    //重置
    void clearDice();
    //使用港口
    void setAddNum(int num);
signals:
    //当抛新的骰子时，会返回新的骰子数（会同时产生dicesSumChanged信号）
    void dicesChanged(Dice* dice);


private:
    int m_firstNum;
    int m_secondNum;
    int m_addNum;
};

#endif // DICE_H
