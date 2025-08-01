#include "dice.h"
#include "randomutils.h"

Dice::Dice(QObject* parent): QObject(parent),m_firstNum(0),m_secondNum(0),m_addNum(0){}

Dice::~Dice(){};

//抛骰子
void Dice::rollDice(int diceNum){
    Dice::clearDice();
    m_firstNum=RandomUtils::instance().generateInt(1,6);
    if(diceNum==2)
        m_secondNum=RandomUtils::instance().generateInt(1,6);
    emit dicesChanged(this);
};

//重置
void Dice::clearDice(){
    m_firstNum=0;
    m_secondNum=0;
    m_addNum=0;
    emit dicesChanged(this);
};

//使用港口
void Dice::setAddNum(int num){
    m_addNum=num;
    emit dicesChanged(this);
};

