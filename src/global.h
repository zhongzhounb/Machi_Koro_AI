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
#include <QColor>

const int MAX_PLAYER_NUM=5;
//初始化锁，希望之后优化掉
extern bool Initialized;

enum class ShowType{
    BackGround,//只展示背景
    Rough,//只展示背景、激活范围和名字
    Ordinary,//除了描述都展示
    Detailed//展示所有
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
    case Type::Landmark:
        return "🗼";
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
    InitGame=0,//初始化游戏

    StartTurn=10,//游戏开始

    RollDice=100,//⚠第一步，抛骰子
    RerollDice=110,//⚠【广播塔】重抛选项
    AddDiceNum=120,//⚠【港口】点数加二选项

    ActivateCards=200,//第二步，执行卡牌效果

    /*红卡效果*/
    StealCoins=210,//【红卡*6】先执行偷钱（所有红卡）

    /*蓝绿卡效果*/
    GainCoins=230,//【蓝卡*6】【绿卡*7】再执行获得钱（包含组合运算）（除特殊效果的所有蓝卡和所有绿卡）
    GainCoinsWithDices=231,//【金枪鱼船】根据骰子数获得钱
    CloseLandmark=240,//⚠【拆迁公司】关闭地标建筑
    GiveCard=241,//⚠【搬家公司】赠送卡

    /*紫卡效果*/
    StealCoinsFromAll=250,//【紫卡*4】偷所有人的钱（包含不同偷钱运算）
    SwapCard=260,//⚠【商业中心】交换牌

    GainOneCoinIfNoCoins=299,//买卡钱没钱，获得1元
    BuyCard=300,//⚠第三步，买卡
    CheckGameEnd=305,//【结算】检查游戏是否结束（购买后）
    GainCoinsIfNoBuy=310,//【机场】没买卡获得10元
    InvestTechnologyCompany=320,//⚠【科技公司】显示是否投资1元
    GainNewTurn=330,//【游乐园】判断是否再来一轮

};

struct OptionData {
    int id;                             // 选项的唯一标识符（按钮ID、卡牌ID、玩家ID）
    QString name;                       // 选项的显示名称（按钮文本、卡牌名称、玩家名称）
    int state;                   // 是否可以点击/选择（0不可点击，1可点击，2已点击）
    QString unClickMessage;         // 如果不可点击，显示的原因
};

class Player;
class Card;
struct PromptData {
    enum PromptType {
        None,           // 无需提示，用于命令完成或内部状态
        Popup,          // 弹窗，通常用于确认、提示信息，选项是按钮
        SelectCard,     // 选择卡牌，选项是卡牌信息
        SelectPlayer,    // 选择玩家，选项是玩家信息
        SelectDice,      // 选择骰子
        StartTurnAnimation,//游戏开始动画
        DiceAnimation,   //骰子动画
        BuyCardAnimation, //买卡动画
        CardInAnimation, //卡牌出现动画
        CardOutAnimation,//卡牌退回动画
        LoadingAnimation,//加载动画
    };
    PromptType type=PromptData::None;
    QString promptMessage="";              // 提示消息，例如“请投掷骰子”，“你的结果是x，y，你可以重抛”
    QList<OptionData> options={};          // 选项列表
    bool isAutoInput=true;//自动选择（如果为true，说明AI或真人玩家托管，无视delay，2s返回；如果为false，则说明是真人，则用户选择后立即返回，超时则返回自动选择结果）
    int autoInput=1;//自动选择结果（默认1代表确认，非确认需要接ai）
    int delay=10000;//多长时间返回（必须大于0）（目前是单机，如果是真人，则无限等待）
    QString waitMessage="";//玩家等待ai执行信息

    QList<int> diceNum;//【骰子动画】记录每个骰子是数字几
    Player* buyer;//【买卡动画】购买者
    Card* card;//【买卡动画】操作卡
    QList<Card*>cards;//【卡牌出现/退回动画】卡牌移动动画
};

struct CharacterData {
    int id;
    QString name;
    QString engName;
    QString imagePath;
    QString description;
    QColor mainColor;
};

#endif // GLOBAL_H


