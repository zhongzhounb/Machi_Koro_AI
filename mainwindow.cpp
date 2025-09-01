#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "gamestate.h"
#include "QStackedLayout"
#include "QGridLayout"
#include "QWidget"
#include "cardstoreareawidget.h"
#include "playerareawidget.h"
#include "playerphotowidget.h"
#include "logviewerwidget.h"
#include "diceareawidget.h"
#include "player.h"
#include "cardstore.h"
#include "card.h"
#include "slotwidget.h"
#include "cardwidget.h"

#include <QScreen>
#include <QGuiApplication>
#include <QTimer>
#include <QPropertyAnimation>
#include <QEasingCurve>
#include <QParallelAnimationGroup> // 如果需要同时动画位置和大小

MainWindow::MainWindow(GameState* state, QWidget *parent)
    : m_state(state), QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    setContentsMargins(0, 0, 0, 0);

    QWidget *centralWidget = new QWidget(this);
    setCentralWidget(centralWidget);

    QStackedLayout* centralLayout = new QStackedLayout(centralWidget);
    centralLayout->setStackingMode(QStackedLayout::StackAll);

    m_gameMainWidget = new QWidget(centralWidget); // 赋值给成员变量
    QGridLayout *gameMainLayout = new QGridLayout(m_gameMainWidget);

    gameMainLayout->setContentsMargins(0, 0, 0, 0);
    gameMainLayout->setSpacing(0);

    for(int i = 0; i < 90; ++i) {
        gameMainLayout->setRowStretch(i, 1);
    }
    for(int i = 0; i < 160; ++i) {
        gameMainLayout->setColumnStretch(i, 1);
    }

    LogViewerWidget* logViewerDummy = new LogViewerWidget(m_gameMainWidget);
    QObject::connect(m_state,&GameState::logMessageAdded,logViewerDummy,&LogViewerWidget::appendLogMessage);
    gameMainLayout->addWidget(logViewerDummy, 23, 100, 40, 30);

    QList<Player*> players=state->getPlayers();

    // 当前玩家 (Player 0)
    PlayerPhotoWidget* playerPhoto0 = new PlayerPhotoWidget(players[0],m_gameMainWidget);
    QObject::connect(players[0], &Player::coinsChange, playerPhoto0, &PlayerPhotoWidget::onCoinsChange);
    QObject::connect(m_state,&GameState::currentPlayerChanged,playerPhoto0,&PlayerPhotoWidget::onCurrentPlayerChanged);
    gameMainLayout->addWidget(playerPhoto0, 70, 0, 20, 20);

    PlayerAreaWidget* playerCardArea0=new PlayerAreaWidget(players[0],true,false,m_gameMainWidget); // 父对象改为 m_gameMainWidget
    gameMainLayout->addWidget(playerCardArea0, 75, 20, 15, 120);
    m_playerToCardAreaMap.insert(players[0], playerCardArea0); // 存储到映射

    PlayerAreaWidget* playerLandmarkArea0=new PlayerAreaWidget(players[0],true,true,m_gameMainWidget,true); // 父对象改为 m_gameMainWidget
    gameMainLayout->addWidget(playerLandmarkArea0, 65, 45, 10, 70);
    m_playerToLandmarkAreaMap.insert(players[0], playerLandmarkArea0);

    // 1号玩家
    PlayerPhotoWidget* playerPhoto1 = new PlayerPhotoWidget(players[1],m_gameMainWidget);
    QObject::connect(players[1], &Player::coinsChange, playerPhoto1, &PlayerPhotoWidget::onCoinsChange);
    QObject::connect(m_state,&GameState::currentPlayerChanged,playerPhoto1,&PlayerPhotoWidget::onCurrentPlayerChanged);
    gameMainLayout->addWidget(playerPhoto1, 0, 0, 15, 15);

    PlayerAreaWidget* playerCardArea1=new PlayerAreaWidget(players[1],false,false,m_gameMainWidget); // 父对象改为 m_gameMainWidget
    gameMainLayout->addWidget(playerCardArea1, 15, 0, 50, 12);
    m_playerToCardAreaMap.insert(players[1], playerCardArea1);

    PlayerAreaWidget* playerLandmarkArea1=new PlayerAreaWidget(players[1],false,true,m_gameMainWidget); // 父对象改为 m_gameMainWidget
    gameMainLayout->addWidget(playerLandmarkArea1, 15, 12, 50, 8);
    m_playerToLandmarkAreaMap.insert(players[1], playerLandmarkArea1);

    // 2号玩家
    PlayerPhotoWidget* playerPhoto2 = new PlayerPhotoWidget(players[2],m_gameMainWidget);
    QObject::connect(players[2], &Player::coinsChange, playerPhoto2, &PlayerPhotoWidget::onCoinsChange);
    QObject::connect(m_state,&GameState::currentPlayerChanged,playerPhoto2,&PlayerPhotoWidget::onCurrentPlayerChanged);
    gameMainLayout->addWidget(playerPhoto2, 0, 70, 15, 15);

    PlayerAreaWidget* playerCardArea2=new PlayerAreaWidget(players[2],true,false,m_gameMainWidget); // 父对象改为 m_gameMainWidget
    gameMainLayout->addWidget(playerCardArea2, 0, 20, 12, 50);
    m_playerToCardAreaMap.insert(players[2], playerCardArea2);

    PlayerAreaWidget* playerLandmarkArea2=new PlayerAreaWidget(players[2],true,true,m_gameMainWidget); // 父对象改为 m_gameMainWidget
    gameMainLayout->addWidget(playerLandmarkArea2, 12, 20, 8, 50);
    m_playerToLandmarkAreaMap.insert(players[2], playerLandmarkArea2);

    // 3号玩家
    PlayerPhotoWidget* playerPhoto3 = new PlayerPhotoWidget(players[3],m_gameMainWidget);
    QObject::connect(players[3], &Player::coinsChange, playerPhoto3, &PlayerPhotoWidget::onCoinsChange);
    QObject::connect(m_state,&GameState::currentPlayerChanged,playerPhoto3,&PlayerPhotoWidget::onCurrentPlayerChanged);
    gameMainLayout->addWidget(playerPhoto3, 0, 140, 15, 15);
    PlayerAreaWidget* playerCardArea3=new PlayerAreaWidget(players[3],true,false,m_gameMainWidget); // 父对象改为 m_gameMainWidget
    gameMainLayout->addWidget(playerCardArea3, 0, 90, 12, 50);
    m_playerToCardAreaMap.insert(players[3], playerCardArea3);

    PlayerAreaWidget* playerLandmarkArea3=new PlayerAreaWidget(players[3],true,true,m_gameMainWidget); // 父对象改为 m_gameMainWidget
    gameMainLayout->addWidget(playerLandmarkArea3, 12, 90, 8, 50);
    m_playerToLandmarkAreaMap.insert(players[3], playerLandmarkArea3);

    // 4号玩家
    PlayerPhotoWidget* playerPhoto4 = new PlayerPhotoWidget(players[4],m_gameMainWidget);
    QObject::connect(players[4], &Player::coinsChange, playerPhoto4, &PlayerPhotoWidget::onCoinsChange);
    QObject::connect(m_state,&GameState::currentPlayerChanged,playerPhoto4,&PlayerPhotoWidget::onCurrentPlayerChanged);
    gameMainLayout->addWidget(playerPhoto4, 70, 145, 15, 15);
    PlayerAreaWidget* playerCardArea4=new PlayerAreaWidget(players[4],false,false,m_gameMainWidget); // 父对象改为 m_gameMainWidget
    gameMainLayout->addWidget(playerCardArea4, 20, 148, 50, 12);
    m_playerToCardAreaMap.insert(players[4], playerCardArea4);

    PlayerAreaWidget* playerLandmarkArea4=new PlayerAreaWidget(players[4],false,true,m_gameMainWidget); // 父对象改为 m_gameMainWidget
    gameMainLayout->addWidget(playerLandmarkArea4, 20, 140, 50, 8);
    m_playerToLandmarkAreaMap.insert(players[4], playerLandmarkArea4);


    // 商店
    m_cardStoreArea = new CardStoreAreaWidget(m_gameMainWidget); // 父对象改为 m_gameMainWidget
    m_cardStoreArea->setGameState(m_state);
    gameMainLayout->addWidget(m_cardStoreArea, 23, 25, 40, 70);

    // 骰子
    DiceAreaWidget* diceAreaWidget= new DiceAreaWidget(m_state->getDice(),m_gameMainWidget);
    gameMainLayout->addWidget(diceAreaWidget,50,60,12,35);

    centralLayout->addWidget(m_gameMainWidget);
}

MainWindow::~MainWindow()
{
    delete ui;
}

// 辅助函数：查找卡牌所在的 CardStore 及其在商店中的位置
CardStore* MainWindow::findCardStoreForCard(Card* card, int& posInStore) {
    QList<CardStore*> stores = m_state->getCardStores();
    for (CardStore* store : stores) {
        // 假设 store->getSlots() 返回 QList<QList<Card*>>
        // 并且我们在寻找特定的卡牌实例
        QList<Card*>cards=store->getCardFirst();
        for (int i = 0; i < cards.size(); ++i) {
            if (cards[i]==card) {

                posInStore = i;
                return store;
            }
        }
    }
    posInStore = -1;
    return nullptr;
}


void MainWindow::onRequestUserInput(PromptData pd){
    switch(pd.type){
    case PromptData::None:
    case PromptData::Popup:
    case PromptData::SelectCard:
    case PromptData::SelectPlayer:
    case PromptData::SelectDice:
    case PromptData::StartTurnAnimation:
    case PromptData::DiceAnimation:
    case PromptData::CardInAnimation:
    case PromptData::CardOutAnimation: {
        int opId = pd.autoInput;
        QTimer::singleShot(500, this, [this, opId](){
            emit responseUserInput(opId);
        });
        break;
    }
    case PromptData::BuyCardAnimation:{
        Player* buyer = pd.buyer;
        Card* cardToBuy = pd.card;

        int opId = pd.autoInput;

        if (!buyer || !cardToBuy) {
            qWarning() << "BuyCardAnimation: Invalid buyer or card in PromptData.";
            emit responseUserInput(opId);
            return;
        }

        if (!m_cardStoreArea || !m_gameMainWidget) {
            qWarning() << "BuyCardAnimation: CardStoreAreaWidget or gameMainWidget not found.";
            emit responseUserInput(opId);
            return;
        }

        // 1. 查找卡牌所在的 CardStore 及其在商店中的位置
        int cardPosInStore = -1;
        CardStore* sourceStore = findCardStoreForCard(cardToBuy, cardPosInStore);

        if (!sourceStore || cardPosInStore == -1) {
            qWarning() << "BuyCardAnimation: Source CardStore or card position not found for card" << cardToBuy->getName();
            emit responseUserInput(opId);
            return;
        }

        // 2. 获取动画的起始位置 (商店卡槽的中心点，转换为 m_gameMainWidget 坐标)
        QPoint startPosInCardStoreArea = m_cardStoreArea->getStoreSlotCenterPos(sourceStore, cardPosInStore);
        QPoint startPosGlobal = m_cardStoreArea->mapToGlobal(startPosInCardStoreArea);
        QPoint startPosInGameMain = m_gameMainWidget->mapFromGlobal(startPosGlobal);

        // 3. 查找购买者的 PlayerAreaWidget
        PlayerAreaWidget* targetPlayerArea = m_playerToCardAreaMap.value(buyer);

        if (!targetPlayerArea) {
            qWarning() << "BuyCardAnimation: Target PlayerAreaWidget not found for buyer" << buyer->getName();
            emit responseUserInput(opId);
            return;
        }

        // 4. 获取动画的结束位置 (玩家卡牌区域的中心点，转换为 m_gameMainWidget 坐标)
        // 注意：这里简化为玩家区域的中心，PlayerAreaWidget 内部会处理最终的精确放置。
        QPoint endPosInPlayerArea = targetPlayerArea->getCardTargetSlotCenterPos(cardToBuy);
        QPoint endPosGlobal = targetPlayerArea->mapToGlobal(endPosInPlayerArea);
        QPoint endPosInGameMain = m_gameMainWidget->mapFromGlobal(endPosGlobal);

        // 5. 创建用于动画的临时 CardWidget
        // 将其父对象设置为 m_gameMainWidget，以便它可以在整个游戏主区域内自由移动
        CardWidget* animatingCardWidget = new CardWidget(cardToBuy, ShowType::Ordinary, m_gameMainWidget);

        // 获取起始卡槽的实际尺寸，作为动画卡牌的初始尺寸
        QList<SlotWidget*> storeSlots = m_cardStoreArea->m_storeToSlotsMap.value(sourceStore);
        SlotWidget* sourceSlot = nullptr;
        // 实际卡槽的索引是 cardPosInStore + 1 (因为 supplySlot 在索引 0)
        if (cardPosInStore + 1 < storeSlots.size()) {
            sourceSlot = storeSlots.at(cardPosInStore + 1);
        }

        QSize startSize = sourceSlot ? sourceSlot->size() : QSize(80, 120); // 如果找不到，给个默认尺寸
        QSize endSize = targetPlayerArea->getTargetCardSize(); // 获取目标玩家区域的卡牌尺寸

        animatingCardWidget->setFixedSize(startSize);
        // 调整位置，使 animatingCardWidget 的中心与 startPosInGameMain 对齐
        animatingCardWidget->move(startPosInGameMain - animatingCardWidget->rect().center());
        animatingCardWidget->show();
        animatingCardWidget->raise(); // 确保它在最上层

        // 6. 创建动画组 (同时动画位置和大小)
        QParallelAnimationGroup* animationGroup = new QParallelAnimationGroup(this);

        QPropertyAnimation* animationPos = new QPropertyAnimation(animatingCardWidget, "pos", this);
        animationPos->setDuration(500); // 动画持续时间 500 毫秒
        animationPos->setStartValue(animatingCardWidget->pos());
        animationPos->setEndValue(endPosInGameMain - animatingCardWidget->rect().center());
        animationPos->setEasingCurve(QEasingCurve::OutQuad);
        animationGroup->addAnimation(animationPos);

        QPropertyAnimation* animationSize = new QPropertyAnimation(animatingCardWidget, "size", this);
        animationSize->setDuration(500); // 动画持续时间 500 毫秒
        animationSize->setStartValue(startSize);
        animationSize->setEndValue(endSize);
        animationSize->setEasingCurve(QEasingCurve::OutQuad);
        animationGroup->addAnimation(animationSize);


        // 7. 连接动画完成信号
        connect(animationGroup, &QParallelAnimationGroup::finished, this, [this, animatingCardWidget, buyer, cardToBuy, opId]() {
            animatingCardWidget->deleteLater(); // 删除临时动画卡牌
        });

        animationGroup->start(QAbstractAnimation::DeleteWhenStopped); // 动画结束后自动删除动画组

        QTimer::singleShot(500, this, [this, opId](){
            emit responseUserInput(opId);
        });
        break;
    }
    }
}
