#ifndef GLOBAL_H
#define GLOBAL_H

#include <QObject>
#include<Qvector>
#include<QString>
#include <QVariant>
#include <QDebug>
#include <QList>
#include <QMap>
#include <QStack>
#include <QPointer>

const int MAX_PLAYER_NUM=5;

enum class Color{
    Landmark=0,   //地标
    Red=1,       //红卡
    Blue=2,      //蓝卡
    Green=3,     //绿卡
    Purple=4    //紫卡
};

enum class Type{
    Agriculture ,//农业
    Husbandry,  //畜牧业
    Industry,   //工业
    Fishery,    //渔业
    Store,      //商店
    Factory,    //工厂
    Company,    //公司
    Restaurant, //餐厅（一般用颜色表示）
    Office,     //大型建筑（一般用颜色表示）
    Landmark    //地标（一般用颜色表示）
};

QString typeToImg(Type type){
    switch (type) {
    case Type::Agriculture:
        return "🌾";
    case Type::Husbandry:
        return "🐄";
    case Type::Industry:
        return "⚙️";
    case Type::Restaurant:
        return "🥤";
    case Type::Store:
        return "🏪";
    default:
        return "InvalidType";
    }
}

enum class State {
    None,       //无状态
    Opening,    //开业中
    Closing     //停业中
};

enum class AIRank {
    None,       // 人类玩家
    Easy,       // 简单AI
    Medium,     // 中等AI
    Hard        // 困难AI
};

enum CommandStatus {
    Pending,    // 待执行
    Executing,  // 正在执行 (可能需要等待外部输入)
    Completed,  // 已完成
    Skipped     // 已跳过 (例如玩家选择跳过某个可选动作)
};

enum CommandType{
    None=-1,//空命令
    StartTurn=000,//游戏开始
    RollDice=100,//第一步，抛骰子
    RerollDice=110,
    AddDiceNum=120,
    CreateCard=200,//第二步，执行卡牌效果
    StealCoins=210,//先执行偷钱（所有红卡）
    //这里可能有扣钱的绿卡（如贷款公司）
    GainCoins=230,//再执行获得钱（所有蓝卡和部分绿卡）
    GainCoinsComboName=231,//和名称组合的绿卡
    GainCoinsComboType=232,//和类型组合的绿卡（可选自己类型还是全场类型）


    BuyCard=300,//第三步，买卡

};
//卡牌排序规则：
//优先级在红>蓝/绿>紫的情况下，红卡座位顺序，蓝/绿和紫：扣钱（贷款公司）＞加钱＞特殊效果（比如拆迁公司等），再蓝＞绿，且同种类牌接近

#endif // GLOBAL_H


