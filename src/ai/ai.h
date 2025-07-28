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
    //获取买卡Id
    int getBuyCardId(PromptData pd,Player* player,GameState* state);


private:
    double simulate(Player* player,int sum,GameState* state);
    struct Data{
        double coins=0;//当前金币，虽然player->getCoins能获得，但是可以假设有这么多金币
        double OneDiceEx=0;//抛1期望值
        double TwoDiceEx=0;//抛2骰子期望值（含港口、游乐园）（广播塔是对比期望后再投出，所以不含）
        double roundEx=1.0;//回合数期望（没开游乐园值为1，开了如果全部都想抛大致为1.16）
        QList<double>value=QList<double>(15, 0.0);
        QList<double>prob=QList<double>(15, 0.0);
        double lastCardMaxValue=2.0;
        double lastCardMinValue=2.0;
    };
    //通过模拟计算每个玩家的某个点数的收益
    QMap<Player*,Data>m_data;
    //通过给定概率模拟计算每个玩家未来的某个点数的收益
    QMap<Player*,Data>m_dataForFuture;
    //回合价值
    double m_roundValue=0.0;
    double m_futurePercentage=0.0;
    double getCardFutureEx(Card* card,Player* owner,GameState*state);
    double getCardEx(Card* card,Player* player,GameState*state,bool isRecent=true);
    double comboEx(Player* owner,QString name,GameState* state);
    double getIncome(Card* card,Player* owner,Player* activePlayer,GameState* state,int count=-1);

};


#endif // AI_H

