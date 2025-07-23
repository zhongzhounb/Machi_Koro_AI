#include "dice.h"
#include "randomutils.h"

Dice::Dice(QObject* parent): QObject(parent),firstNum(0),secondNum(0),addNum(0){}

Dice::~Dice(){};

//抛骰子
void Dice::rollDice(int diceNum){
    Dice::clearDice();
    firstNum=RandomUtils::instance().generateInt(1,6);
    if(diceNum==2)
        secondNum=RandomUtils::instance().generateInt(1,6);
    emit dicesChanged(this);
};

//重置
void Dice::clearDice(){
    firstNum=0;
    secondNum=0;
    addNum=0;
    emit dicesChanged(this);
};
//使用港口
void Dice::setAddNum(int num){
    addNum=num;
    emit dicesChanged(this);
};

