// playerareawidget.cpp

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
    // 1. PlayerAreaWidget 自己的主布局 (垂直)
    QVBoxLayout* topLevelLayout = new QVBoxLayout(this);
    topLevelLayout->setContentsMargins(0, 0, 0, 0);

    // 2. QScrollArea
    m_scrollArea = new QScrollArea(this);
    m_scrollArea->setWidgetResizable(true);
    m_scrollArea->setFrameShape(QFrame::NoFrame);
    m_scrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    m_scrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    m_scrollArea->viewport()->setAutoFillBackground(false);

    // 3. ScrollArea的直接子控件 (会被拉伸)
    m_contentWidget = new QWidget();
    m_scrollArea->setWidget(m_contentWidget);

    // 4. 为 m_contentWidget 创建一个布局，它的唯一任务就是【居中】m_cardContainer
    QHBoxLayout* centeringLayout = new QHBoxLayout(m_contentWidget);
    centeringLayout->setContentsMargins(0, 0, 0, 0);
    centeringLayout->setAlignment(Qt::AlignCenter); // 用 AlignCenter 来居中唯一的子控件

    // 5. 创建“中间容器” (Wrapper Widget)
    m_cardContainer = new QWidget();
    // 关键：设置它的尺寸策略，让它的大小由其内容（卡牌）决定，而不是被拉伸
    // 水平方向使用 Maximum，表示它将尽可能小以适应其内容，但不会超过其最大尺寸提示。
    // 垂直方向使用 Expanding，表示它将扩展以填充可用空间。
    // 结合 SlotWidget 的固定尺寸，这将确保 m_cardContainer 的宽度由卡片数量决定，并被居中。
    m_cardContainer->setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Expanding);


    // 6. 为“中间容器”创建真正的卡牌布局
    if (isHBoxLayout) {
        m_cardLayout = new QHBoxLayout(m_cardContainer);
    } else {
        m_cardLayout = new QVBoxLayout(m_cardContainer);
    }
    m_cardLayout->setContentsMargins(10, 5, 10, 5); // 可以给卡牌组设置一些内外边距
    m_cardLayout->setSpacing(15);                   // 卡槽之间的间距

    // 7. 将“中间容器”添加到用于居中的布局中
    centeringLayout->addWidget(m_cardContainer);

    // 8. 将 ScrollArea 添加到 PlayerAreaWidget 的主布局中
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
    for (SlotWidget* slotWidget : qAsConst(m_slots)) {
        CardWidget* topCard = slotWidget->topCard();
        if (topCard && topCard->getCard() && topCard->getCard()->getName() == card->getName()) {
            slotWidget->pushCard(new CardWidget(card, ShowType::Ordinary, slotWidget));
            return;
        }
    }

    // 没找到，新建一个并找到插入位置
    int addIndex = 0;
    for (const auto& slotWidget : qAsConst(m_slots)) {
        if (getOrderId(slotWidget->topCard()->getCard()) > getOrderId(card)) {
            break;
        }
        addIndex++;
    }

    // 创建新槽和新卡，父对象是 m_cardContainer
    SlotWidget* newSlotWidget = new SlotWidget(false, Color::BackNone, m_cardContainer);
    newSlotWidget->pushCard(new CardWidget(card, ShowType::Ordinary, newSlotWidget));

    // 获取 PlayerAreaWidget 的当前高度作为目标卡牌尺寸
    int targetCardSize = qMax(this->height()-30,40);
    if (targetCardSize > 0)
        newSlotWidget->setFixedSize(targetCardSize, targetCardSize);

    m_slots.insert(addIndex, newSlotWidget);

    // 将新卡槽添加到 m_cardLayout 中
    m_cardLayout->insertWidget(addIndex, newSlotWidget);

    // 布局更新通常由 insertWidget 自动触发，无需手动调用 update()
}

void PlayerAreaWidget::resizeEvent(QResizeEvent* event) {
    QWidget::resizeEvent(event); // 调用基类的实现

    // 获取 PlayerAreaWidget 的当前高度作为目标卡牌尺寸
    int targetCardSize = qMax(this->height()-30,40);

    if (targetCardSize > 0) {
        // 遍历所有已有的 SlotWidget，并设置它们的固定大小
        // 这将确保所有 SlotWidget 都是一个正方形，边长等于 PlayerAreaWidget 的高度
        for (SlotWidget* slotWidget : qAsConst(m_slots)) {
            slotWidget->setFixedSize(targetCardSize, targetCardSize);
        }
    }
}
// --- 结束新增代码 ---
