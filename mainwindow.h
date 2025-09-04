#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QMap>
#include "global.h" // For PromptData
#include <QPoint>   // For QPoint
#include <QGraphicsOpacityEffect> // 新增：QGraphicsOpacityEffect
#include <QPropertyAnimation>     // 新增：QPropertyAnimation

// 前向声明
class GameState;
class CardStoreAreaWidget;
class PlayerAreaWidget;
class PlayerPhotoWidget;
class LogViewerWidget;
class DiceAreaWidget;
class Player;
class CardStore;
class Card;
class CardWidget; // 新增：CardWidget

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(GameState* state, QWidget *parent = nullptr);
    ~MainWindow();

public slots:
    void onRequestUserInput(PromptData pd);

signals:
    void responseUserInput(int opId);

private:
    Ui::MainWindow *ui;
    GameState* m_state;
    CardStoreAreaWidget* m_cardStoreArea = nullptr;
    QWidget* m_gameMainWidget = nullptr; // 指向 gameMain 控件
    QWidget* m_animationOverlayWidget;

    // 映射，用于方便地访问玩家的卡牌区域和地标区域
    QMap<Player*, PlayerAreaWidget*> m_playerToCardAreaMap;
    QMap<Player*, PlayerAreaWidget*> m_playerToLandmarkAreaMap;

    // 新增：每个玩家的动画结束（屏幕外）目标坐标
    QMap<Player*, QPoint> m_playerOutOfWindowTargetPos;

    // 新增：用于显示详细卡牌的成员
    CardWidget* m_detailedCardWidget = nullptr;
    QGraphicsOpacityEffect* m_detailedCardOpacityEffect = nullptr;
    QPropertyAnimation* m_fadeAnimation = nullptr;

    // 新增：处理详细卡牌显示/隐藏的槽函数
private slots:
    void showDetailedCard(Card* card, QPoint globalPos);
    void hideDetailedCard();

private:
    // 辅助函数：查找卡牌所在的 CardStore 及其在商店中的位置
    CardStore* findCardStoreForCard(Card* card, int& posInStore);

};
#endif // MAINWINDOW_H
