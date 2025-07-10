#include "slotwidget.h"
#include "cardwidget.h"
#include "card.h"

#include <QPixmap>
#include <QResizeEvent>
#include <QDebug>
#include <QFont>
#include <QPainter>

#include <QGraphicsDropShadowEffect> // 新增：阴影效果头文件

// --- 参考尺寸和位置常量 ---

//图片圆角算法
QPixmap QPixmapToRound(const QPixmap & img, int radius);


QString getBackImgPath(const QString type){
    return QString( ":/resources/images/card/back/%1.png").arg(type);
}


SlotWidget::SlotWidget(bool isSupplyPile, Color supplyColor, QWidget* parent)
    : QFrame(parent)
    , m_isSupplyPile(isSupplyPile)
    , m_countOverlayLabel(new QLabel(this)) // 确保父对象是 SlotWidget
    , m_stackedLayout(new QStackedLayout())
    , m_supplyColor(supplyColor)
    ,m_displayedCount(0)
{
    //设置主布局
    setLayout(m_stackedLayout);
    setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);

    //设置子布局
    initUI();
    updateDisplay(); // 初始显示更新
}

SlotWidget::~SlotWidget()
{
    // Qt 的父子机制会自动删除子控件
}

void SlotWidget::initUI()
{
    // 设置阴影效果
    QGraphicsDropShadowEffect *shadowEffect = new QGraphicsDropShadowEffect(this);
    shadowEffect->setBlurRadius(20); // 阴影模糊半径
    shadowEffect->setColor(QColor(0, 0, 0, 150)); // 阴影颜色 (黑色，半透明)
    shadowEffect->setOffset(5, 5); // 阴影偏移量 (x, y)
    setGraphicsEffect(shadowEffect);
    //空卡槽默认图片
    Card* emptyCard=new Card("",-1,Color::BackNone,Type::None,-1,-1,-1,this);
    CardWidget* emptyCardWidget=new CardWidget(emptyCard,ShowType::BackGround,this);
    m_stackedLayout->addWidget(emptyCardWidget);
    m_cards.append(emptyCardWidget);

    //如果是供应堆则设置默认有卡图片
    if(m_isSupplyPile){
        Card* supplyCard=new Card("",-1,m_supplyColor,Type::None,-1,-1,-1,this);
        CardWidget* supplyCardWidget=new CardWidget(supplyCard,ShowType::BackGround,this);
        m_stackedLayout->addWidget(supplyCardWidget);
        m_cards.append(supplyCardWidget);
    }

    // 文字上色
    m_countOverlayLabel->setStyleSheet("QLabel { background-color: rgba(0, 0, 0, 180); color: white; border-radius: 5px;}");

    // 文字布局
    m_countOverlayLabel->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
    // 初始隐藏，只有当数量大于1时才显示
    m_countOverlayLabel->hide();
}

void SlotWidget::pushCard(CardWidget* cardWidget)
{
    if (!cardWidget) {
        qWarning() << "SlotWidget::pushCard: Attempted to push a null CardWidget.";
        return;
    }

    // 将 cardWidget 的父对象设置为 this (SlotWidget)，这更符合逻辑
    cardWidget->setParent(this);

    // 断开旧的顶层卡牌的点击信号连接
    if (m_currentTopCardWidget)
        disconnect(m_currentTopCardWidget, &CardWidget::clicked, this, &SlotWidget::onTopCardClicked);

    // cardWidget->setParent(this); // SlotWidget::pushCard 内部已经设置了父对象，这里可以省略
    m_cards.append(cardWidget);
    m_stackedLayout->addWidget(cardWidget);


    m_currentTopCardWidget = cardWidget; // 更新当前顶层卡牌
    connect(m_currentTopCardWidget, &CardWidget::clicked, this, &SlotWidget::onTopCardClicked);

    m_displayedCount++;

    updateDisplay();
}

// 新增：实现更新内部卡牌尺寸的方法
void SlotWidget::updateCardSize(int cardHeight)
{
    for (CardWidget* card : m_cards) {
        if (card) {
            card->setSizeWithAspectRatio(cardHeight);
        }
    }
    m_countOverlayLabel->setGeometry(width() - 40, 5, 35, 20);

    // !!! 新增：通知布局 sizeHint 已经改变，需要重新计算布局 !!!
    updateGeometry();

    // +++ 新增的调试代码 +++
    qDebug() << "SlotWidget" << this << "size hint updated to:" << this->sizeHint();
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
    // 如果槽里只有默认的背景卡，则无法弹出
    // 根据您的 initUI 逻辑，供应堆有2张背景卡，普通槽有1张
    int baseCardCount = m_isSupplyPile ? 2 : 1;
    if (m_cards.size() <= baseCardCount) {
        return nullptr;
    }

    // 断开当前顶层卡牌的信号连接
    if (m_currentTopCardWidget) {
        disconnect(m_currentTopCardWidget, &CardWidget::clicked, this, &SlotWidget::onTopCardClicked);
    }

    CardWidget* poppedCard = m_cards.takeLast();
    m_stackedLayout->removeWidget(poppedCard);
    poppedCard->setParent(nullptr); // 解除父子关系，使其成为一个独立的顶层窗口
    poppedCard->hide();

    m_displayedCount--; // 减少显示的卡牌数量

    // 更新 m_currentTopCardWidget 指向新的顶层卡牌
    if (m_cards.size() > baseCardCount) {
        m_currentTopCardWidget = m_cards.last();
        connect(m_currentTopCardWidget, &CardWidget::clicked, this, &SlotWidget::onTopCardClicked);
    } else {
        // 如果只剩下背景卡，则没有可点击的顶层卡牌了
        m_currentTopCardWidget = nullptr;
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
    } else {
        m_countOverlayLabel->hide();
    }
    //覆盖其上方
    m_countOverlayLabel->raise();

}


void SlotWidget::onTopCardClicked(Card* card)
{
    // 转发信号，不带索引，由 CardStoreWidget 确定
    emit topCardClickedInSlot(card);
}
QSize SlotWidget::sizeHint() const
{
    // SlotWidget 的理想尺寸应该由它内部的顶层卡牌决定
    CardWidget* top = topCard();
    if (top) {
        // 如果有卡牌，就返回那张卡牌的当前尺寸
        // 如果卡牌还未设置尺寸，size() 可能为0，但 setFixedSize 后就会有值
        return top->size();
    }

    // 如果没有卡牌（或只有背景卡），返回一个默认的最小尺寸
    // 防止在没有卡牌时尺寸变为0
    return QSize(80, 80); // 您可以根据需要调整这个默认值
}
