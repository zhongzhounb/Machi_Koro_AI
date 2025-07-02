#include "slotwidget.h"
#include "cardwidget.h"
#include "card.h"

#include <QPixmap>
#include <QResizeEvent>
#include <QDebug>
#include <QFont>
#include <QPainter>


// --- 参考尺寸和位置常量 ---
const int SLOT_REF_WIDTH = 150;
const int SLOT_REF_HEIGHT = 300;

const QRect IMG_RECT(0, 0, SLOT_REF_WIDTH, SLOT_REF_HEIGHT);
const QRect COUNT_RECT(0, 0, SLOT_REF_WIDTH, 15);

const int COUNT_FONT_SIZE = 10;
// --- 参考尺寸和位置常量结束 ---


QString getBackImgPath(const QString type){
    return QString( ":/resources/images/card/back/%1.png").arg(type);
}


QRect SlotWidget::scaledRect(const QRect& originalRect, qreal scaleX, qreal scaleY) {
    int x = qRound(originalRect.x() * scaleX);
    int y = qRound(originalRect.y() * scaleY);
    int width = qRound(originalRect.width() * scaleX);
    int height = qRound(originalRect.height() * scaleY);
    return QRect(x, y, width, height);
}

SlotWidget::SlotWidget(bool isSupplyPile, const QString& supplyType, QWidget* parent)
    : QFrame(parent)
    , m_isSupplyPile(isSupplyPile)
    , m_countOverlayLabel(new QLabel(this))
    , m_stackedLayout(new QStackedLayout())
    , m_supplyType(supplyType)
    ,m_displayedCount(0)
{
    //设置主布局
    setLayout(m_stackedLayout);
    setMinimumSize(SLOT_REF_WIDTH, SLOT_REF_HEIGHT);

    //设置子布局
    initUI();
    updateDisplay();
}

SlotWidget::~SlotWidget()
{
    // Qt 的父子机制会自动删除子控件
}

void SlotWidget::initUI()
{
    //空卡槽默认图片
    QLabel* emptySlotLabel=new QLabel(this);
    QPixmap pixmap;
    pixmap.load(getBackImgPath("none"));
    if (pixmap.isNull()) {
        qWarning() << "Failed to load image for slot empty!" ;
        emptySlotLabel->setText("Image Missing");
    } else
        emptySlotLabel->setPixmap(pixmap);
    //保证图片能缩放
    emptySlotLabel->setScaledContents(true);
    //添加，下标为0
    m_stackedLayout->addWidget(emptySlotLabel);

    //如果是供应堆则设置默认有卡图片
    if(m_isSupplyPile){
        QLabel* supplySlotLabel=new QLabel(this);
        QPixmap pixmap;
        pixmap.load(getBackImgPath(m_supplyType));
        if (pixmap.isNull()) {
            qWarning() << "Failed to load image for slot supply!" ;
            supplySlotLabel->setText("Image Missing");
        } else
            supplySlotLabel->setPixmap(pixmap);
        //保证图片能缩放
        supplySlotLabel->setScaledContents(true);
        //添加，下标为1
        m_stackedLayout->addWidget(supplySlotLabel);
    }

    // 文字上色
    m_countOverlayLabel->setStyleSheet("QLabel { background-color: rgba(0, 0, 0, 180); color: white; border-radius: 5px;}");

    // 文字布局
    m_countOverlayLabel->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
}

void SlotWidget::pushCard(CardWidget* cardWidget)
{
    if (!cardWidget) {
        qWarning() << "SlotWidget::pushCard: Attempted to push a null CardWidget.";
        return;
    }

    // 断开旧的顶层卡牌的点击信号连接
    if (m_currentTopCardWidget)
        disconnect(m_currentTopCardWidget, &CardWidget::clicked, this, &SlotWidget::onTopCardClicked);

    cardWidget->setParent(this); // 设置父对象
    m_cards.append(cardWidget);
    m_stackedLayout->addWidget(cardWidget);


    m_currentTopCardWidget = cardWidget; // 更新当前顶层卡牌
    connect(m_currentTopCardWidget, &CardWidget::clicked, this, &SlotWidget::onTopCardClicked);

    m_displayedCount++;

    updateDisplay();
}

CardWidget* SlotWidget::topCard() const
{
    if (m_cards.isEmpty())
        return nullptr;
    return m_cards.last();
}

//还没改
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

void SlotWidget::addCount()
{
    m_displayedCount++;
    updateDisplay();
}

void SlotWidget::updateDisplay()
{
    //更新目前显示的下标（即为数量）
    int displayIndex=m_displayedCount;
    //如果是供应堆，只显示一个
    if(m_isSupplyPile)
        displayIndex=qMin(displayIndex,1);
    //更新
    m_stackedLayout->setCurrentIndex(displayIndex);

    // 更新数量叠加层
    if (m_displayedCount > 1) {
        m_countOverlayLabel->setText(QString("x%1").arg(m_displayedCount));
        m_countOverlayLabel->show();
    } else
        m_countOverlayLabel->hide();
    //覆盖其上方
    m_countOverlayLabel->raise();
    //重新计算位置
    updatePosition();
}

void SlotWidget::updatePosition()
{
    /*//计算缩放因子
    qreal scaleX = static_cast<qreal>(width()) / SLOT_REF_WIDTH;
    qreal scaleY = static_cast<qreal>(height()) / SLOT_REF_HEIGHT;
    qreal fontScale = qMin(scaleX, scaleY);

    //子类缩放
    m_stackedLayout->setGeometry(scaledRect(IMG_RECT,fontScale,fontScale));
    if (!m_countOverlayLabel->isHidden())
        m_countOverlayLabel->setGeometry(scaledRect(COUNT_RECT,fontScale,fontScale));

    //子类字体缩放
    if (!m_countOverlayLabel->isHidden()){
        QFont countFont("YouYuan",COUNT_FONT_SIZE * fontScale, QFont::Bold);
        m_countOverlayLabel->setFont(countFont);
    }*/

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
/*
QSize SlotWidget::sizeHint() const
{
    return QSize(SLOT_REF_WIDTH, SLOT_REF_HEIGHT);
}

int SlotWidget::heightForWidth(int w) const
{
    return qRound(w * (static_cast<qreal>(SLOT_REF_WIDTH) / SLOT_REF_HEIGHT));
}*/
