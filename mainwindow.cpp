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
#include <QDebug>
#include <QRandomGenerator>

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

    setupGameMainLayout(gameMainLayout, m_state->getPlayers());

    m_animationOverlayWidget = new QWidget(centralWidget);
    m_animationOverlayWidget->setAttribute(Qt::WA_TransparentForMouseEvents);
    m_animationOverlayWidget->setStyleSheet("background: transparent;");

    centralLayout->addWidget(m_animationOverlayWidget);
    centralLayout->addWidget(m_gameMainWidget);

    connect(m_cardStoreArea, &CardStoreAreaWidget::cardWidgetRequestShowDetailed, this, &MainWindow::showDetailedCard);
    connect(m_cardStoreArea, &CardStoreAreaWidget::cardWidgetRequestHideDetailed, this, &MainWindow::hideDetailedCard);
}

MainWindow::~MainWindow()
{
    delete ui;
    hideDetailedCard();
}

// 设置游戏主布局的辅助函数
void MainWindow::setupGameMainLayout(QGridLayout* layout, const QList<Player*>& players) {
    // 日志查看器
    LogViewerWidget* logViewerDummy = new LogViewerWidget(m_gameMainWidget);
    QObject::connect(m_state,&GameState::logMessageAdded,logViewerDummy,&LogViewerWidget::appendLogMessage);
    layout->addWidget(logViewerDummy, 23, 100, 40, 30);

    // 玩家布局配置数据
    QVector<PlayerLayoutConfig> playerConfigs(players.size());

    // 玩家 0 (底部)
    playerConfigs[0] = {
        70, 0, 20, 20, // photo
        75, 20, 15, 120, true, // card area (horizontal)
        true, 65, 45, 10, 70, true, true, // landmark area (horizontal, special player 0)
        [](int w, int h) { return QPoint(w / 2, h + 100); }
    };

    // 玩家 1 (左侧)
    playerConfigs[1] = {
        0, 0, 15, 15, // photo
        15, 0, 50, 12, false, // card area (vertical)
        true, 15, 12, 50, 8, false, false, // landmark area (vertical)
        [](int w, int h) { return QPoint(-100, h / 2); }
    };

    // 玩家 2 (顶部中间)
    playerConfigs[2] = {
        0, 70, 15, 15, // photo
        0, 20, 12, 50, true, // card area (horizontal)
        true, 12, 20, 8, 50, true, false, // landmark area (horizontal)
        [](int w, int h) { return QPoint(w / 3, -100); }
    };

    // 玩家 3 (顶部右侧)
    playerConfigs[3] = {
        0, 140, 15, 15, // photo
        0, 90, 12, 50, true, // card area (horizontal)
        true, 12, 90, 8, 50, true, false, // landmark area (horizontal)
        [](int w, int h) { return QPoint(w / 3 * 2, -100); }
    };

    // 玩家 4 (右侧) - 注意：原代码中玩家4没有地标区
    playerConfigs[4] = {
        70, 145, 15, 15, // photo
        20, 148, 50, 12, false, // card area (vertical)
        false, 0, 0, 0, 0, false, false, // 无地标区
        [](int w, int h) { return QPoint(w + 100, h / 2); }
    };

    // 循环设置所有玩家的UI
    for (int i = 0; i < players.size(); ++i) {
        // ******** 新增：存储玩家的布局配置 ********
        m_playerLayoutConfigs.insert(players[i], playerConfigs[i]);
        // ******** 结束新增 ********

        setupPlayerWidgets(layout, players[i], playerConfigs[i]);
    }

    // 商店
    m_cardStoreArea = new CardStoreAreaWidget(m_gameMainWidget);
    m_cardStoreArea->setGameState(m_state);
    layout->addWidget(m_cardStoreArea, 23, 25, 40, 70);

    // 骰子
    DiceAreaWidget* diceAreaWidget= new DiceAreaWidget(m_state->getDice(),m_gameMainWidget);
    layout->addWidget(diceAreaWidget,50,60,12,35);
}

// 设置单个玩家UI的辅助函数
void MainWindow::setupPlayerWidgets(QGridLayout* layout, Player* player, const PlayerLayoutConfig& config) {
    // 玩家头像
    PlayerPhotoWidget* playerPhoto = new PlayerPhotoWidget(player, m_gameMainWidget);
    QObject::connect(player, &Player::coinsChange, playerPhoto, &PlayerPhotoWidget::onCoinsChange);
    QObject::connect(m_state, &GameState::currentPlayerChanged, playerPhoto, &PlayerPhotoWidget::onCurrentPlayerChanged);
    layout->addWidget(playerPhoto, config.photoRow, config.photoCol, config.photoRowSpan, config.photoColSpan);

    // 玩家卡牌区域
    PlayerAreaWidget* playerCardArea = new PlayerAreaWidget(player, config.cardAreaIsHorizontal, false, m_gameMainWidget);
    layout->addWidget(playerCardArea, config.cardAreaRow, config.cardAreaCol, config.cardAreaRowSpan, config.cardAreaColSpan);
    m_playerToCardAreaMap.insert(player, playerCardArea);
    connect(playerCardArea, &PlayerAreaWidget::cardWidgetRequestShowDetailed, this, &MainWindow::showDetailedCard);
    connect(playerCardArea, &PlayerAreaWidget::cardWidgetRequestHideDetailed, this, &MainWindow::hideDetailedCard);

    // 玩家地标区域 (如果存在)
    if (config.hasLandmarkArea) {
        PlayerAreaWidget* playerLandmarkArea = new PlayerAreaWidget(player, config.landmarkAreaIsHorizontal, true, m_gameMainWidget, config.landmarkAreaIsSpecialPlayer0);
        layout->addWidget(playerLandmarkArea, config.landmarkAreaRow, config.landmarkAreaCol, config.landmarkAreaRowSpan, config.landmarkAreaColSpan);
        m_playerToLandmarkAreaMap.insert(player, playerLandmarkArea);
        connect(playerLandmarkArea, &PlayerAreaWidget::cardWidgetRequestShowDetailed, this, &MainWindow::showDetailedCard);
        connect(playerLandmarkArea, &PlayerAreaWidget::cardWidgetRequestHideDetailed, this, &MainWindow::hideDetailedCard);
    }

    // ******** 移除：这里不再立即计算并存储窗口外坐标 ********
    // m_playerOutOfWindowTargetPos.insert(player, config.getOutOfWindowPos(m_gameMainWidget->width(), m_gameMainWidget->height()));
    // ******** 结束移除 ********
}

CardStore* MainWindow::findCardStoreForCard(Card* card, int& posInStore) {
    QList<CardStore*> stores = m_state->getCardStores();
    for (CardStore* store : stores) {
        for (int i = 0; i < store->getSlots().size(); ++i) {
            if (store->getSlots().at(i).contains(card)) {
                posInStore = i;
                // 注意：这里原代码直接delCard，如果只是查找，不应该删除。
                // 如果这是动画前准备，确保GameState逻辑正确处理卡牌的移除。
                // store->delCard(card);
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
    case PromptData::DiceAnimation: {
        QList<int> diceNumbers = pd.diceNum; // 目标骰子点数
        int opId = pd.autoInput; // 操作ID

        QTimer::singleShot(0, this, [=]() { // 外层 lambda 使用 [=] 隐式捕获 'this'
            if (diceNumbers.isEmpty() || !m_animationOverlayWidget) {
                qWarning() << "DiceAnimation (delayed): Invalid data or widgets.";
                emit responseUserInput(opId);
                return;
            }

            // 1. 创建临时的 Dice 对象和 DiceAreaWidget
            Dice* animatingDice = new Dice(this);
            animatingDice->setFirstNum(0);
            animatingDice->setSecondNum(0);

            DiceAreaWidget* animatingDiceAreaWidget = new DiceAreaWidget(animatingDice, m_animationOverlayWidget);

            // 2. 计算 DiceAreaWidget 的中心位置和大小
            QSize overlaySize = m_animationOverlayWidget->size();
            int targetWidth = static_cast<int>(overlaySize.width() * 0.4); // 占叠加层宽度的20%
            int targetHeight = static_cast<int>(overlaySize.height() * 0.3); // 占叠加层高度的15%
            QSize diceAreaSize(targetWidth, targetHeight);

            // 计算居中矩形
            QRect targetRect(QPoint((overlaySize.width() - targetWidth) / 2, (overlaySize.height() - targetHeight) / 2), diceAreaSize);
            animatingDiceAreaWidget->setGeometry(targetRect);

            // 3. 应用阴影效果
            QGraphicsDropShadowEffect *shadowEffect = new QGraphicsDropShadowEffect(animatingDiceAreaWidget);
            shadowEffect->setBlurRadius(25); // 模糊半径
            shadowEffect->setColor(QColor(0, 0, 0, 150)); // 阴影颜色 (半透明黑色)
            shadowEffect->setOffset(6, 6); // 阴影偏移
            animatingDiceAreaWidget->setGraphicsEffect(shadowEffect); // DiceAreaWidget 接管所有权

            animatingDiceAreaWidget->show(); // 显示动画用的 DiceAreaWidget

            // 4. 定义动画序列
            QSequentialAnimationGroup* sequentialGroup = new QSequentialAnimationGroup(animatingDiceAreaWidget); // 以 DiceAreaWidget 为父对象

            // 阶段1: DiceAreaWidget 渐入
            QPropertyAnimation* fadeInAnim = new QPropertyAnimation(animatingDiceAreaWidget, "windowOpacity");
            fadeInAnim->setDuration(300); // 渐入时间 300ms
            fadeInAnim->setStartValue(0.0);
            fadeInAnim->setEndValue(1.0);
            fadeInAnim->setEasingCurve(QEasingCurve::OutQuad);
            sequentialGroup->addAnimation(fadeInAnim);

            // 阶段2: 骰子滚动 (显示随机数)
            const int rollDuration = 1500; // 滚动持续时间 1.5 秒
            const int rollInterval = 150;  // 每 150ms 更新一次骰子数字

            QTimer* rollTimer = new QTimer(animatingDiceAreaWidget); // 以 DiceAreaWidget 为父对象
            QObject::connect(rollTimer, &QTimer::timeout, this, [animatingDice, diceNumbers]() {
                // 生成随机数 (1到6)
                animatingDice->setFirstNum(QRandomGenerator::global()->bounded(1, 7));
                if (diceNumbers.size() > 1) { // 如果需要显示两个骰子
                    animatingDice->setSecondNum(QRandomGenerator::global()->bounded(1, 7));
                } else {
                    animatingDice->setSecondNum(0); // 隐藏第二个骰子
                }
            });

            QPauseAnimation* rollingPause = new QPauseAnimation(rollDuration);
            sequentialGroup->addAnimation(rollingPause);

            // FIX for Error 1: 使用 sequentialGroup 的 currentAnimationChanged 信号来控制 rollTimer 的启停
            QObject::connect(sequentialGroup, &QSequentialAnimationGroup::currentAnimationChanged, this,
                             [rollTimer, rollInterval, rollingPause](QAbstractAnimation* current) {
                                 if (current == rollingPause) {
                                     rollTimer->start(rollInterval); // 当 rollingPause 成为当前动画时启动计时器
                                 } else {
                                     // 当切换到其他动画时停止计时器。此时 rollTimer 仍然有效。
                                     rollTimer->stop();
                                 }
                             });

            // 当 rollingPause 动画结束时，设置最终骰子点数并显示总和标签
            QObject::connect(rollingPause, &QPauseAnimation::finished, this,
                             [animatingDice, diceNumbers, animatingDiceAreaWidget]() { // 移除 rollTimer 的捕获
                                 // rollTimer->stop(); // REMOVED: 不在这里停止和删除 rollTimer
                                 // rollTimer->deleteLater(); // REMOVED: 不在这里停止和删除 rollTimer

                                 // 设置最终骰子点数
                                 animatingDice->setFirstNum(diceNumbers.at(0));
                                 if (diceNumbers.size() > 1) {
                                     animatingDice->setSecondNum(diceNumbers.at(1));
                                 } else {
                                     animatingDice->setSecondNum(0);
                                 }
                             });

            // 阶段3: 骰子停止后的短暂暂停
            QPauseAnimation* settlePause = new QPauseAnimation(500); // 暂停 0.5 秒
            sequentialGroup->addAnimation(settlePause);

            // 阶段4: DiceAreaWidget 渐出
            QPropertyAnimation* fadeOutAnim = new QPropertyAnimation(animatingDiceAreaWidget, "windowOpacity");
            fadeOutAnim->setDuration(500); // 渐出时间 500ms
            fadeOutAnim->setStartValue(1.0);
            fadeOutAnim->setEndValue(0.0);
            fadeOutAnim->setEasingCurve(QEasingCurve::InQuad);
            sequentialGroup->addAnimation(fadeOutAnim);

            // 5. 动画结束后清理资源并发出响应
            // FIX: 在 lambda 捕获列表中显式捕获 'this' 和 'rollTimer'
            QObject::connect(sequentialGroup, &QSequentialAnimationGroup::finished, this,
                             [this, animatingDiceAreaWidget, animatingDice, opId, rollTimer]() { // 捕获 rollTimer
                                 animatingDiceAreaWidget->deleteLater(); // 删除 DiceAreaWidget
                                 animatingDice->deleteLater(); // 删除临时的 Dice 对象
                                 rollTimer->deleteLater(); // 在整个动画组结束后删除 rollTimer
                                 emit responseUserInput(opId); // 发出用户输入响应
                             });

            sequentialGroup->start(QAbstractAnimation::DeleteWhenStopped); // 启动动画组，并在完成后自动删除
        });

        break;
    }
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

        QTimer::singleShot(0, this, [=]() {
            if (!buyer || !cardToBuy || !m_cardStoreArea || !m_animationOverlayWidget) {
                qWarning() << "BuyCardAnimation (delayed): Invalid data or widgets.";
                emit responseUserInput(opId);
                return;
            }

            // ******** 新增：在动画开始前重新计算所有玩家的窗口外目标位置 ********
            int gameMainWidth = m_gameMainWidget->width();
            int gameMainHeight = m_gameMainWidget->height();

            for (Player* p : m_playerLayoutConfigs.keys()) {
                const PlayerLayoutConfig& config = m_playerLayoutConfigs.value(p);
                m_playerOutOfWindowTargetPos.insert(p, config.getOutOfWindowPos(gameMainWidth, gameMainHeight));
            }
            // ******** 结束新增 ********

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

            // ******** 使用 m_playerOutOfWindowTargetPos 获取目标位置 (现在这个Map是最新计算的) ********
            QPoint outOfWindowPosInGameMain = m_playerOutOfWindowTargetPos.value(buyer,
                                                                                 // 默认值，如果买家未找到（理论上不应该发生）
                                                                                 QPoint(gameMainWidth / 2, -200));

            QPoint outOfWindowPosGlobal = m_gameMainWidget->mapToGlobal(outOfWindowPosInGameMain);
            QPoint outOfWindowPosInOverlay = m_animationOverlayWidget->mapFromGlobal(outOfWindowPosGlobal);

            // ******** 计算 1/4 处的中间点 ********
            QPoint midPointInOverlay = startPosInOverlay + (outOfWindowPosInOverlay - startPosInOverlay) / 4;

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

            // 设置阴影 - 为此控件创建新的 QGraphicsDropShadowEffect
            QGraphicsDropShadowEffect *shadowEffect = new QGraphicsDropShadowEffect(animatingCardWidget); // 父对象是 animatingCardWidget
            shadowEffect->setBlurRadius(15);
            shadowEffect->setColor(QColor(0, 0, 0, 120));
            shadowEffect->setOffset(4, 4);
            animatingCardWidget->setGraphicsEffect(shadowEffect); // animatingCardWidget 接管所有权

            qDebug() << "BuyCardAnimation Debug (Delayed Overlay Method):";
            qDebug() << "  Overlay size is:" << m_animationOverlayWidget->size();
            qDebug() << "  Calculated startRect is:" << startRect;
            qDebug() << "  Calculated endRect is:" << endRect;
            qDebug() << "  Initial widget geometry is:" << animatingCardWidget->geometry();

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

// 显示详细卡牌的槽函数
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

    // 1. 创建阴影效果，并将其父对象设置为 m_detailedCardWidget
    QGraphicsDropShadowEffect *shadowEffect = new QGraphicsDropShadowEffect(m_detailedCardWidget);
    shadowEffect->setBlurRadius(15);
    shadowEffect->setColor(QColor(0, 0, 0, 120));
    shadowEffect->setOffset(4, 4);
    // 2. 将阴影效果设置给 m_detailedCardWidget
    m_detailedCardWidget->setGraphicsEffect(shadowEffect); // m_detailedCardWidget 接管 shadowEffect 的所有权

    // 设置详细卡牌的尺寸
    QSize overlaySize = m_animationOverlayWidget->size();
    int targetWidth = static_cast<int>(overlaySize.width() * 0.15);
    int targetHeight = targetWidth * 3 / 2; // 等比例
    QSize detailedCardSize(targetWidth, targetHeight);

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

    // 3. 直接动画 shadowEffect 的 opacity 属性，而不是创建一个新的 QGraphicsOpacityEffect
    m_fadeAnimation = new QPropertyAnimation(shadowEffect, "opacity", this); // 动画目标是 shadowEffect
    m_fadeAnimation->setDuration(200); // 渐变出现时间 200ms
    m_fadeAnimation->setStartValue(0.0);
    m_fadeAnimation->setEndValue(1.0);
    m_fadeAnimation->setEasingCurve(QEasingCurve::OutQuad);

    m_detailedCardWidget->show();
    m_fadeAnimation->start();
}

// 隐藏详细卡牌的槽函数
void MainWindow::hideDetailedCard()
{
    // 如果渐变动画正在运行，停止它并安全删除
    if (m_fadeAnimation) {
        m_fadeAnimation->stop();
        m_fadeAnimation->deleteLater();
        m_fadeAnimation = nullptr;
    }

    // 删除详细卡牌 widget
    if (m_detailedCardWidget) {
        m_detailedCardWidget->deleteLater();
        m_detailedCardWidget = nullptr;
    }
}
