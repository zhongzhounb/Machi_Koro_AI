#ifndef CARD_H
#define CARD_H
#include"global.h"
class Player;
class GameState;
class GameCommand;

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
    int getNameId() const { return m_nameId; }

    //设置卡牌状态
    void setState(State state);
    //改变值
    void changeValue(int value);
    //判断是否在该范围
    virtual bool isActivate(Player* owner, Player* activePlayer,int rollSum)const ;
    //在该范围，但是是否达成前置条件（比如建造港口)/组合数量（即：未达成前置/组合数量为0则0，无前置组合则1，有combo数量则1+。最后用comboNum*value*cardNum
    int getComboNum(Player* owner, Player* activePlayer,GameState* gameState)const {return 1;}
    //获得特殊命令（基本用在地标建筑上）
    virtual GameCommand* createSpecialCommand(Player* owner){ return nullptr;}
    //获得卡牌购买权重（用以AI计算）
    virtual double getBuyWight(Player* aiPlayer, GameState* gameState) const {return 0.0;};
    // 创建命令
    virtual QList<GameCommand*> createCommands(Player* owner, Player* activePlayer){return {};};
    // 用以UI显示
    virtual QString getDescription() const{return "";}

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
    int m_nameId;

signals:
    //当状态变化，UI需要改变卡的正反面
    void cardStateChanged(Card* card,State newState);
    //当值变化，UI需要重新getDescription()
    void cardValueChanged(Card* card);


private:
    // 静态成员：用于生成唯一的实例ID
    static int s_cardId;
    // 静态成员：用户生成唯一的卡牌名称ID
    static int s_cardNameId;
    // 静态成员：名称映射
    static QMap<QString,int>s_cardNameToId;
};

#endif // CARD_H
