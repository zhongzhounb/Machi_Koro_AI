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

enum class Color{
    Blue,      //蓝卡
    Green,     //绿卡
    Red,       //红卡
    Purple,    //紫卡
    Landmark   //地标
};

enum class Type{
    Agriculture,//农业
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

};
//卡牌排序规则：
//优先级在红>蓝/绿>紫的情况下，红卡座位顺序，蓝/绿和紫：扣钱（贷款公司）＞加钱＞特殊效果（比如拆迁公司等），再蓝＞绿，且同种类牌接近

#endif // GLOBAL_H


