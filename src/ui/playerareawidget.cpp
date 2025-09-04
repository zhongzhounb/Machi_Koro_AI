#include "playerareawidget.h"
#include "card.h"
#include "slotwidget.h"
#include "cardwidget.h"
#include "player.h"

#include <QScrollArea>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QResizeEvent>
#include <QTimer>
#include <QDebug>
#include <QPropertyAnimation>
#include <QEasingCurve>
#include <QParallelAnimationGroup>

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

QPoint PlayerAreaWidget::getCardTargetSlotCenterPos(Card* card) {
    Q_UNUSED(card); // 在简化版本中，card 参数未使用

    // 返回 m_contentWidget 的中心点在 PlayerAreaWidget 局部坐标系中的位置
    // 这是动画的“着陆区”，实际卡牌的精确位置由 PlayerAreaWidget::onCardAdded 决定
    if (m_contentWidget) {
        return m_contentWidget->mapToParent(m_contentWidget->rect().center());
    }
    return rect().center(); // 如果没有 contentWidget，则返回 PlayerAreaWidget 自身的中心
}

QSize PlayerAreaWidget::getTargetCardSize() {
    int targetCardSize = 0;
    if (!m_isLandMark) {
        if (m_isHBoxLayout)
            targetCardSize = qMax(this->height() - 30, 40);
        else
            targetCardSize = qMax(this->width() - 30, 40);
    } else {
        targetCardSize = 100; // 示例值，请根据实际情况调整
    }
    return QSize(targetCardSize, targetCardSize);
}


PlayerAreaWidget::PlayerAreaWidget(Player* player, bool isHBoxLayout, bool isLandMark, QWidget* parent,bool isSelf)
    : QWidget(parent),
    m_player(player),
    m_isHBoxLayout(isHBoxLayout),
    m_isLandMark(isLandMark),
    m_isSelf(isSelf)
{
    connect(m_player, &Player::cardAdded, this, &PlayerAreaWidget::onCardAdded);
    connect(m_player, &Player::cardDeled, this, &PlayerAreaWidget::onCardDeled);

    QVBoxLayout* topLevelLayout = new QVBoxLayout(this);
    topLevelLayout->setContentsMargins(0, 0, 0, 0);

    m_scrollArea = new QScrollArea(this);
    m_scrollArea->setWidgetResizable(true);
    m_scrollArea->setFrameShape(QFrame::NoFrame);
    m_scrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    m_scrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    m_scrollArea->viewport()->setAutoFillBackground(false);

    m_contentWidget = new QWidget();
    m_scrollArea->setWidget(m_contentWidget);

    if (isHBoxLayout) {
        m_cardLayout = new QHBoxLayout(m_contentWidget);
    } else {
        m_cardLayout = new QVBoxLayout(m_contentWidget);
    }
    m_cardLayout->setContentsMargins(10, 5, 10, 5);
    m_cardLayout->setSpacing(15);
    m_cardLayout->setAlignment(Qt::AlignCenter);

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

    connect(card, &Card::cardStateChanged, this, &PlayerAreaWidget::onCardStateChanged);

    // 检查是否已有同名卡槽
    for (SlotWidget* slotWidget : m_slots) {
        CardWidget* topCard = slotWidget->topCard();
        if (topCard && topCard->getCard() && topCard->getCard()->getName() == card->getName()) {
            slotWidget->pushCard(new CardWidget(card, ShowType::Ordinary, slotWidget));
            // 连接新推入的 CardWidget 的信号（通过 SlotWidget 转发）
            connect(slotWidget, &SlotWidget::cardWidgetRequestShowDetailed, this, &PlayerAreaWidget::handleSlotWidgetRequestShowDetailed);
            connect(slotWidget, &SlotWidget::cardWidgetRequestHideDetailed, this, &PlayerAreaWidget::handleSlotWidgetRequestHideDetailed);
            return;
        }
    }

    // 没找到同名卡槽，需要新建一个并找到插入位置
    int addIndex = 0;
    for (const auto& slotWidget : m_slots) {
        Card* existingCard = nullptr;
        CardWidget* topCardWidget = slotWidget->topCard();
        if (topCardWidget) {
            existingCard = topCardWidget->getCard();
        }
        if (existingCard && getOrderId(existingCard) > getOrderId(card)) {
            break;
        }
        addIndex++;
    }

    SlotWidget* newSlotWidget = new SlotWidget(false, Color::BackNone, m_contentWidget);
    newSlotWidget->pushCard(new CardWidget(card, ShowType::Ordinary, newSlotWidget));

    // 连接 newSlotWidget 的详细显示信号
    connect(newSlotWidget, &SlotWidget::cardWidgetRequestShowDetailed, this, &PlayerAreaWidget::handleSlotWidgetRequestShowDetailed);
    connect(newSlotWidget, &SlotWidget::cardWidgetRequestHideDetailed, this, &PlayerAreaWidget::handleSlotWidgetRequestHideDetailed);


    m_slots.insert(addIndex, newSlotWidget);
    m_cardLayout->insertWidget(addIndex, newSlotWidget);

    int targetCardSize = 0;
    if (!m_isLandMark) {
        if (m_isHBoxLayout)
            targetCardSize = qMax(this->height() - 30, 40);
        else
            targetCardSize = qMax(this->width() - 30, 40);
    } else {
        targetCardSize = 100; // 示例值，请根据实际情况调整
    }

    bool shouldBeInitiallyHidden = (m_isLandMark && !m_isSelf && card->getState() == State::Closing);

    if (shouldBeInitiallyHidden) {
        newSlotWidget->setFixedSize(targetCardSize, targetCardSize);
        newSlotWidget->hide();
        if (newSlotWidget->parentWidget() && newSlotWidget->parentWidget()->layout()) {
            newSlotWidget->parentWidget()->layout()->update();
        }
    } else {
        // 动画开始前，将最小和最大尺寸都设置为0
        newSlotWidget->setMinimumSize(0, 0);
        newSlotWidget->setMaximumSize(0, 0);
        newSlotWidget->show(); // 确保 widget 是可见的，这样动画才能被渲染

        // 创建并动画 minimumSize
        QPropertyAnimation* minSizeAnimation = new QPropertyAnimation(newSlotWidget, "minimumSize", this);
        minSizeAnimation->setDuration(300);
        minSizeAnimation->setStartValue(QSize(0, 0));
        minSizeAnimation->setEndValue(QSize(targetCardSize, targetCardSize));
        minSizeAnimation->setEasingCurve(QEasingCurve::OutQuad);

        // 创建并动画 maximumSize
        QPropertyAnimation* maxSizeAnimation = new QPropertyAnimation(newSlotWidget, "maximumSize", this);
        maxSizeAnimation->setDuration(300);
        maxSizeAnimation->setStartValue(QSize(0, 0));
        maxSizeAnimation->setEndValue(QSize(targetCardSize, targetCardSize));
        maxSizeAnimation->setEasingCurve(QEasingCurve::OutQuad);

        // 使用 QParallelAnimationGroup 同时启动这两个动画
        QParallelAnimationGroup* animationGroup = new QParallelAnimationGroup(this);
        animationGroup->addAnimation(minSizeAnimation);
        animationGroup->addAnimation(maxSizeAnimation);

        connect(animationGroup, &QParallelAnimationGroup::finished, this, [newSlotWidget, targetCardSize]() {
            // 动画结束后，设置固定的最终大小，这会覆盖 min/max size
            newSlotWidget->setFixedSize(targetCardSize, targetCardSize);
            // 此时可以重置 min/max size，或者让 setFixedSize 保持其效果
            // newSlotWidget->setMinimumSize(0, 0); // 恢复默认或一个合理值
            // newSlotWidget->setMaximumSize(QWIDGETSIZE_MAX, QWIDGETSIZE_MAX); // 恢复默认或一个合理值

            // 强制更新布局，确保所有卡槽位置正确
            if (newSlotWidget->parentWidget() && newSlotWidget->parentWidget()->layout()) {
                newSlotWidget->parentWidget()->layout()->update();
            }
        });

        animationGroup->start(QAbstractAnimation::DeleteWhenStopped); // 动画结束后自动删除
    }
}

void PlayerAreaWidget::onCardDeled(Player* player, Card* card)
{
    if (player != m_player) return;
    if (!card) return;
    if ((m_isLandMark && card->getType() != Type::Landmark) || (!m_isLandMark && card->getType() == Type::Landmark)) {
        return;
    }

    disconnect(card, &Card::cardStateChanged, this, &PlayerAreaWidget::onCardStateChanged);

    for (int i = 0; i < m_slots.size(); ++i) {
        SlotWidget* slotWidget = m_slots.at(i);
        CardWidget* topCard = slotWidget->topCard();
        if (topCard && topCard->getCard() && topCard->getCard()->getName() == card->getName()) {
            // 在popCard之前断开SlotWidget的信号，因为popCard内部会处理CardWidget的信号断开
            disconnect(slotWidget, &SlotWidget::cardWidgetRequestShowDetailed, this, &PlayerAreaWidget::handleSlotWidgetRequestShowDetailed);
            disconnect(slotWidget, &SlotWidget::cardWidgetRequestHideDetailed, this, &PlayerAreaWidget::handleSlotWidgetRequestHideDetailed);

            slotWidget->popCard();
            if (slotWidget->isEmpty()) {
                // TODO: 考虑删除动画，让卡槽平滑缩小并消失
                m_cardLayout->removeWidget(slotWidget);
                m_slots.removeAt(i);
                slotWidget->deleteLater();
            }
            return;
        }
    }
}

void PlayerAreaWidget::resizeEvent(QResizeEvent* event) {
    QWidget::resizeEvent(event);

    if(!m_isLandMark){
        int targetCardSize;
        if(m_isHBoxLayout)
            targetCardSize = qMax(this->height()-30,40);
        else
            targetCardSize = qMax(this->width()-30,40);

        for (SlotWidget* slotWidget : m_slots) {
            // 只有当卡槽不是正在动画时，才在 resizeEvent 中设置 fixed size
            // 否则动画会覆盖这里的设置
            // 这是一个简化，更严谨的做法是检查是否有动画正在运行
            if (!slotWidget->property("animatingSize").toBool()) { // 假设我们设置一个属性来标记
                slotWidget->setFixedSize(targetCardSize, targetCardSize);
            }
        }
    }
}

void PlayerAreaWidget::onCardStateChanged(Card* card,State state){
    if(m_isLandMark && !m_isSelf){
        for (SlotWidget* slotWidget : m_slots) {
            CardWidget* topCardWidget = slotWidget->topCard();
            if (topCardWidget && topCardWidget->getCard() == card) {
                if(state == State::Closing) {
                    slotWidget->hide();
                } else {
                    slotWidget->show();
                }
                break;
            }
        }
    }
}

// 处理内部 SlotWidget 的请求并转发
void PlayerAreaWidget::handleSlotWidgetRequestShowDetailed(Card* card, QPoint globalPos)
{
    emit cardWidgetRequestShowDetailed(card, globalPos);
}

void PlayerAreaWidget::handleSlotWidgetRequestHideDetailed()
{
    emit cardWidgetRequestHideDetailed();
}
