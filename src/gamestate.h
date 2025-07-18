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

    void initState();

    // ====================================================================
    // 玩家管理（动态，所以保留add方法，其他删除）
    QList<Player*> getPlayers() const { return m_players; }
    void addPlayer(Player* player); // 添加玩家，并设置其父对象为 GameState
    Player* getCurrentPlayer() const { return m_currentPlayer; }
    void nextPlayer(); // 切换到下一个玩家

    // ====================================================================
    // 商店管理
    QList<CardStore*> getCardStores() const { return m_cardStores; }
    // ====================================================================
    // 骰子管理
    Dice* getDice() const { return m_dice; }

    // ====================================================================
    // 获取日志
    void addLog(QString log);

    // ====================================================================
    // 索引卡牌
    Card* getCardForId(int cardId){return m_allCardInstances[cardId];}

signals:
    //发出添加日志信号
    void logMessageAdded(const QString& message);
    //初始化商店
    void initCardStores(const QList<CardStore*> cardStores);


private:
    QList<Player*> m_players;
    Player* m_currentPlayer;

    QList<CardStore*> m_cardStores;
    Dice* m_dice; // GameState 拥有 Dice 实例

    QMap<int, Card*> m_allCardInstances; // 全局卡牌实例映射 (ID -> Card*)

    QList<QString> m_log;

    //用以获取初始商店卡
    QList<Card*>getStoresInitCards();
    //用以获取初始玩家卡
    QList<Card*>getPlayerInitCards();

};

#endif // GAMESTATE_H
