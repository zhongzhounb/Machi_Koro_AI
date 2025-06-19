#include "gamestate.h"
#include "player.h"
#include "cardstore.h" // 假设有 CardStore 类
#include "dice.h"      // 假设有 Dice 类
#include "card.h"      // 需要 Card 类定义

#include <QDebug>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>

GameState::GameState(QObject* parent)
    : QObject(parent)
    , m_currentPlayerIndex(-1)
    , m_dice(nullptr) // 初始为 nullptr
{
    // 在这里创建 Dice 实例，并设置 GameState 为其父对象
    m_dice = new Dice(this);
}

GameState::~GameState() {
    // Qt 的对象树会自动删除子对象，所以这里不需要手动 delete m_players, m_cardStores, m_dice
    // 除非这些对象在创建时没有设置 GameState 为父对象
    // 假设 Player, CardStore, Dice 在 addXxx 方法中设置了父对象为 GameState
    // 那么这里就无需手动 delete
}

void GameState::addPlayer(Player* player) {

}

void GameState::setCurrentPlayer(Player* player) {
    if (m_currentPlayer != player) {
        m_currentPlayer = player;
        m_currentPlayerIndex = m_players.indexOf(player);
        emit currentPlayerChanged(player);
        emit gameStateChanged();
    }
}

void GameState::nextPlayer() {
    if (m_players.isEmpty()) return;
    m_currentPlayerIndex = (m_currentPlayerIndex + 1) % m_players.size();
    setCurrentPlayer(m_players.at(m_currentPlayerIndex));
}

void GameState::addCardStore(CardStore* store) {
    if (store && !m_cardStores.contains(store)) {
        m_cardStores.append(store);
        store->setParent(this); // 设置 GameState 为 CardStore 的父对象
        emit gameStateChanged();
    }
}

void GameState::setDice(Dice* dice) {
    if (m_dice != dice) {
        if (m_dice) m_dice->deleteLater(); // 删除旧的骰子实例
        m_dice = dice;
        if (m_dice) m_dice->setParent(this); // 设置 GameState 为 Dice 的父对象
        emit gameStateChanged();
    }
}

// ====================================================================
// 卡牌实例全局管理实现
void GameState::registerCardInstance(Card* card) {
    if (card && !m_allCardInstances.contains(card->getId())) {
        m_allCardInstances.insert(card->getId(), card);
        // 注意：这里不设置父对象，因为卡牌的父对象是 Player
        // Card* card 的生命周期由 Player 管理
    }
}

void GameState::unregisterCardInstance(Card* card) {
    if (card && m_allCardInstances.contains(card->getId())) {
        m_allCardInstances.remove(card->getId());
    }
}

Card* GameState::getCardInstanceById(int id) const {
    return m_allCardInstances.value(id, nullptr);
}

// ====================================================================
// 其他游戏状态数据实现
void GameState::setLastDiceRolls(const QList<int>& rolls) {
    m_lastDiceRolls = rolls;
    emit gameStateChanged();
}

int GameState::totalDiceRoll() const {
    int sum = 0;
    for (int roll : m_lastDiceRolls) {
        sum += roll;
    }
    return sum;
}

void GameState::setLandmarkUsed(const QString& landmarkName, bool used) {
    m_landmarkUsedThisTurn[landmarkName] = used;
    emit gameStateChanged();
}

void GameState::resetLandmarkUsage() {
    m_landmarkUsedThisTurn.clear();
    emit gameStateChanged();
}


