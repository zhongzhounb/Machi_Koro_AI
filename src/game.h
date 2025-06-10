#ifndef GAME_H
#define GAME_H
#include "global.h"
class Player;
class Card;
class GameCommand;
class CardStore;
class Dice;

class Game : public QObject {
    Q_OBJECT
public:
    explicit Game(QObject* parent);
    ~Game();
    //传入各位玩家信息就可以开始游戏了,包括初始化卡牌、商店
    void startGame(QList<Player*> players);
    //开始某位玩家的回合
    void startTurn(Player* player);
    //



signals:
    // 只保留一个通用的请求UI交互信号
    void requestPlayerChoice(Player* chooser, const QString& prompt, const QVariantList& choices);

public slots:
    // 只保留一个通用的槽函数来接收所有选择反馈
    void onPlayerChoiceMade(Player* chooser, const QVariant& choiceData);


private:
    /*玩家*/
    //所有玩家
    QList<Player*> m_players;
    //当前玩家
    QPointer<Player> m_currentPlayer;
    int m_currentPlayerIndex;//可能有用，尽量别用

    /*卡牌*/
    //所有普通卡
    QList<Card*> m_allCards;
    //所有地标卡
    QList<Card*> m_allLandmarks;

    /*商店*/
    QList<CardStore*> m_cardStores;

    /*命令*/
    //命令队列
    QList<GameCommand*> m_commandQueue;
    // 当前正在执行的命令
    QPointer<GameCommand> m_currentCommand;
    // 内部流程控制
    void processNextCommand(); // 逐个执行队列中的命令

    /*骰子*/
    Dice* m_dice;
};

#endif // GAME_H
