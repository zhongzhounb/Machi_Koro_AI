#include "player.h"
#include "card.h"
// 初始化静态成员变量
int Player::s_playerId = 1;

// 构造函数
Player::Player(const QString& name, AIRank aiRank, QObject* parent)
    : QObject(parent), m_name(name), m_aiRank(aiRank), m_coins(0)
{
    m_id=s_playerId++;
}

Player::~Player(){}

// 赚钱
void Player::addCoins(int amount){
    m_coins+=amount;
    emit coinsChange(this,m_coins);
}

// 亏钱（不负责判负）
void Player::delCoins(int amount){
    m_coins-=amount;
    emit coinsChange(this,m_coins);
}

// 添加卡牌
void Player::addCard(Card* card){
    //先找有没有同类卡
    for(QList<Card*>& currentStack : m_cards)  // 使用引用以便修改栈
        if(currentStack.first()->getName() == card->getName()){
            currentStack.push_front(card);
            emit cardAdded(this,card);
            return ;
        }

    //没有同类卡就创建一个新栈
    QList<Card*> newstack;
    newstack.push_front(card);
    m_cards.push_back(newstack);
    emit cardAdded(this,card);
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

int Player::getCardNum(QString name,State state){
    int num=0;
    for(QList<Card*>& m_card:m_cards)
        for(Card* currentCard:m_card)
            if(name==currentCard->getName()&&(state==State::None||state==currentCard->getState()))
                num++;
    return num;
}

int Player::getTypeCardNum(Type type,State state){
    int num=0;
    for(QList<Card*>& m_card:m_cards)
        for(Card* currentCard:m_card)
            if(type==currentCard->getType()&&(state==State::None||state==currentCard->getState()))
                num++;
    return num;
}

void Player::stealCoins(Player* player,int amount){
    //错误
    if(player->getCoins()<amount)
        return;

    this->addCoins(amount);
    player->delCoins(amount);
}

GameCommand* Player::getCardSpecialCommand(QString name){
    for(QList<Card*> cards:m_cards)
        if(cards.first()->getName()==name)
            return cards.first()->createSpecialCommand(this);
    return nullptr;
}

