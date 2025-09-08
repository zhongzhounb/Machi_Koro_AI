#include "slotwidget.h"
#include "cardwidget.h"
#include "card.h"
#include <QDebug>
#include <QGraphicsDropShadowEffect>
#include <QResizeEvent> // 确保包含此头文件

SlotWidget::SlotWidget(bool isSupplyPile, Color supplyColor, QWidget* parent)
    : QFrame(parent)
    , m_isSupplyPile(isSupplyPile)
    , m_supplyColor(supplyColor)
    , m_displayedCount(0)
    , m_stackedLayout(new QStackedLayout())
    , m_countOverlayLabel(new QLabel(this))
{
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    setLayout(m_stackedLayout);
    initUI();
    updateDisplay();
}

SlotWidget::~SlotWidget() {}

void SlotWidget::initUI()
{
    QGraphicsDropShadowEffect *shadowEffect = new QGraphicsDropShadowEffect(this);
    shadowEffect->setBlurRadius(15);
    shadowEffect->setColor(QColor(0, 0, 0, 120));
    shadowEffect->setOffset(4, 4);
    setGraphicsEffect(shadowEffect);

    // 空卡槽默认图片
    Card* emptyCard = new Card("",-1,Color::BackNone,Type::None,-1,-1,-1,CommandType::None,this);
    CardWidget* emptyCardWidget = new CardWidget(emptyCard, ShowType::BackGround, this);
    m_stackedLayout->addWidget(emptyCardWidget);
    m_cards.append(emptyCardWidget);

    // 连接空卡槽的信号（虽然通常不会悬停在背景卡上，但为了完整性）
    connect(emptyCardWidget, &CardWidget::requestShowDetailedCard, this, &SlotWidget::handleCardWidgetRequestShowDetailed);
    connect(emptyCardWidget, &CardWidget::requestHideDetailedCard, this, &SlotWidget::handleCardWidgetRequestHideDetailed);


    // 如果是供应堆则设置默认有卡图片
    if(m_isSupplyPile){
        Card* supplyCard = new Card("",-1,m_supplyColor,Type::None,-1,-1,-1,CommandType::None,this);
        CardWidget* supplyCardWidget = new CardWidget(supplyCard,ShowType::BackGround,this);
        m_stackedLayout->addWidget(supplyCardWidget);
        m_cards.append(supplyCardWidget);
        // 连接供应堆背景卡槽的信号
        connect(supplyCardWidget, &CardWidget::requestShowDetailedCard, this, &SlotWidget::handleCardWidgetRequestShowDetailed);
        connect(supplyCardWidget, &CardWidget::requestHideDetailedCard, this, &SlotWidget::handleCardWidgetRequestHideDetailed);
    }

    m_countOverlayLabel->setStyleSheet("QLabel { background-color: rgba(0, 0, 0, 180); color: white; border-radius: 5px; padding: 2px; }");
    m_countOverlayLabel->setAlignment(Qt::AlignCenter);
    m_countOverlayLabel->hide();
    // 初始设置标签的尺寸和位置
    // 在 SlotWidget 构造完成后，resizeEvent 会被触发，所以这里不需要额外设置
    // 但如果 SlotWidget 的大小在构造时是固定的，并且之后不会改变，那么可以在这里设置一次
    // 考虑到 SlotWidget 可能会被布局管理，resizeEvent 是更好的选择。
}

void SlotWidget::pushCard(CardWidget* cardWidget)
{
    if (!cardWidget) return;

    cardWidget->setParent(this);
    if (m_currentTopCardWidget) {
        disconnect(m_currentTopCardWidget, &CardWidget::clicked, this, &SlotWidget::onTopCardClicked);
        // 断开旧的 CardWidget 的详细显示信号
        disconnect(m_currentTopCardWidget, &CardWidget::requestShowDetailedCard, this, &SlotWidget::handleCardWidgetRequestShowDetailed);
        disconnect(m_currentTopCardWidget, &CardWidget::requestHideDetailedCard, this, &SlotWidget::handleCardWidgetRequestHideDetailed);
    }

    m_cards.append(cardWidget);
    m_stackedLayout->addWidget(cardWidget);

    m_currentTopCardWidget = cardWidget;
    connect(m_currentTopCardWidget, &CardWidget::clicked, this, &SlotWidget::onTopCardClicked);
    // 连接新的 CardWidget 的详细显示信号
    connect(m_currentTopCardWidget, &CardWidget::requestShowDetailedCard, this, &SlotWidget::handleCardWidgetRequestShowDetailed);
    connect(m_currentTopCardWidget, &CardWidget::requestHideDetailedCard, this, &SlotWidget::handleCardWidgetRequestHideDetailed);

    addCount();
}

CardWidget* SlotWidget::popCard()
{
    if (m_cards.size() == 1) { // 只剩下背景卡
        return nullptr;
    }

    // 如果是供应堆且还有大于1的牌，我们只是减少计数，不移除实际的CardWidget
    // 因为供应堆的CardWidget是作为背景/占位符存在的
    if(m_isSupplyPile && m_displayedCount > 1){
        delCount(); // 仅仅减少计数
        return m_cards.last(); // 返回最顶层的CardWidget（即供应堆的视觉表示卡），但它不是被“弹出”的实际卡牌
    }

    // 对于非供应堆或供应堆只剩一张牌的情况，执行实际的弹出操作
    disconnect(m_currentTopCardWidget, &CardWidget::clicked, this, &SlotWidget::onTopCardClicked);
    disconnect(m_currentTopCardWidget, &CardWidget::requestShowDetailedCard, this, &SlotWidget::handleCardWidgetRequestShowDetailed);
    disconnect(m_currentTopCardWidget, &CardWidget::requestHideDetailedCard, this, &SlotWidget::handleCardWidgetRequestHideDetailed);


    CardWidget* poppedCard = m_cards.takeLast();
    m_stackedLayout->removeWidget(poppedCard);
    poppedCard->setParent(nullptr);
    poppedCard->hide();

    // 更新 m_currentTopCardWidget 为新的顶部卡片
    if (!m_cards.isEmpty()) {
        m_currentTopCardWidget = m_cards.last();
        connect(m_currentTopCardWidget, &CardWidget::clicked, this, &SlotWidget::onTopCardClicked);
        connect(m_currentTopCardWidget, &CardWidget::requestShowDetailedCard, this, &SlotWidget::handleCardWidgetRequestShowDetailed);
        connect(m_currentTopCardWidget, &CardWidget::requestHideDetailedCard, this, &SlotWidget::handleCardWidgetRequestHideDetailed);
    } else {
        m_currentTopCardWidget = nullptr; // 此时应该只剩下背景卡，或者没有卡片
    }

    delCount();

    return poppedCard;
}


CardWidget* SlotWidget::topCard() const
{
    if (m_cards.isEmpty()) return nullptr;
    return m_cards.last();
}

void SlotWidget::addCount()
{
    m_displayedCount++;
    updateDisplay();
}

void SlotWidget::delCount()
{
    m_displayedCount--;
    updateDisplay();
}

void SlotWidget::updateDisplay()
{
    int displayIndex = m_displayedCount;
    if(m_isSupplyPile) {
        // 对于供应堆，如果数量大于0，则显示索引1的卡片（即实际的供应卡），否则显示索引0（背景卡）
        displayIndex = (m_displayedCount > 0) ? 1 : 0;
    } else {
        // 对于普通卡槽，如果数量大于0，则显示最顶层的卡片，否则显示索引0（背景卡）
        displayIndex = (m_displayedCount > 0) ? m_cards.size() -1 : 0;
    }

    // 确保索引有效
    if (displayIndex < 0) displayIndex = 0;
    if (displayIndex >= m_stackedLayout->count()) displayIndex = m_stackedLayout->count() - 1;

    m_stackedLayout->setCurrentIndex(displayIndex);

    if (m_displayedCount > 1) {
        m_countOverlayLabel->setText(QString("×%1").arg(m_displayedCount));
        m_countOverlayLabel->show();
        m_countOverlayLabel->raise();
    } else {
        m_countOverlayLabel->hide();
    }
}

// 处理内部 CardWidget 的请求并转发
void SlotWidget::handleCardWidgetRequestShowDetailed(Card* card, QPoint globalPos)
{
    emit cardWidgetRequestShowDetailed(card, globalPos);
}

void SlotWidget::handleCardWidgetRequestHideDetailed()
{
    emit cardWidgetRequestHideDetailed();
}

void SlotWidget::onTopCardClicked(Card* card)
{
    emit topCardClickedInSlot(card);
}

// 重写 resizeEvent 来调整 m_countOverlayLabel 的大小和位置
void SlotWidget::resizeEvent(QResizeEvent *event)
{
    QFrame::resizeEvent(event); // 调用基类的 resizeEvent

    // 获取 SlotWidget 的当前尺寸
    int widgetWidth = width();
    int widgetHeight = height();

    // 计算标签的期望尺寸 (20% 宽度, 20% 高度)
    int labelWidth = static_cast<int>(widgetWidth * 0.40);
    int labelHeight = static_cast<int>(widgetHeight * 0.20);

    m_countOverlayLabel->setFont(QFont("YouYuan", widgetWidth/8, QFont::Bold));


    // 设置标签的几何形状 (左上角，计算出的宽度和高度)
    m_countOverlayLabel->setGeometry(0, 0, labelWidth, labelHeight);
}
