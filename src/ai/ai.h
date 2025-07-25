#ifndef AI_H
#define AI_H
#include"global.h"
class Card;
class Player;
class GameState;
class AI: public QObject
{
    Q_OBJECT
public:
    explicit AI(QObject* parent = nullptr);
    ~AI(){};

    void update(GameState* state);
    //获取应该投掷的骰子数（比较两个DiceEx）
    int getRollDiceNum(Player* player);
    //获取重抛的骰子数（0为保持)（用value比较DiceEx，游乐园则用value+roundValue>=0）
    int getReRollDiceNum(Player* player,int num1,int num2);
    //获取是否要+2（比较value）
    int getAddDiceNum(Player* player,int sum);





private:
    struct Date{
        int OneDiceEx;//抛1期望值
        int TwoDiceEx;//抛2骰子期望值（含港口、游乐园）（广播塔是对比期望后再投出，所以不含）
        QList<double>value;
        QList<double>prob;

    };
    //通过模拟计算每个玩家的某个点数的收益
    QMap<Player*,Date>m_date;
    //回合价值
    double m_roundValue;

};


#endif // AI_H

