#include "player.h"
#include "card.h"
#include <QTimer>
// 初始化静态成员变量
int Player::s_playerId = 1;

// 构造函数
Player::Player(const QString& name, AIRank aiRank, QObject* parent)
    : QObject(parent), m_name(name), m_aiRank(aiRank), m_coins(0)
{
    m_id=s_playerId++;
}

Player::~Player(){}

QList<Card*> Player::getCardsForName(QString name){
    for(QList<Card*> cards:m_cards)
        if(cards.last()->getName()==name)
            return cards;
    return {};
}

// 赚钱
void Player::addCoins(int amount){
    m_coins+=amount;
    emit coinsChange(this,m_coins,amount);
}

// 亏钱（不负责判负）
void Player::delCoins(int amount){
    m_coins-=amount;
    emit coinsChange(this,m_coins,-amount);
}

// 添加卡牌
void Player::addCard(Card* card){

     // 显式捕获 'this'
        //先找有没有同类卡
        for(QList<Card*>& currentStack : m_cards)  // 使用引用以便修改栈
            if(currentStack.first()->getName() == card->getName()){
                currentStack.push_back(card);
                //添加卡一定是从别的地方移动过来，所以晚1s的等待时间
                QTimer::singleShot(2000, this, [this,card](){
                    emit cardAdded(this,card);
                });
                return ;
            }

        //没有同类卡就创建一个新栈
        QList<Card*> newstack;
        newstack.push_back(card);
        m_cards.push_back(newstack);

        //购物中心特判
        if(this->getCardNum("购物中心",State::Opening)>0)
            if(card->getType()==Type::Store||card->getType()==Type::Restaurant)
                card->changeValue(1);

        QTimer::singleShot(2000, this, [this,card](){
            emit cardAdded(this,card);
        });


}

// 移除卡牌
void Player::delCard(Card* card){
    bool finded = false;
    for(QList<Card*>& m_card : m_cards) {
        if(!m_card.isEmpty() && m_card.last()->getName() == card->getName()){
            m_card.pop_back();
            finded = true;
            break;
        }
    }

    if(this->getCardNum("购物中心", State::Opening) > 0) {
        if(card->getType() == Type::Store || card->getType() == Type::Restaurant)
            card->changeValue(-1);
    }

    if(!finded) {
        qDebug() << "Player::delCard: 错误：未找到删除的卡牌";
        return; // 如果根本没找到卡，直接返回，不发信号
    }

    // 先发射信号，让 UI 层在数据彻底从内存结构消失前完成操作
    emit cardDeled(this, card);

    // 移除空栈（移除 return，确保逻辑走完）
    for(int i = 0; i < m_cards.size(); i++) {
        if(m_cards[i].isEmpty()){
            m_cards.removeAt(i);
            break; // 既然是单次删除，删掉一个空栈就可以跳出了
        }
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



