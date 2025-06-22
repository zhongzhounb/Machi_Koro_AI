#include "cardstore.h"
#include "card.h"
#include <random>

CardStore::CardStore(int id, int slotNum,QString name, QObject* parent)
    : QObject(parent), m_id(id), m_slotNum(slotNum),m_name(name){
    m_slots.reserve(m_slotNum);
    for (int i = 0; i < m_slotNum; ++i)
        m_slots.append(QList<Card*>()); // 添加空的卡牌栈到每个卡槽
}

CardStore::~CardStore(){}

//初始化添加供应堆
void CardStore::addCard(Card* card) {
    m_supplyPile.append(card);
}

//【可能有问题！】初始化打乱供应堆的顺序
void CardStore::shuffleCard(){
    // std::random_device 提供了非确定性随机数（如果可用）作为种子
    std::random_device rd;
    // std::mt19937 是一个高质量的伪随机数生成器
    std::mt19937 g(rd()); // 使用 rd() 作为种子初始化生成器
    std::shuffle(m_supplyPile.begin(), m_supplyPile.end(), g);
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

    //优先补充至有同种牌的卡槽
    for (auto& slot : m_slots)
        if(!slot.empty())
            if(slot.last()==card)
                slot.pop_back();
    //自动补充
    suppleCard();

}
