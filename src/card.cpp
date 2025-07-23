#include"card.h"


// 初始化静态成员变量
int Card::s_cardId = 1;
int Card::s_cardNameId = 1;
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
void Card::setState(State state) {
    m_state = state;
    emit cardStateChanged(this,state);
}

void Card::changeValue(int value){
    m_value+=value;
}

bool Card::isActivate(Player* owner, Player* activePlayer,int rollSum) const {
    //规定地标建筑激活按照状态判断！
    if(m_type==Type::Landmark)
        return m_state==State::Opening;

    if(rollSum >= m_actLNum && rollSum <= m_actRNum){
        if(m_color==Color::Blue)
            return true;
        else if(m_color==Color::Red&&owner!=activePlayer)
            return true;
        else if(m_color==Color::Green&&owner==activePlayer)
            return true;
        else if(m_color==Color::Purple&&owner==activePlayer)
            return true;
        else
            return false;
    }
    return false;
}


