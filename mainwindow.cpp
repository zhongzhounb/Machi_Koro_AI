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
#include <QParallelAnimationGroup>
#include <QSequentialAnimationGroup>
#include <QPauseAnimation>

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

    m_gameMainWidget = new QWidget(centralWidget);
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

    // 当前玩家 (Player 0) - 自己的区域在底部
    PlayerPhotoWidget* playerPhoto0 = new PlayerPhotoWidget(players[0],m_gameMainWidget);
    QObject::connect(players[0], &Player::coinsChange, playerPhoto0, &PlayerPhotoWidget::onCoinsChange);
    QObject::connect(m_state,&GameState::currentPlayerChanged,playerPhoto0,&PlayerPhotoWidget::onCurrentPlayerChanged);
    gameMainLayout->addWidget(playerPhoto0, 70, 0, 20, 20);

    PlayerAreaWidget* playerCardArea0=new PlayerAreaWidget(players[0],true,false,m_gameMainWidget);
    gameMainLayout->addWidget(playerCardArea0, 75, 20, 15, 120);
    m_playerToCardAreaMap.insert(players[0], playerCardArea0);
    // 玩家0的窗口外坐标 (下方)
    m_playerOutOfWindowTargetPos.insert(players[0], QPoint(m_gameMainWidget->width() / 2, m_gameMainWidget->height() + 100));


    PlayerAreaWidget* playerLandmarkArea0=new PlayerAreaWidget(players[0],true,true,m_gameMainWidget,true);
    gameMainLayout->addWidget(playerLandmarkArea0, 65, 45, 10, 70);
    m_playerToLandmarkAreaMap.insert(players[0], playerLandmarkArea0);

    // 1号玩家 (左侧)
    PlayerPhotoWidget* playerPhoto1 = new PlayerPhotoWidget(players[1],m_gameMainWidget);
    QObject::connect(players[1], &Player::coinsChange, playerPhoto1, &PlayerPhotoWidget::onCoinsChange);
    QObject::connect(m_state,&GameState::currentPlayerChanged,playerPhoto1,&PlayerPhotoWidget::onCurrentPlayerChanged);
    gameMainLayout->addWidget(playerPhoto1, 0, 0, 15, 15);

    PlayerAreaWidget* playerCardArea1=new PlayerAreaWidget(players[1],false,false,m_gameMainWidget);
    gameMainLayout->addWidget(playerCardArea1, 15, 0, 50, 12);
    m_playerToCardAreaMap.insert(players[1], playerCardArea1);
    // 玩家1的窗口外坐标 (左侧)
    m_playerOutOfWindowTargetPos.insert(players[1], QPoint(-100, m_gameMainWidget->height() / 2));


    PlayerAreaWidget* playerLandmarkArea1=new PlayerAreaWidget(players[1],false,true,m_gameMainWidget);
    gameMainLayout->addWidget(playerLandmarkArea1, 15, 12, 50, 8);
    m_playerToLandmarkAreaMap.insert(players[1], playerLandmarkArea1);

    // 2号玩家 (顶部中间)
    PlayerPhotoWidget* playerPhoto2 = new PlayerPhotoWidget(players[2],m_gameMainWidget);
    QObject::connect(players[2], &Player::coinsChange, playerPhoto2, &PlayerPhotoWidget::onCoinsChange);
    QObject::connect(m_state,&GameState::currentPlayerChanged,playerPhoto2,&PlayerPhotoWidget::onCurrentPlayerChanged);
    gameMainLayout->addWidget(playerPhoto2, 0, 70, 15, 15);

    PlayerAreaWidget* playerCardArea2=new PlayerAreaWidget(players[2],true,false,m_gameMainWidget);
    gameMainLayout->addWidget(playerCardArea2, 0, 20, 12, 50);
    m_playerToCardAreaMap.insert(players[2], playerCardArea2);
    // 玩家2的窗口外坐标 (上方)
    m_playerOutOfWindowTargetPos.insert(players[2], QPoint(m_gameMainWidget->width() / 2, -100));


    PlayerAreaWidget* playerLandmarkArea2=new PlayerAreaWidget(players[2],true,true,m_gameMainWidget);
    gameMainLayout->addWidget(playerLandmarkArea2, 12, 20, 8, 50);
    m_playerToLandmarkAreaMap.insert(players[2], playerLandmarkArea2);

    // 3号玩家 (顶部右侧)
    PlayerPhotoWidget* playerPhoto3 = new PlayerPhotoWidget(players[3],m_gameMainWidget);
    QObject::connect(players[3], &Player::coinsChange, playerPhoto3, &PlayerPhotoWidget::onCoinsChange);
    QObject::connect(m_state,&GameState::currentPlayerChanged,playerPhoto3,&PlayerPhotoWidget::onCurrentPlayerChanged);
    gameMainLayout->addWidget(playerPhoto3, 0, 140, 15, 15);
    PlayerAreaWidget* playerCardArea3=new PlayerAreaWidget(players[3],true,false,m_gameMainWidget);
    gameMainLayout->addWidget(playerCardArea3, 0, 90, 12, 50);
    m_playerToCardAreaMap.insert(players[3], playerCardArea3);
    // 玩家3的窗口外坐标 (上方偏右)
    m_playerOutOfWindowTargetPos.insert(players[3], QPoint(m_gameMainWidget->width() + 100, -50));


    PlayerAreaWidget* playerLandmarkArea3=new PlayerAreaWidget(players[3],true,true,m_gameMainWidget);
    gameMainLayout->addWidget(playerLandmarkArea3, 12, 90, 8, 50);
    m_playerToLandmarkAreaMap.insert(players[3], playerLandmarkArea3);

    // 4号玩家 (右侧)
    PlayerPhotoWidget* playerPhoto4 = new PlayerPhotoWidget(players[4],m_gameMainWidget);
    QObject::connect(players[4], &Player::coinsChange, playerPhoto4, &PlayerPhotoWidget::onCoinsChange);
    QObject::connect(m_state,&GameState::currentPlayerChanged,playerPhoto4,&PlayerPhotoWidget::onCurrentPlayerChanged);
    gameMainLayout->addWidget(playerPhoto4, 70, 145, 15, 15);
    PlayerAreaWidget* playerCardArea4=new PlayerAreaWidget(players[4],false,false,m_gameMainWidget);
    gameMainLayout->addWidget(playerCardArea4, 20, 148, 50, 12);
    m_playerToCardAreaMap.insert(players[4], playerCardArea4);
    // 玩家4的窗口外坐标 (右侧)
    m_playerOutOfWindowTargetPos.insert(players[4], QPoint(m_gameMainWidget->width() + 100, m_gameMainWidget->height() / 2));


    // 商店
    m_cardStoreArea = new CardStoreAreaWidget(m_gameMainWidget);
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
        for (int i = 0; i < store->getSlots().size(); ++i) {
            if (store->getSlots().at(i).contains(card)) {
                posInStore = i;
                store->delCard(card);
                return store;
            }
        }
    }
    posInStore = -1;
    return nullptr;
}

// 辅助函数：计算动画的中间点
QPoint MainWindow::getMidPointForAnimation(const QPoint& startPos, const QPoint& endPos) {
    return (startPos + endPos) / 2;
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

        // **强制布局更新和重绘，确保所有部件都已在最终位置**
        if (m_gameMainWidget->layout()) {
            m_gameMainWidget->layout()->activate();
            m_gameMainWidget->layout()->update();
            QCoreApplication::processEvents(QEventLoop::ExcludeUserInputEvents | QEventLoop::WaitForMoreEvents, 50); // 稍微等待一下
        }
        m_gameMainWidget->repaint();
        QCoreApplication::processEvents(QEventLoop::ExcludeUserInputEvents | QEventLoop::WaitForMoreEvents, 50); // 再次处理，确保重绘完成


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

        qDebug() << "BuyCardAnimation Debug:";
        qDebug() << "  CardStoreAreaWidget pos:" << m_cardStoreArea->pos() << "size:" << m_cardStoreArea->size();
        qDebug() << "  Slot center in CardStoreArea local:" << startPosInCardStoreArea;
        qDebug() << "  Slot center Global:" << startPosGlobal;
        qDebug() << "  Slot center in GameMain local (Start Pos):" << startPosInGameMain;


        // 3. 获取动画的最终结束位置 (玩家窗口外坐标)
        QPoint outOfWindowPos = m_playerOutOfWindowTargetPos.value(buyer);
        if (outOfWindowPos.isNull()) {
            qWarning() << "BuyCardAnimation: Out-of-window target position not set for buyer" << buyer->getName() << ". Using default.";
            outOfWindowPos = QPoint(m_gameMainWidget->width() / 2, -200);
        }

        // 4. 计算动画的中间点
        QPoint midPointInGameMain = getMidPointForAnimation(startPosInGameMain, outOfWindowPos);


        // 5. 创建用于动画的临时 CardWidget
        // 注意：将父对象设置为 MainWindow，使其成为顶级部件，这样可以完全脱离布局管理器的影响
        CardWidget* animatingCardWidget = new CardWidget(cardToBuy, ShowType::Ordinary, this); // 父对象改为 this (MainWindow)

        // 获取起始卡槽的实际尺寸，作为动画卡牌的初始尺寸
        QList<SlotWidget*> storeSlots = m_cardStoreArea->m_storeToSlotsMap.value(sourceStore);
        SlotWidget* sourceSlot = nullptr;
        if (cardPosInStore + 1 < storeSlots.size()) {
            sourceSlot = storeSlots.at(cardPosInStore + 1);
        }

        QSize startSize = sourceSlot ? sourceSlot->size() : QSize(80, 120);
        QSize midSize = startSize * 1.5;
        QSize endSize = QSize(0, 0);

        animatingCardWidget->setFixedSize(startSize);
        // 计算动画卡牌的左上角起始位置，使其中心与 startPosGlobal 对齐
        // 因为 animatingCardWidget 的父对象是 MainWindow，所以它的位置是相对于 MainWindow 的
        // 我们需要将 startPosGlobal 转换为 MainWindow 的局部坐标
        QPoint desiredStartTopLeft = this->mapFromGlobal(startPosGlobal) - animatingCardWidget->rect().center();
        animatingCardWidget->move(desiredStartTopLeft); // 设置初始位置

        animatingCardWidget->show();
        animatingCardWidget->raise(); // 确保它在最上层


        qDebug() << "  Calculated desired start top-left for animatingCardWidget (relative to MainWindow):" << desiredStartTopLeft;
        qDebug() << "  Animating CardWidget actual pos after move (top-left, relative to MainWindow):" << animatingCardWidget->pos();
        qDebug() << "  Animating CardWidget initial size:" << animatingCardWidget->size();

        // 重新计算中间点和结束点相对于 MainWindow 的位置
        // midPointInGameMain 和 outOfWindowPos 之前是相对于 m_gameMainWidget 的
        // 现在需要转换为相对于 MainWindow 的
        QPoint midPointGlobal = m_gameMainWidget->mapToGlobal(midPointInGameMain);
        QPoint midPointInMainWindow = this->mapFromGlobal(midPointGlobal);

        QPoint outOfWindowGlobal = m_gameMainWidget->mapToGlobal(outOfWindowPos);
        QPoint outOfWindowInMainWindow = this->mapFromGlobal(outOfWindowGlobal);


        // 6. 创建动画序列
        QSequentialAnimationGroup* sequentialGroup = new QSequentialAnimationGroup(this);

        // --- 第一阶段：从商店到中间点，卡牌放大 ---
        QParallelAnimationGroup* phase1Group = new QParallelAnimationGroup(this);

        QPropertyAnimation* animPos1 = new QPropertyAnimation(animatingCardWidget, "pos", this);
        animPos1->setDuration(400);
        animPos1->setStartValue(desiredStartTopLeft);
        animPos1->setEndValue(midPointInMainWindow - animatingCardWidget->rect().center()); // 目标 top-left 位置
        animPos1->setEasingCurve(QEasingCurve::OutQuad);
        phase1Group->addAnimation(animPos1);

        QPropertyAnimation* animSize1 = new QPropertyAnimation(animatingCardWidget, "size", this);
        animSize1->setDuration(400);
        animSize1->setStartValue(startSize);
        animSize1->setEndValue(midSize);
        animSize1->setEasingCurve(QEasingCurve::OutQuad);
        phase1Group->addAnimation(animSize1);

        sequentialGroup->addAnimation(phase1Group);

        // --- 第二阶段：在中间点暂停 0.8 秒 ---
        QPauseAnimation* pauseAnim = new QPauseAnimation(800, this);
        sequentialGroup->addAnimation(pauseAnim);

        // --- 第三阶段：从中间点到屏幕外，卡牌缩小消失 ---
        QParallelAnimationGroup* phase2Group = new QParallelAnimationGroup(this);

        QPropertyAnimation* animPos2 = new QPropertyAnimation(animatingCardWidget, "pos", this);
        animPos2->setDuration(400);
        animPos2->setStartValue(midPointInMainWindow - animatingCardWidget->rect().center());
        animPos2->setEndValue(outOfWindowInMainWindow - animatingCardWidget->rect().center());
        animPos2->setEasingCurve(QEasingCurve::InQuad);
        phase2Group->addAnimation(animPos2);

        QPropertyAnimation* animSize2 = new QPropertyAnimation(animatingCardWidget, "size", this);
        animSize2->setDuration(400);
        animSize2->setStartValue(midSize);
        animSize2->setEndValue(endSize);
        animSize2->setEasingCurve(QEasingCurve::InQuad);
        phase2Group->addAnimation(animSize2);

        sequentialGroup->addAnimation(phase2Group);


        // 7. 连接动画完成信号
        connect(sequentialGroup, &QSequentialAnimationGroup::finished, this, [this, animatingCardWidget, buyer, cardToBuy, opId]() {
            animatingCardWidget->deleteLater();

            //m_state->playerBuyCard(buyer, cardToBuy);

            emit responseUserInput(opId);
        });

        sequentialGroup->start(QAbstractAnimation::DeleteWhenStopped);

        break;
    }
    }
}
