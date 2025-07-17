#include "slotwidget.h"
#include "cardwidget.h"
#include "card.h"
#include <QDebug>
#include <QGraphicsDropShadowEffect>

SlotWidget::SlotWidget(bool isSupplyPile, Color supplyColor, QWidget* parent)
    : QFrame(parent)
    , m_isSupplyPile(isSupplyPile)
    , m_supplyColor(supplyColor)
    , m_displayedCount(0)
    , m_stackedLayout(new QStackedLayout())
    , m_countOverlayLabel(new QLabel(this))
{
    // *** 新增：设置为可伸缩，以填充网格单元 ***
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
    Card* emptyCard = new Card("",-1,Color::BackNone,Type::None,-1,-1,-1,this);
    CardWidget* emptyCardWidget = new CardWidget(emptyCard, ShowType::BackGround, this);
    m_stackedLayout->addWidget(emptyCardWidget);
    m_cards.append(emptyCardWidget);

    // 如果是供应堆则设置默认有卡图片
    if(m_isSupplyPile){
        Card* supplyCard = new Card("",-1,m_supplyColor,Type::None,-1,-1,-1,this);
        CardWidget* supplyCardWidget = new CardWidget(supplyCard,ShowType::BackGround,this);
        m_stackedLayout->addWidget(supplyCardWidget);
        m_cards.append(supplyCardWidget);
    }

    m_countOverlayLabel->setStyleSheet("QLabel { background-color: rgba(0, 0, 0, 180); color: white; border-radius: 5px; padding: 2px; }");
    m_countOverlayLabel->setAlignment(Qt::AlignCenter);
    m_countOverlayLabel->hide();
}

void SlotWidget::pushCard(CardWidget* cardWidget)
{
    if (!cardWidget) return;

    cardWidget->setParent(this);
    if (m_currentTopCardWidget) {
        disconnect(m_currentTopCardWidget, &CardWidget::clicked, this, &SlotWidget::onTopCardClicked);
    }

    m_cards.append(cardWidget);
    m_stackedLayout->addWidget(cardWidget);

    m_currentTopCardWidget = cardWidget;
    connect(m_currentTopCardWidget, &CardWidget::clicked, this, &SlotWidget::onTopCardClicked);

    addCount();
}

CardWidget* SlotWidget::popCard()
{
    if (m_cards.size() == 1) {
        return nullptr;
    }

    //如果是供应堆且还有大于等于2的牌，则直接返回供应堆牌
    if(m_isSupplyPile&&m_displayedCount>1){
        delCount();
        return m_cards.last();
    }

    disconnect(m_currentTopCardWidget, &CardWidget::clicked, this, &SlotWidget::onTopCardClicked);

    CardWidget* poppedCard = m_cards.takeLast();
    m_stackedLayout->removeWidget(poppedCard);
    poppedCard->setParent(nullptr);
    poppedCard->hide();

    m_currentTopCardWidget = m_cards.last();
    connect(m_currentTopCardWidget, &CardWidget::clicked, this, &SlotWidget::onTopCardClicked);

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
        displayIndex = qMin(displayIndex, 1);
    }
    m_stackedLayout->setCurrentIndex(displayIndex);

    if (m_displayedCount > 1) {
        m_countOverlayLabel->setText(QString("x%1").arg(m_displayedCount));
        m_countOverlayLabel->show();
        m_countOverlayLabel->raise();
    } else {
        m_countOverlayLabel->hide();
    }
}

void SlotWidget::onTopCardClicked(Card* card)
{
    emit topCardClickedInSlot(card);
}

