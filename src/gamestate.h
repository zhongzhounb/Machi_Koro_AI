#ifndef GAMESTATE_H
#define GAMESTATE_H

#include <QObject>
#include <QList>
#include <QPointer> // 用于 m_currentPlayer
#include <QMap>     // 用于 m_allCardInstances

#include "global.h" // 包含 Color, Type, State 枚举

// 前向声明
class Player;
class CardStore;
class Dice;
class Card; // 前向声明 Card

class GameState : public QObject {
    Q_OBJECT
public:
    explicit GameState(QObject* parent = nullptr); // 构造函数，parent 默认 nullptr
    ~GameState(); // 析构函数，负责清理拥有的对象

    // ====================================================================
    // 玩家管理
    QList<Player*> players() const { return m_players; }
    void addPlayer(Player* player); // 添加玩家，并设置其父对象为 GameState

    Player* getCurrentPlayer() const { return m_currentPlayer; }
    void setCurrentPlayer(Player* player); // 设置当前玩家
    void nextPlayer(); // 切换到下一个玩家

    // ====================================================================
    // 商店管理
    QList<CardStore*> cardStores() const { return m_cardStores; }
    void addCardStore(CardStore* store); // 添加商店，并设置其父对象为 GameState

    // ====================================================================
    // 骰子管理
    Dice* dice() const { return m_dice; }
    void setDice(Dice* dice); // 设置骰子实例

    // ====================================================================
    // 卡牌实例全局查找 (关键新增)
    // 注册卡牌实例到全局映射
    void registerCardInstance(Card* card);
    // 从全局映射注销卡牌实例 (当卡牌被移除时)
    void unregisterCardInstance(Card* card);
    // 根据 ID 查找卡牌实例 (用于命令反序列化等)
    Card* getCardInstanceById(int id) const;

    // ====================================================================
    // 游戏状态数据 (例如：骰子结果，地标使用情况)
    QList<int> lastDiceRolls() const { return m_lastDiceRolls; }
    void setLastDiceRolls(const QList<int>& rolls);
    int totalDiceRoll() const;

    QMap<QString, bool> landmarkUsedThisTurn() const { return m_landmarkUsedThisTurn; }
    void setLandmarkUsed(const QString& landmarkName, bool used);
    void resetLandmarkUsage(); // 每回合开始时重置

    //获取日志
    void addLog(QString log);

signals:
    void gameStateChanged(); // 任何状态变化时发出，通知UI更新
    void currentPlayerChanged(Player* newPlayer); // 当前玩家改变时发出

private:
    QList<Player*> m_players;
    QPointer<Player> m_currentPlayer;
    int m_currentPlayerIndex; // 辅助索引

    QList<CardStore*> m_cardStores;
    Dice* m_dice; // GameState 拥有 Dice 实例

    QMap<int, Card*> m_allCardInstances; // 全局卡牌实例映射 (ID -> Card*)

    QList<int> m_lastDiceRolls;
    QMap<QString, bool> m_landmarkUsedThisTurn;

    QList<QString*> m_log;
};

#endif // GAMESTATE_H
