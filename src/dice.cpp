#include "dice.h"
#include "randomutils.h"

Dice::Dice(QObject* parent): QObject(parent),firstNum(0),secondNum(0),addNum(0){}

Dice::~Dice(){};

//抛骰子
void Dice::rollDice(int diceNum){
    firstNum=RandomUtils::getInt(1,6);
    if(diceNum>1)
        secondNum=RandomUtils::getInt(1,6);
};

//重置
void Dice::clearDice(){
    firstNum=0;
    secondNum=0;
    addNum=0;
};

//使用港口
void Dice::addTwo(){
    addNum=2;
};

