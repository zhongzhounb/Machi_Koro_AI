#include "slotwidget.h"
#include "cardwidget.h"
#include "card.h"

#include <QPixmap>
#include <QResizeEvent>
#include <QDebug>
#include <QFont>
#include <QPainter>

// --- 参考尺寸和位置常量 ---
const int CARD_REF_WIDTH = 200;
const int CARD_REF_HEIGHT = 300;


const QRect IMG_RECT(0, 0, CARD_REF_WIDTH, CARD_REF_HEIGHT);
QRect SlotWidget::scaledRect(const QRect& originalRect, qreal scaleX, qreal scaleY) {
    int x = qRound(originalRect.x() * scaleX);
    int y = qRound(originalRect.y() * scaleY);
    int width = qRound(originalRect.width() * scaleX);
    int height = qRound(originalRect.height() * scaleY);
    return QRect(x, y, width, height);
}

SlotWidget::SlotWidget(bool isSupplyPile, const QString& supplyPileBackImagePath, QWidget* parent)
    : QFrame(parent)
    , m_isSupplyPile(isSupplyPile)
    , m_countOverlayLabel(new QLabel(this))
    , m_stackedLayout(new QStackedLayout())
    , m_supplyPileBackImagePath(supplyPileBackImagePath)
{
    setFrameShape(QFrame::Box);

    initUI();
    setLayout(m_stackedLayout);
    updateDisplay();
    updatePosition();
}

SlotWidget::~SlotWidget()
{
    // Qt 的父子机制会自动删除子控件
}

void SlotWidget::initUI()
{
    m_backgroundLabel->setScaledContents(true);
    m_backgroundLabel->setAlignment(Qt::AlignCenter);

    m_countOverlayLabel->setStyleSheet("QLabel { background-color: rgba(0, 0, 0, 180); color: white; border-radius: 5px; padding: 2px; }");
    m_countOverlayLabel->setAlignment(Qt::AlignCenter);
    m_countOverlayLabel->setFont(QFont("Arial", 10, QFont::Bold));
    m_countOverlayLabel->hide();

    m_stackedLayout->addWidget(m_backgroundLabel);

    setMinimumSize(SLOT_REF_WIDTH, SLOT_REF_HEIGHT);
}

void SlotWidget::pushCard(CardWidget* cardWidget)
{
    if (!cardWidget) {
        qWarning() << "SlotWidget::pushCard: Attempted to push a null CardWidget.";
        return;
    }

    // 断开旧的顶层卡牌的点击信号连接
    if (m_currentTopCardWidget) {
        disconnect(m_currentTopCardWidget, &CardWidget::clicked, this, &SlotWidget::onTopCardClicked);
        m_currentTopCardWidget->hide(); // 隐藏被覆盖的卡牌
    }

    cardWidget->setParent(this); // 设置父对象
    m_cards.append(cardWidget);

    if (m_stackedLayout->indexOf(cardWidget) == -1) {
        m_stackedLayout->addWidget(cardWidget);
    }

    m_currentTopCardWidget = cardWidget; // 更新当前顶层卡牌
    connect(m_currentTopCardWidget, &CardWidget::clicked, this, &SlotWidget::onTopCardClicked);

    updateDisplay();
}

CardWidget* SlotWidget::topCard() const
{
    if (m_cards.isEmpty())
        return nullptr;
    return m_cards.last();
}

CardWidget* SlotWidget::popCard()
{
    if (m_cards.isEmpty())
        return nullptr;

    CardWidget* poppedCard = m_cards.takeLast();

    // 断开弹出卡牌的信号连接
    if (poppedCard == m_currentTopCardWidget) {
        disconnect(m_currentTopCardWidget, &CardWidget::clicked, this, &SlotWidget::onTopCardClicked);
        m_currentTopCardWidget = nullptr;
    }

    m_stackedLayout->removeWidget(poppedCard);
    poppedCard->setParent(nullptr); // 解除父子关系
    poppedCard->hide();

    if (!m_cards.isEmpty()) {
        CardWidget* newTopCard = m_cards.last();
        newTopCard->show();
        m_currentTopCardWidget = newTopCard; // 设置新的顶层卡牌
        connect(m_currentTopCardWidget, &CardWidget::clicked, this, &SlotWidget::onTopCardClicked);
    }

    updateDisplay();
    return poppedCard;
}

void SlotWidget::setDisplayedCount(int count)
{
    if (m_isSupplyPile) {
        m_displayedCount = count;
        updateDisplay();
    } else {
        qWarning() << "SlotWidget::setDisplayedCount called on a non-supply pile slot. Ignoring.";
    }
}

void SlotWidget::updateDisplay()
{
    if (m_cards.isEmpty()) {
        QPixmap pixmap;
        if (m_isSupplyPile) {
            pixmap.load(m_supplyPileBackImagePath);
        } else {
            pixmap.load(EMPTY_SLOT_IMAGE_PATH);
        }

        if (pixmap.isNull()) {
            qWarning() << "Failed to load image for slot (empty/back):" << (m_isSupplyPile ? m_supplyPileBackImagePath : EMPTY_SLOT_IMAGE_PATH);
            m_backgroundLabel->setText("Image Missing");
            m_backgroundLabel->setStyleSheet("QLabel { background-color: gray; color: white; }");
        } else {
            m_backgroundLabel->setPixmap(pixmap);
            m_backgroundLabel->clear();
        }
        m_stackedLayout->setCurrentWidget(m_backgroundLabel);
        m_backgroundLabel->show();

    } else {
        CardWidget* currentTopCard = m_cards.last();
        m_stackedLayout->setCurrentWidget(currentTopCard);
        currentTopCard->show();
    }

    // 更新数量叠加层
    int currentCount = m_cards.size();
    if (m_isSupplyPile && m_displayedCount != -1) {
        currentCount = m_displayedCount; // 如果是供应堆且设置了覆盖数量，则使用覆盖数量
    }

    if (currentCount > 1) {
        m_countOverlayLabel->setText(QString("x%1").arg(currentCount));
        m_countOverlayLabel->show();
    } else {
        m_countOverlayLabel->hide();
    }
    m_countOverlayLabel->raise();
    updatePosition();
}

void SlotWidget::updatePosition()
{
    qreal scaleX = static_cast<qreal>(width()) / SLOT_REF_WIDTH;
    qreal scaleY = static_cast<qreal>(height()) / SLOT_REF_HEIGHT;
    qreal fontScale = qMin(scaleX, scaleY);

    //m_stackedLayout->setGeometry(scaledRect(IMG_RECT,scaleX,scaleY));

    if (!m_countOverlayLabel->isHidden()) {
        int labelWidth = qRound(SLOT_REF_WIDTH * fontScale / 3.0);
        int labelHeight = qRound(SLOT_REF_HEIGHT * fontScale / 5.0);
        labelWidth = qMax(labelWidth, 20);
        labelHeight = qMax(labelHeight, 15);

        int padding = qRound(5 * fontScale);
        m_countOverlayLabel->setGeometry(width() - labelWidth - padding, padding, labelWidth, labelHeight);

        QFont countFont = m_countOverlayLabel->font();
        countFont.setPointSize(qRound(10 * fontScale));
        m_countOverlayLabel->setFont(countFont);
    }
}

void SlotWidget::resizeEvent(QResizeEvent *event)
{
    QFrame::resizeEvent(event);
    updatePosition();
}

void SlotWidget::onTopCardClicked(Card* card)
{
    // 转发信号，不带索引，由 CardStoreWidget 确定
    emit topCardClickedInSlot(card);
}

QSize SlotWidget::sizeHint() const
{
    return QSize(CARD_REF_WIDTH, CARD_REF_HEIGHT);
}

int SlotWidget::heightForWidth(int w) const
{
    return qRound(w * (static_cast<qreal>(CARD_REF_HEIGHT) / CARD_REF_WIDTH));
}
