#include "cardstore.h"
#include "card.h"
#include "randomutils.h"
#include <QTimer>

CardStore::CardStore(int id, int slotNum,Color type, QObject* parent)
    : QObject(parent), m_id(id), m_slotNum(slotNum),m_type(type){
    m_slots.reserve(m_slotNum);
    for (int i = 0; i < m_slotNum; ++i)
        m_slots.append(QList<Card*>()); // 添加空的卡牌栈到每个卡槽
}

CardStore::~CardStore(){}

//初始化添加供应堆
void CardStore::addCard(Card* card) {
    m_supplyPile.append(card);
    emit supplyCardAdded(this);
}

//初始化打乱供应堆的顺序
void CardStore::shuffleCard(){
    // 获取 RandomUtils 单例的随机数引擎
    std::mt19937& rng = RandomUtils::instance().getEngine();

    // 使用 std::shuffle 对 m_supplyPile 进行洗牌
    // QList 的迭代器与 std::shuffle 兼容
    std::shuffle(m_supplyPile.begin(), m_supplyPile.end(), rng);

}

//查看卡槽是否有空
bool CardStore::hasEmptySlot() const { // 修正：成为成员函数，并加上 const
    for (const auto& slot : m_slots)
        if (slot.isEmpty())
            return true;
    return false;
}

// 供应堆补充卡牌至卡槽（补足N种，N为slotNum）
void CardStore::suppleCard() {
    while(hasEmptySlot()&&!m_supplyPile.isEmpty()){
        Card* topCard=m_supplyPile.takeLast();
        //优先补充至有同种牌的卡槽
        bool findFlag=false;
        for (int i=0;i<m_slots.size()&&!findFlag;i++)
            if(!m_slots[i].empty()){
                Card* card=m_slots[i].last();
                if(topCard->getName()==card->getName()){
                    addCardToSlot(topCard,i);
                    findFlag=true;
                }
            }
        if(findFlag)
            continue;
        //其次补充至空卡槽（如有多个只补充第一个）
        for (int i=0;i<m_slots.size()&&!findFlag;i++)
            if(m_slots[i].empty()){
                addCardToSlot(topCard,i);
                findFlag=true;
            }
    }
}

//槽位增加卡牌（有UI动画）
void CardStore::addCardToSlot(Card* card, int pos){
    m_slots[pos].push_back(card);
    qDebug()<<card->getName()<<"放置位置："<<pos;
    emit cardAdded(this,card,pos);
}

// 获取所有卡槽位中的栈顶卡
QList<Card*> CardStore::getCardFirst() const{
    QList<Card*> firstCards;
    for (const auto& slotStack : m_slots)
        if (!slotStack.isEmpty())
            firstCards.append(slotStack.last());
    return firstCards;
}

// 某张卡被买走
void CardStore::delCard(Card* card) {
    //删除需要延迟一段时间，因为需要让动画先找到删除的卡用于做动画
    QTimer::singleShot(50, this, [this,card](){ // 显式捕获 'this'
        //优先补充至有同种牌的卡槽
        for (int i=0;i<m_slots.size();i++)
            if(!m_slots[i].empty())
                if(m_slots[i].last()==card){
                    m_slots[i].pop_back();
                    emit cardDeled(this,card,i);
                }

        //自动补充
        suppleCard();
    });

}

int CardStore::getSupplyCount()const{
    return m_supplyPile.size();
}
