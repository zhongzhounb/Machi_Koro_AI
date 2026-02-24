#include "mainwindow.h"
#include "coins/coinchangewidget.h" // ADD THIS LINE
#include "ui_mainwindow.h"
#include "gamestate.h"
#include "QStackedLayout"
#include "QGridLayout"
#include "QWidget"
#include "cards/cardstoreareawidget.h"
#include "players/playerareawidget.h"
#include "players/playerphotowidget.h"
#include "logviewerwidget.h"
#include "dices/diceareawidget.h"
#include "player.h"
#include "cardstore.h"
#include "card.h"
#include "cards/slotwidget.h"
#include "cards/cardwidget.h"
#include "background/gamebackgroundwidget.h"
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
#include <QLabel>
#include <QPushButton> // For QPushButton
#include <QVBoxLayout> // For layouts
#include <QHBoxLayout>
#include <functional> // For std::function
#include <QGraphicsOpacityEffect> // For QGraphicsOpacityEffect
#include <QGraphicsColorizeEffect> // For QGraphicsColorizeEffect
#include <QPointer> // IMPORTANT: Include QPointer for safe object references
#include <QSharedPointer> // IMPORTANT: Include QSharedPointer for shared ownership
#include <QFile>
MainWindow::MainWindow(GameState* state, QWidget *parent)
    : m_state(state), QMainWindow(parent)
    , ui(new Ui::MainWindow)
    // ====== Handler 初始化保持原样 ======
    , m_startTurnAnimationHandler(new PromptStartTurnAnimationHandler(this))
    , m_buyCardAnimationHandler(new PromptBuyCardAnimationHandler(this))
    , m_cardInAnimationHandler(new PromptCardInAnimationHandler(this))
    , m_cardOutAnimationHandler(new PromptCardOutAnimationHandler(this))
    , m_diceAnimationHandler(new PromptDiceAnimationHandler(this))
    , m_popupHandler(new PromptPopupHandler(this))
    , m_waitCurtain(nullptr) // 关键：确保初始是空的
    , m_waitLabel(nullptr)   // 关键
    , m_selectCardHandler(new PromptSelectCardHandler(this))
    , m_selectDiceHandler(new PromptSelectDiceHandler(this))
    , m_selectPlayerHandler(new PromptSelectPlayerHandler(this))
{
    ui->setupUi(this);
    setContentsMargins(0, 0, 0, 0);

    // 基础架子
    QWidget *centralWidget = new QWidget(this);
    setCentralWidget(centralWidget);
    QStackedLayout* centralLayout = new QStackedLayout(centralWidget);
    centralLayout->setStackingMode(QStackedLayout::StackAll);

    // 1. 先创建背景层（这个很快）
    m_backgroundWidget = new GameBackgroundWidget(centralWidget);
    QObject::connect(m_state, &GameState::backgroundChanged,m_backgroundWidget, &GameBackgroundWidget::advanceState);

    // 2. 创建一个空的游戏主界面（先不塞入具体内容）
    m_gameMainWidget = new QWidget(centralWidget);
    m_gameMainWidget->hide();


    // 3. 创建动画覆盖层
    m_animationOverlayWidget = new QWidget(centralWidget);
    m_animationOverlayWidget->setAttribute(Qt::WA_TransparentForMouseEvents);

    // 4. 创建启动场景层（最顶层）
    m_startSceneWidget = new QWidget(centralWidget);
    m_startSceneWidget->setStyleSheet("background-color: black;");
    centralLayout->addWidget(m_startSceneWidget);
    centralLayout->addWidget(m_animationOverlayWidget);
    centralLayout->addWidget(m_gameMainWidget);
    centralLayout->addWidget(m_backgroundWidget);
    // 立即运行 Logo 动画
    runStartSequence();
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
        75, 20, 15, 120, true,  // card area (horizontal)
        65, 45, 10, 70, true, true, // landmark area (horizontal, special player 0)
        [](int w, int h) { return QPoint(w / 2, h/4*5); }, // getOutOfWindowPos
        [](int w, int h) { return QPoint(w / 2, h/4*3); } // getDisplayPos (example: near center bottom of gameMainWidget)
    };

    // 玩家 1 (左侧)
    playerConfigs[1] = {
        0, 0, 15, 15, // photo
        15, 0, 50, 12, false, // card area (vertical)
        15, 12, 50, 9, false, false, // landmark area (vertical)
        [](int w, int h) { return QPoint(-w/8, h / 2); }, // getOutOfWindowPos
        [](int w, int h) { return QPoint(w/8, h / 2); } // getDisplayPos (example: 50px from left, vertically centered in gameMainWidget)
    };

    // 玩家 2 (顶部左侧)
    playerConfigs[2] = {
        0, 70, 15, 15, // photo
        0, 20, 12, 50, true, // card area (horizontal)
        12, 20, 9, 50, true, false, // landmark area (horizontal)
        [](int w, int h) { return QPoint(w / 3, -h/4); }, // getOutOfWindowPos
        [](int w, int h) { return QPoint(w / 3 , h/4); } // getDisplayPos (example: at 1/3 width, 50px from top of gameMainWidget)
    };

    // 玩家 3 (顶部右侧)
    playerConfigs[3] = {
        0, 140, 15, 15, // photo
        0, 90, 12, 50, true, // card area (horizontal)
        12, 90, 9, 50, true, false, // landmark area (horizontal)
        [](int w, int h) { return QPoint(w / 3 * 2, -h/4); }, // getOutOfWindowPos
        [](int w, int h) { return QPoint(w / 3 * 2, h/4); } // getDisplayPos (example: at 2/3 width, 50px from top of gameMainWidget)
    };

    // 玩家 4 (右侧)
    playerConfigs[4] = {
        70, 145, 15, 15, // photo
        20, 148, 50, 12, false, // card area (vertical)
        20, 139, 50, 9, false, false, // landmark area (vertical)
        [](int w, int h) { return QPoint(w/8*9, h / 2); }, // getOutOfWindowPos
        [](int w, int h) { return QPoint(w/8*7, h / 2 ); } // getDisplayPos (example: 50px from right, vertically centered in gameMainWidget)
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
    m_mainDiceAreaWidget = diceAreaWidget;
}

// --- 逻辑拆分：启动动画流程 ---
void MainWindow::runStartSequence() {
    QLabel *logoLabel = new QLabel(m_startSceneWidget);
    logoLabel->setPixmap(QPixmap(":/resources/logo.jpg").scaled(400, 400, Qt::KeepAspectRatio, Qt::SmoothTransformation));
    qDebug() << "Logo exists:" << QFile::exists(":/resources/logo.png");
    QVBoxLayout *layout = new QVBoxLayout(m_startSceneWidget);
    layout->addWidget(logoLabel, 0, Qt::AlignCenter);

    QGraphicsOpacityEffect *opacityEffect = new QGraphicsOpacityEffect(logoLabel);
    logoLabel->setGraphicsEffect(opacityEffect);

    // 创建串行动画组
    QSequentialAnimationGroup *seq = new QSequentialAnimationGroup(this);

    // 淡入
    QPropertyAnimation *fadeIn = new QPropertyAnimation(opacityEffect, "opacity");
    fadeIn->setDuration(1200);
    fadeIn->setStartValue(0);
    fadeIn->setEndValue(1);

    // 淡出
    QPropertyAnimation *fadeOut = new QPropertyAnimation(opacityEffect, "opacity");
    fadeOut->setDuration(800);
    fadeOut->setStartValue(1);
    fadeOut->setEndValue(0);

    seq->addAnimation(fadeIn);
    seq->addPause(1000);
    seq->addAnimation(fadeOut);

    connect(seq, &QSequentialAnimationGroup::finished, this, [=](){
        logoLabel->hide();
        m_startSceneWidget->setStyleSheet("background: transparent;"); // 露出背景
        showLoadingUI();
    });

    seq->start(QAbstractAnimation::DeleteWhenStopped);
}

// --- 逻辑拆分：加载与菜单 ---
void MainWindow::showLoadingUI() {
    QVBoxLayout *layout = qobject_cast<QVBoxLayout*>(m_startSceneWidget->layout());

    // 游戏标题
    QLabel *title = new QLabel("骰 子 街", m_startSceneWidget);
    title->setStyleSheet("font-family: 'YouYuan'; font-size: 80px; color: white; margin-bottom: 50px;");
    layout->addWidget(title, 0, Qt::AlignCenter);

    // 进度条
    m_loadingBar = new QProgressBar(m_startSceneWidget);
    m_loadingBar->setFixedWidth(500);
    m_loadingBar->setFixedHeight(10);
    m_loadingBar->setTextVisible(false);
    m_loadingBar->setStyleSheet("QProgressBar { background: rgba(255,255,255,50); border-radius: 5px; } "
                                "QProgressBar::chunk { background: #00FFCC; border-radius: 5px; }");
    layout->addWidget(m_loadingBar, 0, Qt::AlignCenter);

    QTimer *timer = new QTimer(this);
    static int stage = 0; // 记录加载阶段

    connect(timer, &QTimer::timeout, this, [=](){
        static int progress = 0;
        progress += 5;
        m_loadingBar->setValue(progress);

        // --- 核心改动：在进度条跑的过程中执行耗时操作 ---
        if (progress == 20) {
            // 阶段 1：初始化游戏网格和基础组件
            QGridLayout *gameMainLayout = new QGridLayout(m_gameMainWidget);
            gameMainLayout->setContentsMargins(0, 0, 0, 0);
            gameMainLayout->setSpacing(0);
            for(int i = 0; i < 90; ++i) gameMainLayout->setRowStretch(i, 1);
            for(int i = 0; i < 160; ++i) gameMainLayout->setColumnStretch(i, 1);
        }

        if (progress == 50) {
            // 阶段 2：创建玩家区域、商店等（原本最耗时的 setupGameMainLayout）
            // 注意：现在 setupGameMainLayout 里的 layout 要用上面创建的那个
            setupGameMainLayout(static_cast<QGridLayout*>(m_gameMainWidget->layout()), m_state->getPlayers());
        }

        if (progress == 80) {
            // 阶段 3：连接其余信号、初始化遮罩等
            m_waitCurtain = new QWidget(m_animationOverlayWidget);
            m_waitLabel = new QLabel(m_waitCurtain);
            m_waitCurtain->hide();
        }

        if(progress >= 100) {
            timer->stop();
            m_loadingBar->hide();
            // 显示按钮
            QPushButton *btnStart = new QPushButton("开始游戏", m_startSceneWidget);
            btnStart->setFixedSize(200, 60);
            btnStart->setStyleSheet("QPushButton { background: #2ecc71; color: white; font-size: 24px; border-radius: 10px; }"
                                    "QPushButton:hover { background: #27ae60; }");
            layout->addWidget(btnStart, 0, Qt::AlignCenter);

            connect(btnStart, &QPushButton::clicked, this, &MainWindow::enterGame);

        }
    });
    timer->start(30);


}

// --- 逻辑拆分：进入游戏 ---
void MainWindow::enterGame() {
    // 1. 视觉上的切换
    m_startSceneWidget->hide();   // 隐藏黑色启动层
    m_gameMainWidget->show();    // 显示游戏网格

    // 2. 逻辑上的触发
    qDebug() << "用户点击了开始游戏按钮";
    emit gameStarted(); // 触发信号，main.cpp 里的 Lambda 会捕捉到并调用 initializeGame()
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

    // 玩家地标区域
    PlayerAreaWidget* playerLandmarkArea = new PlayerAreaWidget(player, config.landmarkAreaIsHorizontal, true, m_gameMainWidget, config.landmarkAreaIsSpecialPlayer0);
    layout->addWidget(playerLandmarkArea, config.landmarkAreaRow, config.landmarkAreaCol, config.landmarkAreaRowSpan, config.landmarkAreaColSpan);
    m_playerToLandmarkAreaMap.insert(player, playerLandmarkArea);
    connect(playerLandmarkArea, &PlayerAreaWidget::cardWidgetRequestShowDetailed, this, &MainWindow::showDetailedCard);
    connect(playerLandmarkArea, &PlayerAreaWidget::cardWidgetRequestHideDetailed, this, &MainWindow::hideDetailedCard);

    // MODIFIED: 金币变化显示 Widget
    // 传入 m_gameMainWidget 作为第二个参数
    CoinChangeWidget* coinChangeWidget = new CoinChangeWidget(player, m_gameMainWidget, m_animationOverlayWidget); // Parent to m_animationOverlayWidget
    coinChangeWidget->hide(); // Initially hidden

    // Store the widget
    m_playerToCoinChangeWidgetMap.insert(player, coinChangeWidget);

    // Connect the signal
    QObject::connect(player, &Player::coinsChange, coinChangeWidget, &CoinChangeWidget::showChange);

    // ADDED: Set the positioning lambda for the CoinChangeWidget
    coinChangeWidget->setCoinChangePosFunction(config.getDisplayPos);


}

CardStore* MainWindow::findCardStoreForCard(Card* card, int& posInStore) {
    QList<CardStore*> stores = m_state->getCardStores();
    for (CardStore* store : stores) {
        for (int i = 0; i < store->getSlots().size(); ++i) {
            if (store->getSlots().at(i).contains(card)) {
                posInStore = i;
                return store;
            }
        }
    }
    posInStore = -1;
    return nullptr;
}
void MainWindow::showWaitCurtain(QString waitMessage){
    if (!m_waitCurtain || !m_waitLabel) return;
    QSize overlaySize = m_animationOverlayWidget->size();
    int overlayWidth = overlaySize.width();
    int overlayHeight = overlaySize.height();

    // 几何设置：宽100%，高15%，距顶部15%
    int curtainY = static_cast<int>(overlayHeight * 0.17);
    int curtainHeight = static_cast<int>(overlayHeight * 0.08);

    // --- 创建半透明黑幕 ---
    m_waitCurtain->setStyleSheet("background-color: rgba(0, 0, 0, 150);");
    m_waitCurtain->setAttribute(Qt::WA_TransparentForMouseEvents);
    m_waitCurtain->setGeometry(0, curtainY, overlayWidth, curtainHeight);
    m_waitCurtain->show();

    // --- 生成带空格文字 ---
    QString spacedMessage;
    for (int i = 0; i < waitMessage.length(); ++i) {
        spacedMessage.append(waitMessage.at(i));
        if (i < waitMessage.length() - 1)
            spacedMessage.append(" ");
    }

    // --- 创建文字标签 ---
    m_waitLabel->setStyleSheet("color: white;");
    m_waitLabel->setAlignment(Qt::AlignCenter);
    m_waitLabel->setFont(QFont("YouYuan", overlayHeight / 30, QFont::Bold));
    m_waitLabel->setGeometry(0, 0, overlayWidth, curtainHeight);
    m_waitLabel->setText(spacedMessage);
    m_waitLabel->show();
}

void MainWindow::hideWaitCurtain(){
    if (m_waitCurtain) {
        m_waitCurtain->hide();
    }
    if (m_waitLabel) {
        m_waitLabel->hide();
    }
}
void MainWindow::onRequestUserInput(PromptData pd){

    //等待提示显示
    if (pd.waitMessage!=""&&pd.isAutoInput)
        showWaitCurtain(pd.waitMessage);
    else
        hideWaitCurtain();

    //分类执行后端响应
    switch(pd.type){
    case PromptData::None:
    case PromptData::SelectPlayer:{//因为selectPlayer只有【搬家公司】有用，所以暂时先不做。
        m_selectPlayerHandler->handle(pd);
        break;
    }
    case PromptData::SelectCard: {
        m_selectCardHandler->handle(pd);
        break;
    }
    case PromptData::Popup: { // 弹窗动画
        m_popupHandler->handle(pd);
        break;
    }
    case PromptData::DiceAnimation: { // DiceAnimation 动画
        m_diceAnimationHandler->handle(pd);
        break;
    }
    case PromptData::SelectDice: { // SelectDice 交互式选择
        m_selectDiceHandler->handle(pd);
        break;
    }
    case PromptData::StartTurnAnimation: {
        m_startTurnAnimationHandler->handle(pd);
        break;
    }
    case PromptData::CardInAnimation: {
        m_cardInAnimationHandler->handle(pd);
        break;
    }

    case PromptData::CardOutAnimation: {
        m_cardOutAnimationHandler->handle(pd);
        break;
    }
    case PromptData::BuyCardAnimation: {
        m_buyCardAnimationHandler->handle(pd);
        break;
    }
    }
}

// 显示详细卡牌的槽函数
void MainWindow::showDetailedCard(Card* card, QPoint globalPos)
{
    // 新增：如果交互式提示处于活动状态，则不显示详细卡牌。
    if (m_isInteractivePromptActive) {
        return;
    }

    // 如果已经有详细卡牌在显示，先隐藏它
    if (m_detailedCardWidget) {
        hideDetailedCard();
    }

    if (!card) return;

    // 重要的修改：不要在这里改变 m_animationOverlayWidget 的鼠标事件透明性。
    // 它应该保持透明，以便鼠标事件可以穿透到下面的游戏主界面或弹窗按钮。
    // m_animationOverlayWidget->setAttribute(Qt::WA_TransparentForMouseEvents, false); // 移除此行
    m_animationOverlayWidget->show(); // 确保覆盖层可见（如果它之前被隐藏了）

    // 创建详细卡牌
    m_detailedCardWidget = new CardWidget(card, ShowType::Detailed, m_animationOverlayWidget);
    m_detailedCardWidget->setAnimated(true); // 标记为动画状态，防止其自身处理 hover 样式

    // 新增：确保详细卡牌本身对鼠标事件是透明的，这样它就不会阻挡下面的元素。
    m_detailedCardWidget->setAttribute(Qt::WA_TransparentForMouseEvents, true);

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
    // 关键更改：将 m_fadeAnimation 的父对象设置为 shadowEffect，这样当 shadowEffect 被删除时，动画也会被删除
    m_fadeAnimation = new QPropertyAnimation(shadowEffect, "opacity", shadowEffect); // 动画目标是 shadowEffect，父对象也是 shadowEffect
    m_fadeAnimation->setDuration(200); // 渐变出现时间 200ms
    m_fadeAnimation->setStartValue(0.0);
    m_fadeAnimation->setEndValue(1.0);
    m_fadeAnimation->setEasingCurve(QEasingCurve::OutQuad);

    m_detailedCardWidget->show();
    // 由于 m_fadeAnimation 是 QPointer，在调用 start() 前进行安全检查
    if (m_fadeAnimation) {
        m_fadeAnimation->start();
    }
}

// 隐藏详细卡牌的槽函数
void MainWindow::hideDetailedCard()
{
    if (m_fadeAnimation) { // QPointer 检查
        m_fadeAnimation->stop();
    }

    // 删除详细卡牌 widget
    if (m_detailedCardWidget) {
        m_detailedCardWidget->deleteLater();
        m_detailedCardWidget = nullptr;
    }
}
