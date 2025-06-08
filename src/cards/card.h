#ifndef CARD_H
#define CARD_H
#include"global.h"
class Player;
class Game;

class Card: public QObject
{
    Q_OBJECT
public:
    explicit Card(int id,const QString& name, int cost, Color color, Type type,
         int actLNum = 0, int actRNum = 0, const QString& description = "",
         QObject* parent = nullptr,State state=State::Opening):m_id(id),
        m_name(name),m_cost(cost),m_color(color),m_type(type),m_actLNum(actLNum),
        m_actRNum(actRNum),m_description(description),QObject(parent),m_state(state){};
    ~Card();

    int getId() const { return m_id ;}
    QString getName() const { return m_name; }
    int getCost() const { return m_cost; }
    Color getColor() const { return m_color; }
    Type getType() const { return m_type; }
    int getActLNum() const { return m_actLNum; }
    int getActRNum() const { return m_actRNum; }
    QString getDescription() const { return m_description; }

    //可变状态量
    State getState() { return m_state; }
    void setState(State state) { m_state = state; }

    virtual double getBuyWight(Player* aiPlayer, Game* game) const = 0;

    bool isActivate(int rollSum) const {
        if (m_actLNum == 0 && m_actRNum == 0) return false;
        return rollSum >= m_actLNum && rollSum <= m_actRNum;
    }

    // 获得选项
    virtual QVariantList getChoices(Player* owner, Player* activePlayer, int rollSum, Game* game) const {
        Q_UNUSED(owner); Q_UNUSED(activePlayer); Q_UNUSED(rollSum); Q_UNUSED(game);
        return {}; // 默认返回空列表，表示该卡牌在掷骰子激活后不需要玩家选择
    }

    // 自动推荐选项
    virtual QVariant autoChoices(Player* owner, Player* activePlayer, int rollSum, Game* game,QVariantList choices) const {
        Q_UNUSED(owner); Q_UNUSED(activePlayer); Q_UNUSED(rollSum); Q_UNUSED(game);Q_UNUSED(choices);
        return {}; // 默认返回空列表，表示该卡牌在掷骰子激活后不需要玩家选择
    }

    // 统一的激活函数
    virtual QString activate(Player* owner, Player* activePlayer, int rollSum, Game* game, const QVariant& choiceData) = 0;

protected:
    int m_id;
    QString m_name;
    int m_actLNum;
    int m_actRNum;
    int m_cost;
    Color m_color;
    Type m_type;
    QString m_description;
    State m_state;
};

#endif // CARD_H
