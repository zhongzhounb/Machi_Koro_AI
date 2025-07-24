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

    //近期收益计算（状态有关）
    double getCardRecentValue(Card* card,Player* owner,GameState* state);
    //未来收益计算（状态无关（定值））
    double getCardFutureValue(Card* card);
    //计算某个玩家最后执行点数的概率
    QMap<int,double>getPointNumProb(Player* player,GameState* state);


private:
    //抛一个骰子的期望
    double oneDiceEx();
    //抛两个骰子的期望（已计算【港口】效益）
    double twoDiceEx(bool hasHarbor);
    QMap<int,double>m_PointNumToValue;
};


#endif // AI_H
