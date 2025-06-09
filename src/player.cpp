#include "player.h"

// 构造函数
Player::Player(int id, const QString& name, AIRank aiRank, QObject* parent)
    : QObject(parent), m_id(id), m_name(name), m_aiRank(aiRank), m_coins(0)
{

}

Player::~Player(){}

//初始化地标建筑
void Player::addLandmark(Card* landmark){
    m_landmarks.append(landmark);
}

// 赚钱
void Player::addCoins(int amount){
    m_coins+=amount;
}

// 亏钱（不负责判负）
void Player::delCoins(int amount){
    m_coins-=amount;
}

// 添加卡牌
void Player::addCard(Card* card){
    //先找有没有同类卡
    for(QList<Card*>& currentStack : m_cards)  // 使用引用以便修改栈
        if(currentStack.takeFirst()->getName() == card->getName()){
            currentStack.push_front(card);
            return ;
        }


    //没有同类卡就创建一个新栈
    QList<Card*> newstack;
    newstack.push_front(card);
    m_cards.push_back(newstack);
}

// 移除卡牌（没处理没找到的情况）
void Player::delCard(Card* card){
    //因为同种卡牌效果相同，只允许动栈顶的卡，所以直接操作栈顶
    for(QList<Card*>& m_card:m_cards)
        if(m_card.takeFirst()->getName()==card->getName()){
            m_card.pop_front();
            break;
        }
    //移除空栈
    for(int i=0;i<m_cards.size();i++)
        if(m_cards[i].isEmpty()){
            m_cards.removeAt(i);
            return;
        }
}

// 设置卡牌/地标状态
void Player::setCardState(Card* card,State state){
    //先看是否是地标建筑
    for(Card* currentCard:m_landmarks)
        if(currentCard->getId()==card->getId()){
            currentCard->setState(state);
            return;
        }

    //再看是否是手牌（含栈中元素）
    for(QList<Card*>& m_card:m_cards)
        for(Card* currentCard:m_card)
            if(currentCard->getId()==card->getId()){
                currentCard->setState(state);
                return;
            }

}

// 查看地标建筑是否激活
bool Player::isLandmarkOpening(QString name){
    for(Card* currentCard:m_landmarks)
        if(currentCard->getName()==name)
            return currentCard->getState()==State::Opening;
    //错误
    return false;
}
