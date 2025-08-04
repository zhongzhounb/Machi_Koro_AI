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
         int actLNum, int actRNum,CommandType activateType, QObject* parent = nullptr,
        QString comboLog="",QString noneLog="",State state=State::Opening,CommandType specialType=CommandType::None);
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
    CommandType getActivateType() const {return m_activateType;}
    CommandType getSpecialType() const {return m_specialType;}
    QString getComboLog() const { return m_comboLog; }
    QString getNoneLog() const { return m_noneLog; }

    //设置卡牌状态
    void setState(State state);
    //改变值
    void changeValue(int value);
    //判断是否在该范围
    bool isActivate(Player* owner, Player* activePlayer,int rollSum)const ;
    //
    virtual QString getDescription() const{return "";}

    /*以下是特殊卡牌的*/
    //组合卡、前置条件卡：只有GainCoins可以调用，达成前置条件（比如港口)/组合数量为0则0，无前置组合则1，有combo数量则1+。
    virtual int getComboNum(Player* owner, Player* activePlayer,GameState* gameState)const {return 1;}
    //群体收钱紫卡：只有StealCoinsFromAll可以调用，一搬就是收取定值m_value，动态收取需要override。
    virtual int getStealCoins(Player* player)const{return m_value;}


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
    CommandType m_activateType;//点数激活后产生的命令
    CommandType m_specialType;//非点数激活产生的特殊命令
    QString m_comboLog;//组合命令日志
    QString m_noneLog;//卡牌激活但无效日志

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
