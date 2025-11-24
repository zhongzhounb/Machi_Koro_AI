#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QMap>
#include <QPointer>
#include <functional>
#include <QLabel>
#include "global.h"
#include "prompt/promptbuycardanimationhandler.h"
#include "prompt/promptcardinanimationhandler.h"
#include "prompt/promptcardoutanimationhandler.h"
#include "prompt/promptdiceanimationhandler.h"
#include "prompt/promptselectdicehandler.h"
#include "prompt/promptpopuphandler.h"
#include "prompt/promptselectcardhandler.h"
#include "prompt/promptselectplayerhandler.h"
#include "prompt/promptstartturnanimationhandler.h"
// 前向声明，减少头文件依赖
class GameState;
class Player;
class CardStore;
class Card;
class PlayerAreaWidget;
class CardStoreAreaWidget;
class CardWidget;
class QGridLayout;
class QGraphicsDropShadowEffect;
class QPropertyAnimation;
class PromptData;
class DiceAreaWidget;
#include <QGraphicsEffect> // 新增：QGraphicsEffect 头文件
class CoinChangeWidget;
class SlotWidget; // 新增：前向声明 SlotWidget

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE


// 辅助结构体，用于配置每个玩家的UI布局
struct PlayerLayoutConfig {
    int photoRow, photoCol, photoRowSpan, photoColSpan;
    int cardAreaRow, cardAreaCol, cardAreaRowSpan, cardAreaColSpan;
    bool cardAreaIsHorizontal;

    int landmarkAreaRow, landmarkAreaCol, landmarkAreaRowSpan, landmarkAreaColSpan;
    bool landmarkAreaIsHorizontal;
    bool landmarkAreaIsSpecialPlayer0; // 仅用于玩家0的地标区

    // 使用std::function来动态计算窗口外位置，因为m_gameMainWidget的尺寸可能在构造函数时尚未确定
    std::function<QPoint(int gameMainWidgetWidth, int gameMainWidgetHeight)> getOutOfWindowPos;
    std::function<QPoint(int gameMainWidgetWidth, int gameMainWidgetHeight)> getDisplayPos;
};

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(GameState* state, QWidget *parent = nullptr);
    ~MainWindow();
    // 新增：状态变量，用于跟踪是否有交互式提示处于活动状态
    bool m_isInteractivePromptActive = false;

    void setupGameMainLayout(QGridLayout* layout, const QList<Player*>& players);
    void setupPlayerWidgets(QGridLayout* layout, Player* player, const PlayerLayoutConfig& config);
    CardStore* findCardStoreForCard(Card* card, int& posInStore);
    void showWaitCurtain(QString waitMessage);
    void hideWaitCurtain();
    Ui::MainWindow *ui;
    GameState* m_state;

    QWidget* m_gameMainWidget;
    QWidget* m_animationOverlayWidget;

    CardStoreAreaWidget* m_cardStoreArea;

    QMap<Player*, PlayerAreaWidget*> m_playerToCardAreaMap;
    QMap<Player*, PlayerAreaWidget*> m_playerToLandmarkAreaMap;
    QMap<Player*, CoinChangeWidget*> m_playerToCoinChangeWidgetMap; // ADD THIS LINE

    QMap<Player*, PlayerLayoutConfig> m_playerLayoutConfigs;

    QMap<Player*, QPoint> m_playerOutOfWindowTargetPos; // 存储计算好的窗口外目标位置 (此Map将在动画前动态更新)

    // 用于详细卡牌显示和动画
    QPointer<CardWidget> m_detailedCardWidget = nullptr;
    QPointer<QPropertyAnimation> m_fadeAnimation = nullptr;

    QPointer<DiceAreaWidget> m_mainDiceAreaWidget;

    //用于CardIn/CardOut动画的成员变量
    QMap<QPointer<SlotWidget>, QPointer<Player>> m_animatedSlotToPlayerMap; // 跟踪临时动画卡槽及其关联玩家
    QList<QPointer<SlotWidget>> m_currentAnimatedInSlots; // 当前动画进入的卡槽列表

    QWidget* m_waitCurtain;
    QLabel* m_waitLabel;

signals:
    void responseUserInput(int opId);

public slots:
    void onRequestUserInput(PromptData pd);
    void showDetailedCard(Card* card, QPoint globalPos);
    void hideDetailedCard();

private:
    PromptStartTurnAnimationHandler* m_startTurnAnimationHandler;
    PromptBuyCardAnimationHandler* m_buyCardAnimationHandler;
    PromptCardInAnimationHandler* m_cardInAnimationHandler;
    PromptCardOutAnimationHandler* m_cardOutAnimationHandler;
    PromptDiceAnimationHandler* m_diceAnimationHandler;
    PromptPopupHandler* m_popupHandler;
    PromptSelectCardHandler* m_selectCardHandler;
    PromptSelectDiceHandler* m_selectDiceHandler;
    PromptSelectPlayerHandler* m_selectPlayerHandler;



};

#endif // MAINWINDOW_H
