#include "cardstore.h"
#include "card.h"
#include <random>

CardStore::CardStore(int id, int slotNum, QObject* parent)
    : QObject(parent), m_id(id), m_slotNum(slotNum){
    m_slots.reserve(m_slotNum);
    for (int i = 0; i < m_slotNum; ++i)
        m_slots.append(QStack<Card*>()); // 添加空的卡牌栈到每个卡槽
}

CardStore::~CardStore(){}

//初始化添加供应堆
void CardStore::addCard(Card* card) {
    m_supplyPile.push(card);
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
//【需要hasEmptySlot()先验】
Card* CardStore::suppleCard() {
    Card* topCard=m_supplyPile.top();
    m_supplyPile.pop();

    //优先补充至有同种牌的卡槽
    for (auto& slot : m_slots)
        if(!slot.empty()){
            Card* card=slot.top();
            if(topCard->getName()==card->getName()){
                slot.push(topCard);
                return topCard;
            }
        }

    //其次补充至空卡槽（如有多个只补充第一个）
    for(auto& slot : m_slots)
        if(slot.empty()){
            slot.push(topCard);
            return topCard;
        }

    //没返回说明就有问题
    qWarning() << "CardStore::suppleCard():卡牌补充时发生错误！";
    return NULL;

}

// 获取所有卡槽位中的栈顶卡
QList<Card*> CardStore::getCardFirst() const{
    QList<Card*> firstCards;
    for (const auto& slotStack : m_slots)
        if (!slotStack.isEmpty())
            firstCards.append(slotStack.top());
    return firstCards;
}

// 某张卡被买走
void CardStore::delCard(Card* card) {

    //优先补充至有同种牌的卡槽
    for (auto& slot : m_slots)
        if(!slot.empty())
            if(slot.top()==card){
                slot.pop();
                return;
            }

    //没找到就有错误
    qWarning() << "CardStore::delCard：卡牌移除时发生错误！";
}
