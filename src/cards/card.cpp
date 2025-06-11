#include"card.h"

// 初始化静态成员变量
int Card::s_nextId = 0;


Card::Card(const QString& name, int cost, Color color, Type type,
              int actLNum , int actRNum , const QString& description ,
              QObject* parent ,State state):
    m_name(name),m_cost(cost),m_color(color),m_type(type),m_actLNum(actLNum),
    m_actRNum(actRNum),m_description(description),QObject(parent),m_state(state){
    m_id = ++s_nextId;
      };

void Card::setState(State state) { m_state = state; }


bool Card::isActivate(int rollSum) const {
    if (m_actLNum == 0 && m_actRNum == 0) return false;
    return rollSum >= m_actLNum && rollSum <= m_actRNum;
}

// 获得选项
QVariantList Card::getChoices(Player* owner, Player* activePlayer, Game* game) const {
    Q_UNUSED(owner); Q_UNUSED(activePlayer); Q_UNUSED(game);
    return {}; // 默认返回空列表，表示该卡牌在掷骰子激活后不需要玩家选择
}

// 自动推荐选项
QVariant Card::autoChoices(Player* owner, Player* activePlayer, Game* game,QVariantList choices) const {
    Q_UNUSED(owner); Q_UNUSED(activePlayer);  Q_UNUSED(game);Q_UNUSED(choices);
    return {}; // 默认返回空列表，表示该卡牌在掷骰子激活后不需要玩家选择
}
