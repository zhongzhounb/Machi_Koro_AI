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
#include <QMapIterator>

const int MAX_PLAYER_NUM=5;

enum class ShowType{
    BackGround,//只展示背景
    Rough,//只展示背景、激活范围和名字
    Ordinary,//除了描述都展示
    detailed//展示所有
};

enum class Color{
    Landmark=0,   //地标
    Red=1,       //红卡
    Blue=2,      //蓝卡
    Green=3,     //绿卡
    Purple=4,    //紫卡
    BackNone=5,
    BackSmall=6,
    BackBig=7,
    BackPurple=8
};

enum class Type{
    None,
    Agriculture ,//农业
    Husbandry,  //畜牧业
    Industry,   //工业
    Fishery,    //渔业
    Store,      //商店
    Factory,    //工厂
    Company,    //公司
    Restaurant, //餐厅(所有红卡）
    Office,     //大型建筑（所有紫卡）
    Landmark
};

inline QString typeToImg(Type type){
    switch (type) {
    case Type::Agriculture:
        return "🌾";
    case Type::Husbandry:
        return "🐄";
    case Type::Industry:
        return "⚙️";
    case Type::Restaurant:
        return "☕";
    case Type::Store:
        return "🏪";
    case Type::Office:
        return "🗼";
    case Type::Fishery:
        return "⛵";
    case Type::Factory:
        return "🏭";
    case Type::Company:
        return "💼";
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
    InitGame=000,//初始化游戏

    StartTurn=010,//游戏开始

    RollDice=100,//⚠第一步，抛骰子
    RerollDice=110,//⚠【广播塔】重抛选项
    AddDiceNum=120,//⚠【港口】点数加二选项

    ActivateCards=200,//第二步，执行卡牌效果

    /*红卡效果*/
    StealCoins=210,//【红卡*6】先执行偷钱（所有红卡）

    /*蓝绿卡效果*/
         //【贷款公司】这里可能有扣钱的绿卡（目前没加入）
    GainCoins=230,//【蓝卡*6】【绿卡*2】再执行获得钱（所有蓝卡和部分绿卡）
    GainCoinsWithDices=231,//【金枪鱼船】根据骰子数获得钱
    GainCoinsComboName=232,//【花店】和名称组合的绿卡
    GainCoinsComboType=233,//【绿卡*4】和类型组合的绿卡
    CloseLandmark=240,//⚠【拆迁公司】关闭地标建筑
    GiveCard=241,//⚠【搬家公司】赠送卡

    /*紫卡效果*/
    StealCoinsFromAll=250,//【体育馆】【科技公司】偷所有人的钱
    StealCoinsHalfFromWealthy=251,//【税务所】偷所有人满10的一半钱
    StealCoinsComboRedStore=252,//【出版社】偷所有人和咖啡/店面组合的钱
    SwapCard=260,//⚠【商业中心】交换牌

    GainOneCoinIfNoCoins=299,//买卡钱没钱，获得1元
    BuyCard=300,//⚠第三步，买卡
    GainCoinsIfNoBuyCard=310,//【机场】没买卡就获得10元
    InvestTechnologyCompany=320,//⚠【科技公司】显示是否投资1元
    GainNewTurn=330,//【游乐园】判断是否再来一轮

};

#endif // GLOBAL_H


