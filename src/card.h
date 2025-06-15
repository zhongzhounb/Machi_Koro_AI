#ifndef CARD_H
#define CARD_H
#include"global.h"
class Player;
class GameState;
class GameCommand;
class GameController;

class Card: public QObject
{
    Q_OBJECT
public:
    explicit Card(const QString& name, int cost, Color color, Type type,int value,
         int actLNum = 0, int actRNum = 0, QObject* parent = nullptr,State state=State::Opening);
    ~Card(){};

    int getId() const { return m_id ;}
    QString getName() const { return m_name; }
    int getCost() const { return m_cost; }
    Color getColor() const { return m_color; }
    Type getType() const { return m_type; }
    int getValue() const { return m_value; }
    int getActLNum() const { return m_actLNum; }
    int getActRNum() const { return m_actRNum; }
    State getState() const { return m_state; }

    //设置卡牌状态
    void setState(State state);
    //获得卡牌购买权重（用以AI计算）
    virtual double getBuyWight(Player* aiPlayer, GameState* gameState) const = 0;
    //判断是否能激活该卡
    virtual bool isActivate (Player* owner, Player* activePlayer, int rollSum)const;
    // 统一的激活函数
    virtual QList<GameCommand*> createCommands(Player* owner, Player* activePlayer,GameController* controller)= 0;

    //用以UI显示
    virtual QString getDescription() const=0;

protected:
    int m_id;
    QString m_name;
    int m_actLNum;
    int m_actRNum;
    int m_cost;
    int m_value;
    Color m_color;
    Type m_type;
    State m_state;

private:
    // 静态成员：用于生成唯一的实例ID
    static int s_nextId;
};

#endif // CARD_H
