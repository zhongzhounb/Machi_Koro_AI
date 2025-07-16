#include "playerareawidget.h"
#include "card.h"
#include "slotwidget.h"
#include "cardwidget.h"
#include "player.h"

#include <QScrollArea>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QResizeEvent> // 确保包含 QResizeEvent 头文件
#include <QTimer>
#include <QDebug>

// 辅助函数：根据ID排序
int getOrderId(Card* card) {
    if (!card) return 0;
    int num = 0;
    num += card->getActLNum() * 100000;
    num += card->getActRNum() * 1000;
    num += (int)card->getColor() * 100;
    num += card->getNameId();
    return num;
}


PlayerAreaWidget::PlayerAreaWidget(Player* player, bool isHBoxLayout, bool isLandMark, QWidget* parent)
    : QWidget(parent),
    m_player(player),
    m_isHBoxLayout(isHBoxLayout),
    m_isLandMark(isLandMark)
{
    // 1. PlayerAreaWidget 自己的主布局 (垂直或水平都不影响，怎么都得有一个布局)
    QVBoxLayout* topLevelLayout = new QVBoxLayout(this);
    topLevelLayout->setContentsMargins(0, 0, 0, 0);

    // 2. QScrollArea
    m_scrollArea = new QScrollArea(this);
    m_scrollArea->setWidgetResizable(true); // 保持为 true，m_contentWidget 将填充视口
    m_scrollArea->setFrameShape(QFrame::NoFrame);
    m_scrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    m_scrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    m_scrollArea->viewport()->setAutoFillBackground(false);

    // 3. ScrollArea的直接子控件 (现在它就是内容容器)
    m_contentWidget = new QWidget();
    m_scrollArea->setWidget(m_contentWidget);

    // 4. 为 m_contentWidget 创建真正的卡牌布局
    // 布局现在直接应用于 m_contentWidget
    if (isHBoxLayout) {
        m_cardLayout = new QHBoxLayout(m_contentWidget);
    } else {
        m_cardLayout = new QVBoxLayout(m_contentWidget);
    }
    m_cardLayout->setContentsMargins(10, 5, 10, 5); // 可以给卡牌组设置一些内外边距
    m_cardLayout->setSpacing(15);                   // 卡槽之间的间距
    m_cardLayout->setAlignment(Qt::AlignCenter);   // 关键：将居中对齐设置到卡牌布局本身

    // 5. 将 ScrollArea 添加到 PlayerAreaWidget 的主布局中
    topLevelLayout->addWidget(m_scrollArea);
}

PlayerAreaWidget::~PlayerAreaWidget() {}

void PlayerAreaWidget::onCardAdded(Player* player, Card* card)
{
    if (player != m_player) return;
    if (!card) return;
    if ((m_isLandMark && card->getType() != Type::Landmark) || (!m_isLandMark && card->getType() == Type::Landmark)) {
        return;
    }

    // 检查是否已有同名卡槽
    for (SlotWidget* slotWidget : m_slots) {
        CardWidget* topCard = slotWidget->topCard();
        if (topCard && topCard->getCard() && topCard->getCard()->getName() == card->getName()) {
            slotWidget->pushCard(new CardWidget(card, ShowType::Ordinary, slotWidget));
            return;
        }
    }

    // 没找到，新建一个并找到插入位置
    int addIndex = 0;
    for (const auto& slotWidget : m_slots) {
        if (getOrderId(slotWidget->topCard()->getCard()) > getOrderId(card)) {
            break;
        }
        addIndex++;
    }

    // 创建新槽和新卡，父对象是 m_contentWidget (而不是之前的 m_cardContainer)
    SlotWidget* newSlotWidget = new SlotWidget(false, Color::BackNone, m_contentWidget);
    newSlotWidget->pushCard(new CardWidget(card, ShowType::Ordinary, newSlotWidget));

    // 如果是普通卡，则长宽有一遍是不固定的，需要计算。
    if(!m_isLandMark){
        // 获取 PlayerAreaWidget 的当前高度作为目标卡牌尺寸
        int targetCardSize;
        if(m_isHBoxLayout)
            targetCardSize = qMax(this->height()-30,40);
        else
            targetCardSize = qMax(this->width()-30,40);
        if (targetCardSize > 0)
            newSlotWidget->setFixedSize(targetCardSize, targetCardSize);
    }


    m_slots.insert(addIndex, newSlotWidget);

    // 将新卡槽添加到 m_cardLayout 中
    m_cardLayout->insertWidget(addIndex, newSlotWidget);

    // 布局更新通常由 insertWidget 自动触发，无需手动调用 update()
}

void PlayerAreaWidget::resizeEvent(QResizeEvent* event) {
    QWidget::resizeEvent(event); // 调用基类的实现

    if(!m_isLandMark){
        // 获取 PlayerAreaWidget 的当前高度作为目标卡牌尺寸
        int targetCardSize;
        if(m_isHBoxLayout)
            targetCardSize = qMax(this->height()-30,40);
        else
            targetCardSize = qMax(this->width()-30,40);

        if (targetCardSize > 0) {
            // 遍历所有已有的 SlotWidget，并设置它们的固定大小
            // 这将确保所有 SlotWidget 都是一个正方形，边长等于 PlayerAreaWidget 的高度
            for (SlotWidget* slotWidget : m_slots) {
                slotWidget->setFixedSize(targetCardSize, targetCardSize);
            }
        }
    }
}
