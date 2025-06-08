#include "player.h"

// 构造函数
Player::Player(int id, const QString& name, AIRank aiRank, QObject* parent)
    : QObject(parent), m_id(id), m_name(name), m_aiRank(aiRank), m_coins(0)
{
    // 构造时，金币默认为0，卡牌列表为空
    qDebug() << "Player created: ID =" << m_id << ", Name =" << m_name << ", AI Rank =" << static_cast<int>(m_aiRank);
}

// 析构函数
Player::~Player() {
    // 销毁所有拥有的卡牌实例。
    // 由于在 addCard 中将 Card 实例设置为 Player 的子对象，
    // 当 Player 对象被销毁时，其子对象（Card 实例）也会被 Qt 的对象树机制自动销毁。
    // 因此，这里无需手动调用 qDeleteAll(m_cards);
    // 这样做是为了避免在卡牌被移除（例如交换）后，又被析构函数尝试删除。
    qDebug() << "Player destroyed: ID =" << m_id << ", Name =" << m_name;
}

// 增加金币
void Player::addCoins(int amount) {
    if (amount > 0) {
        m_coins += amount;
        emit coinsChanged(m_coins);
        qDebug() << m_name << "金币变化：+" << amount << "，当前：" << m_coins;
    }
}

// 减少金币
void Player::removeCoins(int amount) {
    if (amount > 0) {
        m_coins = qMax(0, m_coins - amount); // 确保金币不为负
        emit coinsChanged(m_coins);
        qDebug() << m_name << "金币变化：-" << amount << "，当前：" << m_coins;
    }
}

// 添加卡牌实例
void Player::addCard(Card* cardInstance) {
    if (cardInstance) {
        m_cards.append(cardInstance);
        cardInstance->setState(State::Opening); // 默认设置为正常状态
        // Card 不再存储 m_owner 指针，而是通过参数传递
        emit cardAdded(cardInstance);
        qDebug() << m_name << "获得了卡牌：" << cardInstance->getName() << "(ID:" << cardInstance->getId() << ")";
    }
}

// 移除卡牌实例
void Player::removeCard(Card* cardInstance) {
    if (cardInstance) {
        // QList::removeOne 会移除列表中第一个匹配的元素
        bool removed = m_cards.removeOne(cardInstance);
        if (removed) {
            emit cardRemoved(cardInstance);
            qDebug() << m_name << "移除了卡牌：" << cardInstance->getName() << "(ID:" << cardInstance->getId() << ")";
        } else {
            qDebug() << "警告：尝试移除不存在的卡牌实例：" << cardInstance->getName() << " from " << m_name;
        }
    }
}

// 获取某个卡牌类型的数量
int Player::getCardTypeCount(int cardId) const {
    int count = 0;
    for (Card* card : m_cards) {
        if (card->getId() == cardId) {
            count++;
        }
    }
    return count;
}

// 获取某个卡牌类型的特定状态数量
int Player::getCardTypeStateCount(int cardId, State state) const {
    int count = 0;
    for (Card* card : m_cards) {
        if (card->getId() == cardId && card->getState() == state) {
            count++;
        }
    }
    return count;
}

// 获取特定卡牌类型的实例列表
QList<Card*> Player::getCardInstancesByType(int cardId) const {
    QList<Card*> instances;
    for (Card* card : m_cards) {
        if (card->getId() == cardId) {
            instances.append(card);
        }
    }
    return instances;
}
