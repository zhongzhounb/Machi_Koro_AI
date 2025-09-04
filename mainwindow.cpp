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
#include <QGraphicsDropShadowEffect>
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
    m_playerOutOfWindowTargetPos.insert(players[2], QPoint(m_gameMainWidget->width() /3, -100));


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
    m_playerOutOfWindowTargetPos.insert(players[3], QPoint(m_gameMainWidget->width() /3*2, -100));


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

    // ******** 新增：创建和设置动画覆盖层 ********
    m_animationOverlayWidget = new QWidget(centralWidget);
    // 让鼠标事件穿透此层，到达下面的游戏控件
    m_animationOverlayWidget->setAttribute(Qt::WA_TransparentForMouseEvents);
    // 确保它没有背景
    m_animationOverlayWidget->setStyleSheet("background: transparent;");

    // ******** 修改：将控件添加到 Stacked Layout ********
    // 先添加动画覆盖层，它会浮在游戏界面之上
    centralLayout->addWidget(m_animationOverlayWidget);
    // 再添加游戏界面
    centralLayout->addWidget(m_gameMainWidget);

    // ******** 新增：连接 CardStoreAreaWidget 的详细卡牌显示/隐藏信号 ********
    connect(m_cardStoreArea, &CardStoreAreaWidget::cardWidgetRequestShowDetailed, this, &MainWindow::showDetailedCard);
    connect(m_cardStoreArea, &CardStoreAreaWidget::cardWidgetRequestHideDetailed, this, &MainWindow::hideDetailedCard);

    // ******** 新增：连接所有 PlayerAreaWidget 的详细卡牌显示/隐藏信号 ********
    for (PlayerAreaWidget* area : m_playerToCardAreaMap.values()) {
        connect(area, &PlayerAreaWidget::cardWidgetRequestShowDetailed, this, &MainWindow::showDetailedCard);
        connect(area, &PlayerAreaWidget::cardWidgetRequestHideDetailed, this, &MainWindow::hideDetailedCard);
    }
    for (PlayerAreaWidget* area : m_playerToLandmarkAreaMap.values()) {
        connect(area, &PlayerAreaWidget::cardWidgetRequestShowDetailed, this, &MainWindow::showDetailedCard);
        connect(area, &PlayerAreaWidget::cardWidgetRequestHideDetailed, this, &MainWindow::hideDetailedCard);
    }
}

MainWindow::~MainWindow()
{
    delete ui;
    // 确保在析构时调用 hideDetailedCard() 来清理所有相关的对象
    hideDetailedCard();
}

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
    case PromptData::BuyCardAnimation: {
        Player* buyer = pd.buyer;
        Card* cardToBuy = pd.card;
        int opId = pd.autoInput;

        // 确保在执行任何计算之前，主窗口已经完成其初始布局和大小调整。
        QTimer::singleShot(0, this, [=]() {
            if (!buyer || !cardToBuy || !m_cardStoreArea || !m_animationOverlayWidget) {
                qWarning() << "BuyCardAnimation (delayed): Invalid data or widgets.";
                emit responseUserInput(opId);
                return;
            }

            int cardPosInStore = -1;
            CardStore* sourceStore = findCardStoreForCard(cardToBuy, cardPosInStore);
            if (!sourceStore || cardPosInStore == -1) {
                qWarning() << "BuyCardAnimation (delayed): Source CardStore not found for card" << cardToBuy->getName();
                emit responseUserInput(opId);
                return;
            }

            // --- 1. 现在进行坐标和尺寸计算 ---
            QPoint startPosInCardStoreArea = m_cardStoreArea->getStoreSlotCenterPos(sourceStore, cardPosInStore);
            QPoint startPosGlobal = m_cardStoreArea->mapToGlobal(startPosInCardStoreArea);
            QPoint startPosInOverlay = m_animationOverlayWidget->mapFromGlobal(startPosGlobal);

            QList<SlotWidget*> storeSlots = m_cardStoreArea->m_storeToSlotsMap.value(sourceStore);
            SlotWidget* sourceSlot = (cardPosInStore < storeSlots.size()) ? storeSlots.at(cardPosInStore) : nullptr;
            QSize startSize = sourceSlot ? sourceSlot->size() : QSize(80, 120);

            // ******** 关键修改：在此处动态计算 outOfWindowPosInGameMain ********
            QPoint outOfWindowPosInGameMain;
            QList<Player*> players = m_state->getPlayers(); // 获取玩家列表以确定是哪个玩家
            if (buyer == players[0]) { // 玩家0 (底部)
                outOfWindowPosInGameMain = QPoint(m_gameMainWidget->width() / 2, m_gameMainWidget->height() + 100);
            } else if (buyer == players[1]) { // 玩家1 (左侧)
                outOfWindowPosInGameMain = QPoint(-100, m_gameMainWidget->height() / 2);
            } else if (buyer == players[2]) { // 玩家2 (顶部中间)
                outOfWindowPosInGameMain = QPoint(m_gameMainWidget->width() /3, -100);
            } else if (buyer == players[3]) { // 玩家3 (顶部右侧)
                outOfWindowPosInGameMain = QPoint(m_gameMainWidget->width() /3*2, -100);
            } else if (buyer == players[4]) { // 玩家4 (右侧)
                outOfWindowPosInGameMain = QPoint(m_gameMainWidget->width() + 100, m_gameMainWidget->height() / 2);
            } else {
                // 默认值，如果买家未找到（理论上不应该发生）
                outOfWindowPosInGameMain = QPoint(m_gameMainWidget->width() / 2, -200);
            }
            // ******** 结束动态计算 ********

            QPoint outOfWindowPosGlobal = m_gameMainWidget->mapToGlobal(outOfWindowPosInGameMain);
            QPoint outOfWindowPosInOverlay = m_animationOverlayWidget->mapFromGlobal(outOfWindowPosGlobal);

            // ******** 修改点：计算 1/4 处的中间点 ********
            QPoint midPointInOverlay = startPosInOverlay + (outOfWindowPosInOverlay - startPosInOverlay) / 4;
            // ******** 结束修改点 ********

            QSize midSize = QSize(startSize.width(),startSize.width()*3/2) * 2;
            QSize endSize = midSize;

            // --- 2. 计算完整的几何矩形 (QRect) ---
            QRect startRect(startPosInOverlay - QRect(QPoint(0,0), startSize).center(), startSize);
            QRect midRect(midPointInOverlay - QRect(QPoint(0,0), midSize).center(), midSize);
            QRect endRect(outOfWindowPosInOverlay - QRect(QPoint(0,0), endSize).center(), endSize);

            // --- 3. 创建动画控件 ---
            CardWidget* animatingCardWidget = new CardWidget(cardToBuy, ShowType::Detailed, m_animationOverlayWidget);
            animatingCardWidget->setAnimated(true);
            animatingCardWidget->setGeometry(startRect);
            animatingCardWidget->show();

            //设置阴影
            QGraphicsDropShadowEffect *shadowEffect = new QGraphicsDropShadowEffect(this);
            shadowEffect->setBlurRadius(15);
            shadowEffect->setColor(QColor(0, 0, 0, 120));
            shadowEffect->setOffset(4, 4);
            animatingCardWidget->setGraphicsEffect(shadowEffect);

            qDebug() << "BuyCardAnimation Debug (Delayed Overlay Method):";
            qDebug() << "  Overlay size is:" << m_animationOverlayWidget->size();
            qDebug() << "  Calculated startRect is:" << startRect;
            qDebug() << "  Calculated endRect is:" << endRect; // 检查这个值
            qDebug() << "  Initial widget geometry is:" << animatingCardWidget->geometry();

            // ... 后续动画代码保持不变 ...
            QSequentialAnimationGroup* sequentialGroup = new QSequentialAnimationGroup(animatingCardWidget);

            QPropertyAnimation* animGeo1 = new QPropertyAnimation(animatingCardWidget, "geometry");
            animGeo1->setDuration(400);
            animGeo1->setStartValue(startRect);
            animGeo1->setEndValue(midRect);
            animGeo1->setEasingCurve(QEasingCurve::OutQuad);

            QPauseAnimation* pauseAnim = new QPauseAnimation(1500);

            QPropertyAnimation* animGeo2 = new QPropertyAnimation(animatingCardWidget, "geometry");
            animGeo2->setDuration(400);
            animGeo2->setEndValue(endRect);
            animGeo2->setEasingCurve(QEasingCurve::InQuad);

            sequentialGroup->addAnimation(animGeo1);
            sequentialGroup->addAnimation(pauseAnim);
            sequentialGroup->addAnimation(animGeo2);

            connect(sequentialGroup, &QSequentialAnimationGroup::finished, this, [=]() {
                animatingCardWidget->deleteLater();
                emit responseUserInput(opId);
            });

            sequentialGroup->start(QAbstractAnimation::DeleteWhenStopped);
        });

        break;
    }
    }
}

// 新增：显示详细卡牌的槽函数
void MainWindow::showDetailedCard(Card* card, QPoint globalPos)
{
    // 如果已经有详细卡牌在显示，先隐藏它
    if (m_detailedCardWidget) {
        hideDetailedCard();
    }

    if (!card) return;

    // 创建详细卡牌
    m_detailedCardWidget = new CardWidget(card, ShowType::Detailed, m_animationOverlayWidget);
    m_detailedCardWidget->setAnimated(true); // 标记为动画状态，防止其自身处理 hover 样式

    // 设置详细卡牌的尺寸
    QSize detailedCardSize(200, 333); // 示例尺寸，宽高比约为 0.57，接近 ShowType::Detailed 的 0.5625

    // 计算卡牌位置：鼠标位置 globalPos 转换为 overlay 的局部坐标
    QPoint localPosInOverlay = m_animationOverlayWidget->mapFromGlobal(globalPos);

    // 调整位置，使卡牌中心位于鼠标右下方一定偏移
    int offsetX = 20;
    int offsetY = 20;
    QPoint targetTopLeft = localPosInOverlay + QPoint(offsetX, offsetY);

    // 确保卡牌不会超出屏幕边界
    QRect overlayRect = m_animationOverlayWidget->rect();
    if (targetTopLeft.x() + detailedCardSize.width() > overlayRect.width()) {
        targetTopLeft.setX(localPosInOverlay.x() - detailedCardSize.width() - offsetX); // 如果超出右边界，则显示在鼠标左侧
    }
    if (targetTopLeft.y() + detailedCardSize.height() > overlayRect.height()) {
        targetTopLeft.setY(localPosInOverlay.y() - detailedCardSize.height() - offsetY); // 如果超出下边界，则显示在鼠标上方
    }
    // 避免超出左边界或上边界
    if (targetTopLeft.x() < 0) targetTopLeft.setX(0);
    if (targetTopLeft.y() < 0) targetTopLeft.setY(0);


    m_detailedCardWidget->setGeometry(QRect(targetTopLeft, detailedCardSize));

    // 设置阴影
    QGraphicsDropShadowEffect *shadowEffect = new QGraphicsDropShadowEffect(m_detailedCardWidget);
    shadowEffect->setBlurRadius(15);
    shadowEffect->setColor(QColor(0, 0, 0, 120));
    shadowEffect->setOffset(4, 4);
    m_detailedCardWidget->setGraphicsEffect(shadowEffect);

    // 设置透明度效果并开始渐变动画
    m_detailedCardOpacityEffect = new QGraphicsOpacityEffect(m_detailedCardWidget); // 父对象是 m_detailedCardWidget
    m_detailedCardWidget->setGraphicsEffect(m_detailedCardOpacityEffect);

    m_fadeAnimation = new QPropertyAnimation(m_detailedCardOpacityEffect, "opacity", this); // 父对象是 MainWindow
    m_fadeAnimation->setDuration(200); // 渐变出现时间 200ms
    m_fadeAnimation->setStartValue(0.0);
    m_fadeAnimation->setEndValue(1.0);
    m_fadeAnimation->setEasingCurve(QEasingCurve::OutQuad);

    m_detailedCardWidget->show();
    m_fadeAnimation->start(); // IMPORTANT: 移除 QAbstractAnimation::DeleteWhenStopped
}

// 新增：隐藏详细卡牌的槽函数
void MainWindow::hideDetailedCard()
{
    // 如果渐变动画正在运行，停止它并安全删除
    if (m_fadeAnimation) {
        m_fadeAnimation->stop();
        m_fadeAnimation->deleteLater(); // 使用 deleteLater 确保安全删除
        m_fadeAnimation = nullptr; // 将指针置空
    }

    // 删除详细卡牌 widget
    if (m_detailedCardWidget) {
        m_detailedCardWidget->deleteLater(); // 使用 deleteLater 确保安全删除
        m_detailedCardWidget = nullptr; // 将指针置空
        // m_detailedCardOpacityEffect 是 m_detailedCardWidget 的子对象，会随其父对象一同删除
        m_detailedCardOpacityEffect = nullptr; // 将指针置空
    }
}
