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
#include <QLabel>
#include <QPushButton> // For QPushButton
#include <QVBoxLayout> // For layouts
#include <QHBoxLayout>
#include <functional> // For std::function
#include <QGraphicsOpacityEffect> // For QGraphicsOpacityEffect
#include <QGraphicsColorizeEffect> // For QGraphicsColorizeEffect
#include <QPointer> // IMPORTANT: Include QPointer for safe object references
#include <QSharedPointer> // IMPORTANT: Include QSharedPointer for shared ownership

// 辅助类：用于处理按钮的鼠标进入和离开事件，以触发骰子动画
// IMPORTANT: 这个类现在将持有并管理骰子动画的状态变量
class DiceOptionEventFilter : public QObject {
public:
    // 构造函数现在接受 animatingDice 和 rollAnimationTimer 的 QPointer
    DiceOptionEventFilter(QObject* parent, QPointer<Dice> animatingDice, QPointer<QTimer> rollAnimationTimer)
        : QObject(parent),
        m_animatingDice(animatingDice),
        m_rollAnimationTimer(rollAnimationTimer),
        m_currentRollIndex1(0),
        m_currentRollIndex2(0),
        m_diceToAnimateCount(0)
    {
        // 连接计时器的 timeout 信号，用于更新骰子数字
        // lambda 捕获 'this'，并直接访问成员变量 (QPointer会自动处理空值)
        connect(m_rollAnimationTimer, &QTimer::timeout, this, [this]() {
            if (!m_animatingDice) return; // QPointer 检查

            const QList<int> rollSequence1 = {1, 3, 5, 2, 4, 6};
            const QList<int> rollSequence2 = {2, 4, 6, 1, 3, 5};

            if (m_diceToAnimateCount > 0) {
                m_animatingDice->setFirstNum(rollSequence1.at(m_currentRollIndex1));
                m_currentRollIndex1 = (m_currentRollIndex1 + 1) % rollSequence1.size();

                if (m_diceToAnimateCount == 2) {
                    m_animatingDice->setSecondNum(rollSequence2.at(m_currentRollIndex2));
                    m_currentRollIndex2 = (m_currentRollIndex2 + 1) % rollSequence2.size();
                } else {
                    m_animatingDice->setSecondNum(0);
                }
            }
        });
    }

    // eventFilter 是一个 const 方法，所以它调用的成员函数也必须是 const-callable
    bool eventFilter(QObject *obj, QEvent *event) override {
        QPushButton* button = qobject_cast<QPushButton*>(obj);
        // 仅处理 QPushButton 且按钮必须是启用的
        if (!button || !button->isEnabled()) {
            return QObject::eventFilter(obj, event);
        }

        if (event->type() == QEvent::Enter) {
            int optionId = button->property("optionId").toInt(); // 获取按钮存储的选项ID
            if (optionId == 1) { // 抛一个骰子
                startDiceAnimation(1);
            } else if (optionId == 2) { // 抛两个骰子
                startDiceAnimation(2);
            }
        } else if (event->type() == QEvent::Leave) {
            stopDiceAnimation(); // 停止动画并重置骰子
        }
        return QObject::eventFilter(obj, event);
    }

private:
    // 这些是 DiceOptionEventFilter 的私有成员函数，它们可以修改 DiceOptionEventFilter 的状态
    void startDiceAnimation(int numDice) {
        m_diceToAnimateCount = numDice;
        m_currentRollIndex1 = 0; // 重置索引
        m_currentRollIndex2 = 0; // 重置索引
        if (m_rollAnimationTimer) { // QPointer 检查
            m_rollAnimationTimer->start();
        }
    }

    void stopDiceAnimation() {
        if (m_rollAnimationTimer) { // QPointer 检查
            m_rollAnimationTimer->stop();
        }
        m_diceToAnimateCount = 0; // 重置计数
        if (m_animatingDice) { // QPointer 检查
            m_animatingDice->setFirstNum(1);
            m_animatingDice->setSecondNum(0);
        }
    }

    QPointer<Dice> m_animatingDice; // 更改为 QPointer
    QPointer<QTimer> m_rollAnimationTimer; // 更改为 QPointer
    int m_currentRollIndex1;
    int m_currentRollIndex2;
    int m_diceToAnimateCount;
};


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
    m_animationOverlayWidget->setAttribute(Qt::WA_TransparentForMouseEvents); // 默认设置为透明
    m_animationOverlayWidget->setStyleSheet("background: transparent;");

    // 保持这个顺序，确保 m_animationOverlayWidget 在 m_gameMainWidget 之上

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
        75, 20, 15, 120, true, true, // card area (horizontal)
        65, 45, 10, 70, true, true, // landmark area (horizontal, special player 0)
        [](int w, int h) { return QPoint(w / 2, h + 100); }
    };

    // 玩家 1 (左侧)
    playerConfigs[1] = {
        0, 0, 15, 15, // photo
        15, 0, 50, 12, false, true, // card area (vertical)
        15, 12, 50, 9, false, false, // landmark area (vertical)
        [](int w, int h) { return QPoint(-100, h / 2); }
    };

    // 玩家 2 (顶部左侧)
    playerConfigs[2] = {
        0, 70, 15, 15, // photo
        0, 20, 12, 50, true, true, // card area (horizontal)
        12, 20, 9, 50, true, false, // landmark area (horizontal)
        [](int w, int h) { return QPoint(w / 3, -100); }
    };

    // 玩家 3 (顶部右侧)
    playerConfigs[3] = {
        0, 140, 15, 15, // photo
        0, 90, 12, 50, true, true, // card area (horizontal)
        12, 90, 9, 50, true, false, // landmark area (horizontal)
        [](int w, int h) { return QPoint(w / 3 * 2, -100); }
    };

    // 玩家 4 (右侧)
    playerConfigs[4] = {
        70, 145, 15, 15, // photo
        20, 148, 50, 12, false, true, // card area (vertical)
        20, 139, 50, 9, false, false, // landmark area (vertical)
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
    m_mainDiceAreaWidget = diceAreaWidget;
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
    connect(playerCardArea, &PlayerAreaWidget::cardWidgetRequestHideDetailed, this, &MainWindow::hideDetailedCard); // 修正此行

    // 玩家地标区域 (如果存在)
    if (config.hasLandmarkArea) {
        PlayerAreaWidget* playerLandmarkArea = new PlayerAreaWidget(player, config.landmarkAreaIsHorizontal, true, m_gameMainWidget, config.landmarkAreaIsSpecialPlayer0);
        layout->addWidget(playerLandmarkArea, config.landmarkAreaRow, config.landmarkAreaCol, config.landmarkAreaRowSpan, config.landmarkAreaColSpan);
        m_playerToLandmarkAreaMap.insert(player, playerLandmarkArea);
        connect(playerLandmarkArea, &PlayerAreaWidget::cardWidgetRequestShowDetailed, this, &MainWindow::showDetailedCard);
        connect(playerLandmarkArea, &PlayerAreaWidget::cardWidgetRequestHideDetailed, this, &MainWindow::hideDetailedCard); // 修正此行
    }
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

void MainWindow::onRequestUserInput(PromptData pd){
    switch(pd.type){
    case PromptData::None:
    case PromptData::SelectPlayer:{
        int opId = pd.autoInput;
        QTimer::singleShot(500, this, [this, opId](){ // 显式捕获 'this'
            // --- 确保 m_animationOverlayWidget 是鼠标事件透明的 ---
            m_animationOverlayWidget->setAttribute(Qt::WA_TransparentForMouseEvents, true);
            emit responseUserInput(opId);
        });
        break;
    }
    case PromptData::SelectCard: {
        // 如果是自动选择，则延迟后直接响应
        if (pd.isAutoInput) {
            QTimer::singleShot(500, this, [this, opId = pd.autoInput]() {
                // SelectCard 模式下，m_animationOverlayWidget 始终保持透明，不用于事件阻挡
                emit responseUserInput(opId);
            });
            break;
        }

        QTimer::singleShot(0, this, [this, pd]() {
            if (!m_animationOverlayWidget) {
                qWarning() << "SelectCard (delayed): Animation overlay widget is null. Emitting autoInput.";
                emit responseUserInput(pd.autoInput);
                return;
            }

            // 获取主界面骰子区域的几何信息（相对于 m_gameMainWidget）
            QRect diceAreaRectInGameMain = m_mainDiceAreaWidget->geometry();
            // 将骰子区域的左上角坐标从 m_gameMainWidget 的局部坐标转换为全局屏幕坐标
            QPoint diceAreaTopLeftGlobal = m_gameMainWidget->mapToGlobal(diceAreaRectInGameMain.topLeft());
            // 将全局屏幕坐标转换为 m_animationOverlayWidget 的局部坐标
            QPoint diceAreaTopLeftInOverlay = m_animationOverlayWidget->mapFromGlobal(diceAreaTopLeftGlobal);

            // 创建一个容器用于放置“未找到卡牌”的按钮
            QWidget* buttonContainer = new QWidget(m_gameMainWidget);
            buttonContainer->setStyleSheet("background-color: rgba(0, 0, 0, 150); border-radius: 10px;");

            // 设置按钮容器的几何形状，使其与骰子区域对齐且大小相同
            buttonContainer->setGeometry(diceAreaRectInGameMain);

            // 获取实际的容器宽度和高度，用于内部按钮的尺寸计算
            int actualContainerWidth = diceAreaRectInGameMain.size().width();
            int actualContainerHeight = diceAreaRectInGameMain.size().height();

            QVBoxLayout* buttonLayout = new QVBoxLayout(buttonContainer);
            buttonLayout->setAlignment(Qt::AlignCenter);
            buttonLayout->setContentsMargins(10, 10, 10, 10); // 调整内边距，让按钮有更多空间
            buttonLayout->setSpacing(5); // 调整间距

            // IMPORTANT CHANGE: 使用 QSharedPointer 来管理这些 QMap 的生命周期
            QSharedPointer<QMap<QPointer<CardWidget>, QPointer<QGraphicsEffect>>> detachedOriginalEffects_ptr =
                QSharedPointer<QMap<QPointer<CardWidget>, QPointer<QGraphicsEffect>>>::create();
            QSharedPointer<QMap<QPointer<QGraphicsEffect>, QPointer<QPropertyAnimation>>> activeAnimations_ptr =
                QSharedPointer<QMap<QPointer<QGraphicsEffect>, QPointer<QPropertyAnimation>>>::create();

            // ****** 新增：用于存储 CardWidget::clicked 信号连接的列表 ******
            QSharedPointer<QList<QMetaObject::Connection>> activeCardClickConnections_ptr =
                QSharedPointer<QList<QMetaObject::Connection>>::create();
            // ***************************************************************

            // Lambda 函数：处理清理和响应用户输入
            // IMPORTANT: 捕获 QSharedPointer by value，这样它们会随着 lambda 副本的生命周期而存在
            auto finalCleanupAndRespond = [this, buttonContainer_ptr = QPointer<QWidget>(buttonContainer),
                                           detachedOriginalEffects_ptr, activeAnimations_ptr,
                                           activeCardClickConnections_ptr](int selectedId) mutable { // 捕获新增的 QSharedPointer

                // 通过 QSharedPointer 访问实际的 QMap
                QMap<QPointer<CardWidget>, QPointer<QGraphicsEffect>>& detachedOriginalEffects = *detachedOriginalEffects_ptr;
                QMap<QPointer<QGraphicsEffect>, QPointer<QPropertyAnimation>>& activeAnimations = *activeAnimations_ptr;
                QList<QMetaObject::Connection>& activeConnections = *activeCardClickConnections_ptr; // 获取连接列表的引用

                for (QPointer<QPropertyAnimation> anim_ptr : activeAnimations.values()) {
                    if (anim_ptr) {
                        anim_ptr->stop();
                        anim_ptr->deleteLater();
                    }
                }
                activeAnimations.clear();

                // 恢复所有被修改的 CardWidget 的原始状态
                for (QPointer<CardWidget> cw_ptr : detachedOriginalEffects.keys()) {
                    if (cw_ptr) {
                        QPointer<QGraphicsEffect> originalEffect_ptr = detachedOriginalEffects.value(cw_ptr);
                        if (originalEffect_ptr) {
                            // 重新将原始效果的父对象设置为 CardWidget，这将删除当前（临时）效果
                            originalEffect_ptr->setParent(cw_ptr);
                            cw_ptr->setGraphicsEffect(originalEffect_ptr);
                        } else {
                            // 如果没有原始效果，则清除当前（临时）效果
                            cw_ptr->setGraphicsEffect(nullptr);
                        }
                        cw_ptr->setStyleSheet(""); // 清除临时样式
                        cw_ptr->setEnabled(true); // 重新启用
                        cw_ptr->setCursor(Qt::ArrowCursor); // 重置光标
                        cw_ptr->setToolTip(""); // 清除工具提示

                        // 重新连接 CardWidget 的详细显示信号
                        QObject::connect(cw_ptr, &CardWidget::requestShowDetailedCard, this, &MainWindow::showDetailedCard);
                        QObject::connect(cw_ptr, &CardWidget::requestHideDetailedCard, this, &MainWindow::hideDetailedCard);
                    }
                }
                detachedOriginalEffects.clear(); // 清空 map，QPointer 会自动处理空值

                // ****** 新增：断开所有存储的 CardWidget::clicked 信号连接 ******
                for (const QMetaObject::Connection& connection : activeConnections) {
                    QObject::disconnect(connection);
                }
                activeConnections.clear(); // 清空列表
                // ***************************************************************

                // 删除按钮容器及其子对象
                if (buttonContainer_ptr) {
                    buttonContainer_ptr->deleteLater();
                }

                if (m_animationOverlayWidget) {
                    m_animationOverlayWidget->setAttribute(Qt::WA_TransparentForMouseEvents, true); // 恢复透明
                }

                emit responseUserInput(selectedId);
            };

            // 收集所有 SlotWidget
            QList<QPointer<SlotWidget>> allSlotWidgets;
            for (QList<SlotWidget*> storeSlots : m_cardStoreArea->m_storeToSlotsMap.values()) {
                for (SlotWidget* slot : storeSlots) {
                    allSlotWidgets.append(slot);
                }
            }
            for (PlayerAreaWidget* playerArea : m_playerToCardAreaMap.values()) {
                for (SlotWidget* slot : playerArea->getSlots()) { // 使用新添加的 getSlots() 方法
                    allSlotWidgets.append(slot);
                }
            }
            for (PlayerAreaWidget* playerArea : m_playerToLandmarkAreaMap.values()) {
                for (SlotWidget* slot : playerArea->getSlots()) { // 使用新添加的 getSlots() 方法
                    allSlotWidgets.append(slot);
                }
            }
            qDebug() << "  Collected" << allSlotWidgets.size() << "slot widgets.";

            // 处理所有选项
            for (const OptionData& option : pd.options) {
                qDebug() << "Processing option:" << option.name << ", ID:" << option.id << ", State:" << option.state;
                CardWidget* foundCardWidget = nullptr;
                for (QPointer<SlotWidget> slot_ptr : allSlotWidgets) {
                    if (slot_ptr && slot_ptr->topCard()) {
                        Card* cardInSlot = slot_ptr->topCard()->getCard();
                        if (cardInSlot && cardInSlot->getId() == option.id) {
                            foundCardWidget = slot_ptr->topCard();
                            qDebug() << "    Found CardWidget for option ID" << option.id << ":" << foundCardWidget->getCard()->getName();
                            break;
                        }
                    }
                }

                if (foundCardWidget) {
                    // IMPORTANT: 在设置新效果之前，先分离并存储原始效果
                    QGraphicsEffect* originalEffect = foundCardWidget->graphicsEffect();
                    if (originalEffect) {
                        originalEffect->setParent(nullptr); // 解除 CardWidget 对其的所有权
                        detachedOriginalEffects_ptr->insert(foundCardWidget, originalEffect); // 存储到 QSharedPointer 管理的 map 中
                    } else {
                        detachedOriginalEffects_ptr->insert(foundCardWidget, nullptr); // 存储到 QSharedPointer 管理的 map 中
                    }

                    // 断开 CardWidget 的详细显示信号，防止在选择时弹出详细卡牌
                    QObject::disconnect(foundCardWidget, &CardWidget::requestShowDetailedCard, this, &MainWindow::showDetailedCard);
                    QObject::disconnect(foundCardWidget, &CardWidget::requestHideDetailedCard, this, &MainWindow::hideDetailedCard);
                    qDebug() << "    Disconnected CardWidget detailed display signals.";

                    // 根据状态应用视觉效果和连接信号
                    if (option.state == 0) { // 暗色，不可点击，提示信息
                        qDebug() << "    Applying darken effect (State 0).";
                        QGraphicsColorizeEffect* darkenEffect = new QGraphicsColorizeEffect(foundCardWidget); // 新效果，父对象为 foundCardWidget
                        darkenEffect->setColor(QColor(0, 0, 0)); // 变暗为黑色
                        darkenEffect->setStrength(0.3); // 30% 强度
                        foundCardWidget->setGraphicsEffect(darkenEffect); // CardWidget 接管 darkenEffect 的所有权
                        foundCardWidget->setEnabled(false);
                        foundCardWidget->setCursor(Qt::ForbiddenCursor);
                        foundCardWidget->setToolTip(option.unClickMessage);
                        qDebug() << "    CardWidget disabled and tooltip set.";
                    } else if (option.state == 1) { // 闪烁，可点击
                        qDebug() << "    Applying opacity effect and flash animation (State 1).";
                        QGraphicsOpacityEffect* opacityEffect = new QGraphicsOpacityEffect(foundCardWidget); // 新效果，父对象为 foundCardWidget
                        foundCardWidget->setGraphicsEffect(opacityEffect); // CardWidget 接管 opacityEffect 的所有权

                        QPropertyAnimation* flashAnim = new QPropertyAnimation(opacityEffect, "opacity", opacityEffect); // Effect 接管动画所有权
                        flashAnim->setDuration(800);
                        flashAnim->setStartValue(0.5);
                        flashAnim->setEndValue(1.0);
                        flashAnim->setEasingCurve(QEasingCurve::InOutQuad);
                        flashAnim->setLoopCount(-1); // 无限循环
                        flashAnim->start(QAbstractAnimation::DeleteWhenStopped);
                        activeAnimations_ptr->insert(opacityEffect, flashAnim); // 存储到 QSharedPointer 管理的 map

                        foundCardWidget->setEnabled(true);
                        foundCardWidget->setCursor(Qt::PointingHandCursor);
                        // ****** 修改：存储连接以便后续断开 ******
                        QMetaObject::Connection connection = QObject::connect(foundCardWidget, &CardWidget::clicked, this, [finalCleanupAndRespond, option]() mutable { // 添加 mutable
                            finalCleanupAndRespond(option.id);
                        });
                        activeCardClickConnections_ptr->append(connection); // 添加到列表中
                        // ****************************************
                    } else if (option.state == 2) { // 高亮，可点击
                        QGraphicsDropShadowEffect* highlightEffect = new QGraphicsDropShadowEffect(foundCardWidget); // 新效果，父对象为 foundCardWidget
                        highlightEffect->setColor(QColor(255, 255, 0)); // 黄色高亮
                        highlightEffect->setBlurRadius(20);
                        highlightEffect->setOffset(0, 0);
                        foundCardWidget->setGraphicsEffect(highlightEffect); // CardWidget 接管 highlightEffect 的所有权
                        foundCardWidget->setEnabled(true);
                        foundCardWidget->setCursor(Qt::PointingHandCursor);
                        // ****** 修改：存储连接以便后续断开 ******
                        QMetaObject::Connection connection = QObject::connect(foundCardWidget, &CardWidget::clicked, this, [finalCleanupAndRespond, option]() mutable { // 添加 mutable
                            finalCleanupAndRespond(option.id);
                        });
                        activeCardClickConnections_ptr->append(connection); // 添加到列表中
                        // ****************************************
                    }
                } else {
                    QPushButton* button = new QPushButton(option.name, buttonContainer);
                    // selectCard中目前只有一个按钮，让它占据大部分空间
                    button->setFixedSize(static_cast<int>(actualContainerWidth * 0.5), static_cast<int>(actualContainerHeight * 0.5));
                    QFont buttonFont = button->font();
                    buttonFont.setPointSize(actualContainerHeight / 5); // 字体大小根据容器高度调整
                    button->setFont(buttonFont);
                    button->setProperty("optionId", option.id);
                    buttonLayout->addWidget(button);

                    button->setEnabled(true);
                    button->setStyleSheet("QPushButton { background-color: #4CAF50; color: white; border: 1px solid #388E3C; border-radius: 5px; }"
                                          "QPushButton:hover { background-color: #66BB6A; }"
                                          "QPushButton:pressed { background-color: #388E3C; }");
                    button->setCursor(Qt::PointingHandCursor);
                    QObject::connect(button, &QPushButton::clicked, this, [finalCleanupAndRespond, option]() mutable { // 添加 mutable
                        finalCleanupAndRespond(option.id);
                    });
                }
            }

            // 如果有任何按钮被添加，则渐入显示按钮容器
            if (buttonLayout->count() > 0) { // 检查是否有实际的按钮被添加到布局中
                qDebug() << "  Starting buttonContainer fade-in animation.";

                // 为 buttonContainer 创建并应用 QGraphicsOpacityEffect
                QGraphicsOpacityEffect* buttonContainerOpacityEffect = new QGraphicsOpacityEffect(buttonContainer);
                buttonContainer->setGraphicsEffect(buttonContainerOpacityEffect);
                buttonContainerOpacityEffect->setOpacity(0.0); // 初始设置为完全透明

                // 动画 QGraphicsOpacityEffect 的 opacity 属性
                QPropertyAnimation* initialFadeInAnim = new QPropertyAnimation(buttonContainerOpacityEffect, "opacity", buttonContainerOpacityEffect);
                initialFadeInAnim->setDuration(300);
                initialFadeInAnim->setStartValue(0.0);
                initialFadeInAnim->setEndValue(1.0);
                initialFadeInAnim->setEasingCurve(QEasingCurve::OutQuad);
                initialFadeInAnim->start(QAbstractAnimation::DeleteWhenStopped);

                buttonContainer->show();
                buttonContainer->raise(); // 确保按钮容器在 m_gameMainWidget 的所有其他子部件之上
            } else {
                buttonContainer->deleteLater(); // 没有按钮，直接删除容器
            }
        });
        break;
    }
    case PromptData::Popup: { // 弹窗动画
        int opId = pd.autoInput;
        QString promptMessage = pd.promptMessage;
        QList<OptionData> options = pd.options;
        int delay = pd.delay;

        if(pd.isAutoInput){
            QTimer::singleShot(500, this, [this, opId](){ // 显式捕获 'this'
                // --- 确保 m_animationOverlayWidget 是鼠标事件透明的 ---
                m_animationOverlayWidget->setAttribute(Qt::WA_TransparentForMouseEvents, true);
                emit responseUserInput(opId);
            });
            break;
        }

        QTimer::singleShot(0, this, [this, opId, promptMessage, options, delay]() {
            if (!m_animationOverlayWidget) {
                qWarning() << "Popup (delayed): Animation overlay widget is null.";
                emit responseUserInput(opId);
                return;
            }

            // 新增：设置交互式提示为活动状态
            m_isInteractivePromptActive = true;
            // 禁用 m_animationOverlayWidget 的鼠标事件透明性，使其捕获事件
            m_animationOverlayWidget->setAttribute(Qt::WA_TransparentForMouseEvents, false);
            m_animationOverlayWidget->show(); // 确保覆盖层可见

            // 1. 创建一个容器 QWidget 用于承载整个弹窗UI
            QWidget* popupContainer = new QWidget(m_animationOverlayWidget);
            popupContainer->setStyleSheet("background-color: rgba(0, 0, 0, 180); border-radius: 10px;");
            popupContainer->setWindowOpacity(0.0); // 初始透明度为0，用于渐入动画

            QSize overlaySize = m_animationOverlayWidget->size();
            int containerWidth = static_cast<int>(overlaySize.width() * 0.4);
            int containerHeight = static_cast<int>(overlaySize.height() * 0.3);
            QRect containerRect(QPoint((overlaySize.width() - containerWidth) / 2, (overlaySize.height() - containerHeight) / 2),
                                QSize(containerWidth, containerHeight));
            popupContainer->setGeometry(containerRect);

            QVBoxLayout* mainLayout = new QVBoxLayout(popupContainer);
            mainLayout->setAlignment(Qt::AlignCenter);
            mainLayout->setContentsMargins(20, 20, 20, 20);
            mainLayout->setSpacing(20);

            // 2. 添加提示消息标签
            QLabel* messageLabel = new QLabel(promptMessage, popupContainer);
            messageLabel->setStyleSheet("color: white; font-size: 20px; font-weight: bold;");
            messageLabel->setAlignment(Qt::AlignCenter);
            messageLabel->setWordWrap(true); // 允许文本换行
            mainLayout->addWidget(messageLabel);

            // 3. 创建选项按钮
            QHBoxLayout* optionsLayout = new QHBoxLayout();
            optionsLayout->setAlignment(Qt::AlignCenter);
            mainLayout->addLayout(optionsLayout);

            // Lambda 函数：处理清理和响应用户输入（包含渐出动画）
            auto finalCleanupAndRespond = [this, popupContainer_ptr = QPointer<QWidget>(popupContainer)](int selectedId) {
                // 如果容器已经被删除（例如，由于某种竞态条件），直接发出信号
                if (!popupContainer_ptr) { // QPointer 检查
                    // 恢复 overlay 的鼠标事件透明性，并设置交互式提示为非活动状态
                    if (m_animationOverlayWidget) {
                        m_animationOverlayWidget->setAttribute(Qt::WA_TransparentForMouseEvents, true);
                    }
                    m_isInteractivePromptActive = false; // 在动画完成后重置标志
                    emit responseUserInput(selectedId); // 使用传入的 selectedId
                    return;
                }

                // 启动容器的渐出动画
                QPropertyAnimation* fadeOutAnim = new QPropertyAnimation(popupContainer_ptr, "windowOpacity", popupContainer_ptr);
                fadeOutAnim->setDuration(300);
                fadeOutAnim->setStartValue(popupContainer_ptr->windowOpacity());
                fadeOutAnim->setEndValue(0.0);
                fadeOutAnim->setEasingCurve(QEasingCurve::InQuad);

                // 渐出动画完成后，删除容器并发出响应
                connect(fadeOutAnim, &QPropertyAnimation::finished, popupContainer_ptr, [popupContainer_ptr, selectedId, this]() {
                    if (popupContainer_ptr) { // 最终检查，确保没有在删除后再次访问
                        popupContainer_ptr->deleteLater(); // 删除容器及其所有子对象 (包括 DiceAreaWidget, Dice, QTimer, messageLabel, buttons, diceEventFilter)
                    }
                    // 新增：恢复 overlay 的鼠标事件透明性，并设置交互式提示为非活动状态
                    if (m_animationOverlayWidget) {
                        m_animationOverlayWidget->setAttribute(Qt::WA_TransparentForMouseEvents, true);
                    }
                    m_isInteractivePromptActive = false; // 在动画完成后重置标志
                    emit responseUserInput(selectedId); // 使用传入的 selectedId
                });
                fadeOutAnim->start(QAbstractAnimation::DeleteWhenStopped);
            };

            for (const OptionData& option : options) {
                QPushButton* button = new QPushButton(option.name, popupContainer);
                button->setFixedSize(static_cast<int>(containerWidth * 0.3), static_cast<int>(containerHeight * 0.2));
                QFont buttonFont = button->font();
                buttonFont.setPointSize(14);
                button->setFont(buttonFont);
                button->setCursor(Qt::PointingHandCursor);
                button->setProperty("optionId", option.id); // 将选项ID存储为属性

                if (option.state == 0) { // 选项不可点击
                    button->setEnabled(false);
                    button->setStyleSheet("QPushButton { background-color: #555; color: #aaa; border: 1px solid #777; border-radius: 5px; }"
                                          "QPushButton:disabled { background-color: #555; color: #aaa; }");
                } else { // 选项可点击
                    button->setStyleSheet("QPushButton { background-color: #4CAF50; color: white; border: 1px solid #388E3C; border-radius: 5px; }"
                                          "QPushButton:hover { background-color: #66BB6A; }"
                                          "QPushButton:pressed { background-color: #388E3C; }");
                    // 连接点击信号到清理和响应函数
                    connect(button, &QPushButton::clicked, this, [finalCleanupAndRespond, option]() {
                        finalCleanupAndRespond(option.id);
                    });
                }
                optionsLayout->addWidget(button);
            }

            // 4. 设置容器的渐入动画并显示
            QPropertyAnimation* initialFadeInAnim = new QPropertyAnimation(popupContainer, "windowOpacity", popupContainer);
            initialFadeInAnim->setDuration(300);
            initialFadeInAnim->setStartValue(0.0);
            initialFadeInAnim->setEndValue(1.0);
            initialFadeInAnim->setEasingCurve(QEasingCurve::OutQuad);
            initialFadeInAnim->start(QAbstractAnimation::DeleteWhenStopped); // 启动渐入动画，完成后自动删除
            popupContainer->show(); // 显示容器，透明度将从0开始动画
        });
        break;
    }
    case PromptData::DiceAnimation: { // DiceAnimation 动画
        QList<int> diceNumbers = pd.diceNum; // 目标骰子点数
        int opId = pd.autoInput; // 操作ID

        QTimer::singleShot(0, this, [this, diceNumbers, opId]() { // 显式捕获 'this'
            // --- 确保 m_animationOverlayWidget 是鼠标事件透明的 ---
            m_animationOverlayWidget->setAttribute(Qt::WA_TransparentForMouseEvents, true);

            if (diceNumbers.isEmpty() || !m_animationOverlayWidget) {
                qWarning() << "DiceAnimation (delayed): Invalid data or widgets.";
                emit responseUserInput(opId);
                return;
            }

            // 1. 创建临时的 Dice 对象和 DiceAreaWidget
            Dice* animatingDice = new Dice(nullptr); // 先不设置父对象
            animatingDice->setFirstNum(0);
            animatingDice->setSecondNum(0);

            DiceAreaWidget* animatingDiceAreaWidget = new DiceAreaWidget(animatingDice, m_animationOverlayWidget);
            animatingDice->setParent(animatingDiceAreaWidget); // 将Dice设置为DiceAreaWidget的子对象

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
            const int rollDuration = 1200;
            const int rollInterval = 70;

            QTimer* rollTimer = new QTimer(animatingDiceAreaWidget); // 以 DiceAreaWidget 为父对象
            QPointer<Dice> animatingDice_ptr = animatingDice; // IMPORTANT: 使用 QPointer
            QObject::connect(rollTimer, &QTimer::timeout, this, [animatingDice_ptr, diceNumbers]() {
                if (!animatingDice_ptr) return; // QPointer 检查

                // 生成随机数 (1到6)
                animatingDice_ptr->setFirstNum(QRandomGenerator::global()->bounded(1, 7));
                if (diceNumbers.size() > 1) { // 如果需要显示两个骰子
                    animatingDice_ptr->setSecondNum(QRandomGenerator::global()->bounded(1, 7));
                } else {
                    animatingDice_ptr->setSecondNum(0); // 隐藏第二个骰子
                }
            });

            QPauseAnimation* rollingPause = new QPauseAnimation(rollDuration);
            sequentialGroup->addAnimation(rollingPause);

            // FIX for Error 1: 使用 sequentialGroup 的 currentAnimationChanged 信号来控制 rollTimer 的启停
            QObject::connect(sequentialGroup, &QSequentialAnimationGroup::currentAnimationChanged, this,
                             [rollTimer, rollInterval, rollingPause](QAbstractAnimation* current) {
                                 if (current == rollingPause) {
                                     if (rollTimer) rollTimer->start(rollInterval); // QPointer 检查
                                 } else {
                                     if (rollTimer) rollTimer->stop(); // QPointer 检查
                                 }
                             });

            // 当 rollingPause 动画结束时，设置最终骰子点数并显示总和标签
            QObject::connect(rollingPause, &QPauseAnimation::finished, this,
                             [animatingDice_ptr, diceNumbers]() { // IMPORTANT: 使用 QPointer
                                 if (!animatingDice_ptr) return; // QPointer 检查

                                 // 设置最终骰子点数
                                 animatingDice_ptr->setFirstNum(diceNumbers.at(0));
                                 if (diceNumbers.size() > 1) {
                                     animatingDice_ptr->setSecondNum(diceNumbers.at(1));
                                 } else {
                                     animatingDice_ptr->setSecondNum(0);
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
            QObject::connect(sequentialGroup, &QSequentialAnimationGroup::finished, this,
                             [this, animatingDiceAreaWidget, opId]() {
                                 animatingDiceAreaWidget->deleteLater(); // 删除 DiceAreaWidget 及其所有子对象 (包括 Dice 和 QTimer)
                                 emit responseUserInput(opId); // 发出用户输入响应
                             });

            sequentialGroup->start(QAbstractAnimation::DeleteWhenStopped); // 启动动画组，并在完成后自动删除
        });

        break;
    }
    case PromptData::SelectDice: { // SelectDice 交互式选择
        int opId = pd.autoInput;
        QString promptMessage = pd.promptMessage;
        QList<OptionData> options = pd.options;
        int delay = pd.delay;

        if(pd.isAutoInput){
            QTimer::singleShot(500, this, [this, opId](){ // 显式捕获 'this'
                // --- 确保 m_animationOverlayWidget 是鼠标事件透明的 ---
                m_animationOverlayWidget->setAttribute(Qt::WA_TransparentForMouseEvents, true);
                emit responseUserInput(opId);
            });
            break;
        }

        QTimer::singleShot(0, this, [this, opId, promptMessage, options, delay]() {
            if (!m_animationOverlayWidget) {
                qWarning() << "SelectDice (delayed): Animation overlay widget is null.";
                emit responseUserInput(opId);
                return;
            }

            // 新增：设置交互式提示为活动状态
            m_isInteractivePromptActive = true;
            // 禁用 m_animationOverlayWidget 的鼠标事件透明性，使其捕获事件
            m_animationOverlayWidget->setAttribute(Qt::WA_TransparentForMouseEvents, false);
            m_animationOverlayWidget->show(); // 确保覆盖层可见

            // 1. 创建一个容器 QWidget 用于承载整个选择骰子UI
            QWidget* selectDiceContainer = new QWidget(m_animationOverlayWidget);
            selectDiceContainer->setStyleSheet("background-color: rgba(0, 0, 0, 180); border-radius: 10px;");
            selectDiceContainer->setWindowOpacity(0.0); // 初始透明度为0，用于渐入动画

            QSize overlaySize = m_animationOverlayWidget->size();
            int containerWidth = static_cast<int>(overlaySize.width() * 0.6);
            int containerHeight = static_cast<int>(overlaySize.height() * 0.5);
            QRect containerRect(QPoint((overlaySize.width() - containerWidth) / 2, (overlaySize.height() - containerHeight) / 2),
                                QSize(containerWidth, containerHeight));
            selectDiceContainer->setGeometry(containerRect);

            QVBoxLayout* mainLayout = new QVBoxLayout(selectDiceContainer);
            mainLayout->setAlignment(Qt::AlignCenter);
            mainLayout->setContentsMargins(20, 20, 20, 20);
            mainLayout->setSpacing(20);

            // 2. 添加提示消息标签
            QLabel* messageLabel = new QLabel(promptMessage, selectDiceContainer);
            messageLabel->setStyleSheet("color: white; font-size: 24px; font-weight: bold;");
            messageLabel->setAlignment(Qt::AlignCenter);
            mainLayout->addWidget(messageLabel);

            // 3. 创建用于动画的 Dice 对象和 DiceAreaWidget
            Dice* animatingDice = new Dice(nullptr); // 先不设置父对象
            animatingDice->setFirstNum(1); // 初始状态：一个骰子，数字1
            animatingDice->setSecondNum(0); // 第二个骰子隐藏

            DiceAreaWidget* animatingDiceAreaWidget = new DiceAreaWidget(animatingDice, selectDiceContainer); // DiceAreaWidget是container的子对象
            animatingDice->setParent(animatingDiceAreaWidget); // 将Dice设置为DiceAreaWidget的子对象

            animatingDiceAreaWidget->setFixedSize(static_cast<int>(containerWidth * 0.4), static_cast<int>(containerHeight * 0.4));
            mainLayout->addWidget(animatingDiceAreaWidget, 0, Qt::AlignCenter);

            // 为骰子区域应用阴影效果
            QGraphicsDropShadowEffect *shadowEffect = new QGraphicsDropShadowEffect(animatingDiceAreaWidget);
            shadowEffect->setBlurRadius(25);
            shadowEffect->setColor(QColor(0, 0, 0, 150));
            shadowEffect->setOffset(6, 6);
            animatingDiceAreaWidget->setGraphicsEffect(shadowEffect);
            animatingDiceAreaWidget->show();

            // 4. 创建并配置 QTimer
            QTimer* rollAnimationTimer = new QTimer(selectDiceContainer); // 以 selectDiceContainer 为父对象
            rollAnimationTimer->setInterval(70); // 10帧/秒

            // 5. 创建用于按钮悬停事件的事件过滤器实例
            // IMPORTANT: DiceOptionEventFilter 的构造函数现在接受 QPointer<Dice> 和 QPointer<QTimer>
            DiceOptionEventFilter* diceEventFilter = new DiceOptionEventFilter(selectDiceContainer,
                                                                               QPointer<Dice>(animatingDice), // 传递 QPointer
                                                                               QPointer<QTimer>(rollAnimationTimer)); // 传递 QPointer


            // 6. 创建选项按钮
            QHBoxLayout* optionsLayout = new QHBoxLayout();
            optionsLayout->setAlignment(Qt::AlignCenter);
            mainLayout->addLayout(optionsLayout);

            // Lambda 函数：处理清理和响应用户输入（包含渐出动画）
            // IMPORTANT: finalCleanupAndRespond 现在接受一个 int selectedId 参数，并使用 QPointer 捕获
            auto finalCleanupAndRespond = [this, selectDiceContainer_ptr = QPointer<QWidget>(selectDiceContainer), rollAnimationTimer_ptr = QPointer<QTimer>(rollAnimationTimer)](int selectedId) {
                // 停止定时器以防止任何进一步的超时事件
                if (rollAnimationTimer_ptr) { // IMPORTANT: QPointer 检查
                    rollAnimationTimer_ptr->stop(); // 这现在是安全的
                }

                // 如果容器已经被删除（例如，由于某种竞态条件），直接发出信号
                if (!selectDiceContainer_ptr) { // IMPORTANT: QPointer 检查
                    // 恢复 overlay 的鼠标事件透明性，并设置交互式提示为非活动状态
                    if (m_animationOverlayWidget) {
                        m_animationOverlayWidget->setAttribute(Qt::WA_TransparentForMouseEvents, true);
                    }
                    m_isInteractivePromptActive = false; // 在动画完成后重置标志
                    emit responseUserInput(selectedId); // 使用传入的 selectedId
                    return;
                }

                // 启动容器的渐出动画
                QPropertyAnimation* fadeOutAnim = new QPropertyAnimation(selectDiceContainer_ptr, "windowOpacity", selectDiceContainer_ptr);
                fadeOutAnim->setDuration(300);
                fadeOutAnim->setStartValue(selectDiceContainer_ptr->windowOpacity());
                fadeOutAnim->setEndValue(0.0);
                fadeOutAnim->setEasingCurve(QEasingCurve::InQuad);

                // 渐出动画完成后，删除容器并发出响应
                connect(fadeOutAnim, &QPropertyAnimation::finished, selectDiceContainer_ptr, [selectDiceContainer_ptr, selectedId, this]() {
                    if (selectDiceContainer_ptr) { // IMPORTANT: 最终检查，确保没有在删除后再次访问
                        selectDiceContainer_ptr->deleteLater(); // 删除容器及其所有子对象 (包括 DiceAreaWidget, Dice, QTimer, messageLabel, buttons, diceEventFilter)
                    }
                    // 新增：恢复 overlay 的鼠标事件透明性，并设置交互式提示为非活动状态
                    if (m_animationOverlayWidget) {
                        m_animationOverlayWidget->setAttribute(Qt::WA_TransparentForMouseEvents, true);
                    }
                    m_isInteractivePromptActive = false; // 在动画完成后重置标志
                    emit responseUserInput(selectedId); // 使用传入的 selectedId
                });
                fadeOutAnim->start(QAbstractAnimation::DeleteWhenStopped);
            };


            for (const OptionData& option : options) {
                QPushButton* button = new QPushButton(option.name, selectDiceContainer);
                button->setFixedSize(static_cast<int>(containerWidth * 0.3), static_cast<int>(containerHeight * 0.15));
                QFont buttonFont = button->font();
                buttonFont.setPointSize(16);
                button->setFont(buttonFont);
                button->setCursor(Qt::PointingHandCursor);
                button->setProperty("optionId", option.id); // 将选项ID存储为属性，供事件过滤器使用

                if (option.state == 0) { // 选项不可点击
                    button->setEnabled(false);
                    button->setStyleSheet("QPushButton { background-color: #555; color: #aaa; border: 1px solid #777; border-radius: 5px; }"
                                          "QPushButton:disabled { background-color: #555; color: #aaa; }");
                } else { // 选项可点击
                    button->setStyleSheet("QPushButton { background-color: #4CAF50; color: white; border: 1px solid #388E3C; border-radius: 5px; }"
                                          "QPushButton:hover { background-color: #66BB6A; }"
                                          "QPushButton:pressed { background-color: #388E3C; }");
                    // 连接点击信号到清理和响应函数
                    // IMPORTANT: 调用 finalCleanupAndRespond 时传入 option.id
                    connect(button, &QPushButton::clicked, this, [finalCleanupAndRespond, option]() {
                        finalCleanupAndRespond(option.id);
                    });

                    // 为按钮安装事件过滤器，处理悬停动画
                    button->installEventFilter(diceEventFilter);
                }
                optionsLayout->addWidget(button);
            }

            // 7. 设置容器的渐入动画并显示
            QPropertyAnimation* initialFadeInAnim = new QPropertyAnimation(selectDiceContainer, "windowOpacity", selectDiceContainer);
            initialFadeInAnim->setDuration(300);
            initialFadeInAnim->setStartValue(0.0);
            initialFadeInAnim->setEndValue(1.0);
            initialFadeInAnim->setEasingCurve(QEasingCurve::OutQuad);
            initialFadeInAnim->start(QAbstractAnimation::DeleteWhenStopped); // 启动渐入动画，完成后自动删除
            selectDiceContainer->show(); // 显示容器，透明度将从0开始动画
        });
        break;
    }
    case PromptData::StartTurnAnimation: {
        int opId = pd.autoInput;
        QString message = pd.promptMessage;

        QTimer::singleShot(0, this, [this, opId, message]() { // 显式捕获 'this'
            // --- 确保 m_animationOverlayWidget 是鼠标事件透明的 ---
            m_animationOverlayWidget->setAttribute(Qt::WA_TransparentForMouseEvents, true);

            if (!m_animationOverlayWidget) {
                qWarning() << "StartTurnAnimation (delayed): Animation overlay widget is null.";
                emit responseUserInput(opId);
                return;
            }

            QSize overlaySize = m_animationOverlayWidget->size();
            // 修正：黑幕宽度为窗口的完整宽度，高度从 0 扩展到窗口高度的 20%
            int curtainWidth = overlaySize.width();
            int targetCurtainHeight = static_cast<int>(overlaySize.height() * 0.15);

            // 计算垂直居中位置
            int startCurtainY = (overlaySize.height() - 0) / 2; // 初始高度为0，垂直居中
            int endCurtainY = (overlaySize.height() - targetCurtainHeight) / 2; // 最终高度为 targetCurtainHeight，垂直居中

            // 1. 创建半透明黑幕
            QWidget* curtainWidget = new QWidget(m_animationOverlayWidget);
            curtainWidget->setStyleSheet("background-color: rgba(0, 0, 0, 150);");
            // 初始几何形状：x=0, y=startCurtainY, width=curtainWidth, height=0
            curtainWidget->setGeometry(0, startCurtainY, curtainWidth, 0);
            curtainWidget->setAttribute(Qt::WA_TransparentForMouseEvents);
            curtainWidget->show();

            // 增加字间距
            QString spacedMessage;
            for (int i = 0; i < message.length(); ++i) {
                spacedMessage.append(message.at(i));
                if (i < message.length() - 1) {
                    spacedMessage.append(" "); // 在每个字符后添加一个空格
                }
            }

            // 2. 创建消息标签
            QLabel* messageLabel = new QLabel(spacedMessage, curtainWidget); // 使用 spacedMessage
            messageLabel->setStyleSheet("color: white; ");
            messageLabel->setAlignment(Qt::AlignCenter);
            messageLabel->setFont(QFont("YouYuan", overlaySize.height()/25, QFont::Bold));
            // 消息标签的几何形状设置为覆盖 curtainWidget 的整个区域，以便文本居中
            messageLabel->setGeometry(0, 0, curtainWidth, targetCurtainHeight); // 相对于 curtainWidget 的 (0,0)

            // 为消息标签创建不透明度效果
            QGraphicsOpacityEffect* messageOpacityEffect = new QGraphicsOpacityEffect(messageLabel);
            messageOpacityEffect->setOpacity(0.0); // 初始完全透明
            messageLabel->setGraphicsEffect(messageOpacityEffect); // messageLabel 接管所有权

            messageLabel->show();  // 确保标签显示
            messageLabel->raise(); // 确保标签在父控件背景之上

            // 3. 定义动画序列
            QSequentialAnimationGroup* sequentialGroup = new QSequentialAnimationGroup(curtainWidget); // 以 curtainWidget 为父对象，便于清理

            // 阶段1: 黑幕展开 (0.3秒)
            QPropertyAnimation* animCurtainExpand = new QPropertyAnimation(curtainWidget, "geometry");
            animCurtainExpand->setDuration(300);
            animCurtainExpand->setStartValue(QRect(0, startCurtainY, curtainWidth, 0)); // 初始：全宽，高0
            animCurtainExpand->setEndValue(QRect(0, endCurtainY, curtainWidth, targetCurtainHeight)); // 结束：全宽，高 targetCurtainHeight
            animCurtainExpand->setEasingCurve(QEasingCurve::OutQuad);
            sequentialGroup->addAnimation(animCurtainExpand);

            // 阶段2: 消息渐入 (0.2秒)
            QPropertyAnimation* animMessageFadeIn = new QPropertyAnimation(messageOpacityEffect, "opacity");
            animMessageFadeIn->setDuration(200);
            animMessageFadeIn->setStartValue(0.0);
            animMessageFadeIn->setEndValue(1.0);
            animMessageFadeIn->setEasingCurve(QEasingCurve::OutQuad);
            sequentialGroup->addAnimation(animMessageFadeIn);

            // 阶段3: 暂停 (1.4秒)
            QPauseAnimation* pauseAnim = new QPauseAnimation(1400);
            sequentialGroup->addAnimation(pauseAnim);

            // 阶段4: 消息渐出 (0.2秒)
            QPropertyAnimation* animMessageFadeOut = new QPropertyAnimation(messageOpacityEffect, "opacity");
            animMessageFadeOut->setDuration(200);
            animMessageFadeOut->setStartValue(1.0);
            animMessageFadeOut->setEndValue(0.0);
            animMessageFadeOut->setEasingCurve(QEasingCurve::InQuad);
            sequentialGroup->addAnimation(animMessageFadeOut);

            // 阶段5: 黑幕收缩 (0.3秒)
            QPropertyAnimation* animCurtainCollapse = new QPropertyAnimation(curtainWidget, "geometry");
            animCurtainCollapse->setDuration(300);
            animCurtainCollapse->setStartValue(QRect(0, endCurtainY, curtainWidth, targetCurtainHeight)); // 初始：全宽，高 targetCurtainHeight
            animCurtainCollapse->setEndValue(QRect(0, startCurtainY, curtainWidth, 0)); // 结束：全宽，高0
            animCurtainCollapse->setEasingCurve(QEasingCurve::InQuad);
            sequentialGroup->addAnimation(animCurtainCollapse);

            // 6. 动画结束后清理资源并发出响应
            QObject::connect(sequentialGroup, &QAbstractAnimation::finished, this,
                             [this, curtainWidget, opId]() { // 显式捕获 'this'
                                 curtainWidget->deleteLater(); // 删除 curtainWidget 会自动删除其子对象 messageLabel 和 messageOpacityEffect
                                 emit responseUserInput(opId);
                             });

            sequentialGroup->start(QAbstractAnimation::DeleteWhenStopped); // 启动动画组，并在完成后自动删除
        });
        break;
    }
    case PromptData::CardInAnimation:
    case PromptData::CardOutAnimation: {
        int opId = pd.autoInput;
        QTimer::singleShot(500, this, [this, opId](){ // 显式捕获 'this'
            // --- 确保 m_animationOverlayWidget 是鼠标事件透明的 ---
            m_animationOverlayWidget->setAttribute(Qt::WA_TransparentForMouseEvents, true);
            emit responseUserInput(opId);
        });
        break;
    }
    case PromptData::BuyCardAnimation: {
        Player* buyer = pd.buyer;
        Card* cardToBuy = pd.card;
        int opId = pd.autoInput;

        QTimer::singleShot(0, this, [this, buyer, cardToBuy, opId]() { // 显式捕获 'this'
            // --- 确保 m_animationOverlayWidget 是鼠标事件透明的 ---
            m_animationOverlayWidget->setAttribute(Qt::WA_TransparentForMouseEvents, true);

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

            connect(sequentialGroup, &QSequentialAnimationGroup::finished, this, [this, animatingCardWidget, opId]() { // 显式捕获 'this'
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

