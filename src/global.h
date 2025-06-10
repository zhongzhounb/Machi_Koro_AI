#ifndef GLOBAL_H
#define GLOBAL_H

#include <QObject>
#include<Qvector>
#include<QString>
#include <QVariant>
#include <QDebug>
#include <algorithm>
#include <QList>
#include <QMap>
#include <QStack>
#include <QPointer>

enum class Color{
    Blue,      //蓝卡
    Green,     //绿卡
    Red,       //红卡
    Purple,    //紫卡
    Grey       //地标
};

enum class Type{
    Agriculture,//农业
    Husbandry,  //畜牧业
    Industry,   //工业
    Fishery,    //渔业
    Store,      //商店
    Factory,    //工厂
    Company,    //公司
    Restaurant, //餐厅（红卡）
    Office,     //大型建筑（紫卡）
    Landmark    //地标
};

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

#endif // GLOBAL_H


