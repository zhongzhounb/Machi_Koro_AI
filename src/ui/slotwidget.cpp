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
    // 设置阴影效果（这个应该放在CardWidget上）
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

