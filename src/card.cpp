#include"card.h"

// 初始化静态成员变量
int Card::s_cardId = 0;
int Card::s_cardNameId = 0;
QMap<QString,int> Card::s_cardNameToId;

Card::Card(const QString& name, int cost, Color color, Type type, int value,
              int actLNum , int actRNum , QObject* parent ,State state):
    m_name(name),m_cost(cost),m_color(color),m_type(type),m_value(value),m_actLNum(actLNum),
    m_actRNum(actRNum),QObject(parent),m_state(state){
    //自动获取卡id
    m_id = s_cardId++;
    //自动获取卡名称id（排序用）
    if(s_cardNameToId.find(name)==s_cardNameToId.end()){
        s_cardNameToId[name]=s_cardNameId++;
    }
    m_nameId=s_cardNameToId[name];
};

//之后肯定有发信号的
void Card::setState(State state) { m_state = state; }


bool Card::isActivate(Player* owner, Player* activePlayer,int rollSum) const {
    return rollSum >= m_actLNum && rollSum <= m_actRNum;
}
