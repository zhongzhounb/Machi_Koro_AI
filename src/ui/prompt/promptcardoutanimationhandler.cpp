#include "PromptCardOutAnimationHandler.h"
#include "MainWindow.h"
#include "cards/slotwidget.h"
#include "Player.h"
#include "players/playerareawidget.h"
PromptCardOutAnimationHandler::PromptCardOutAnimationHandler(MainWindow* main)
    : PromptHandlerBase(main)
{
}

void PromptCardOutAnimationHandler::handle(const PromptData& pd)
{
    if (m_main->m_animationOverlayWidget)
        m_main->m_animationOverlayWidget->setAttribute(Qt::WA_TransparentForMouseEvents, true);

    // 若没有卡牌可动画，直接返回
    if (m_main->m_currentAnimatedInSlots.isEmpty()) {
        emit responseUserInput(pd.autoInput);
        return;
    }

    QParallelAnimationGroup* animationGroup = new QParallelAnimationGroup(m_main);

    QList<QPointer<SlotWidget>> slotsToAnimateOut = m_main->m_currentAnimatedInSlots;
    m_main->m_currentAnimatedInSlots.clear();

    int gameMainWidgetWidth = m_main->m_gameMainWidget->width();
    int gameMainWidgetHeight = m_main->m_gameMainWidget->height();

    for (QPointer<SlotWidget> slot_ptr : slotsToAnimateOut) {

        if (!slot_ptr)
            continue;

        SlotWidget* tempSlot = slot_ptr.data();
        Player* associatedPlayer = m_main->m_animatedSlotToPlayerMap.value(tempSlot);

        if (!associatedPlayer || !m_main->m_playerLayoutConfigs.contains(associatedPlayer)) {
            qWarning() << "CardOutAnimation: missing associated player/layout. Deleting slot.";
            tempSlot->deleteLater();
            continue;
        }

        const PlayerLayoutConfig& config =
            m_main->m_playerLayoutConfigs.value(associatedPlayer);

        // 卡牌当前大小（来自 CardInAnimation 最终大小）
        QSize currentCardSize = tempSlot->size();

        // 窗口外的目标中心点
        QPoint endCenterPoint =
            config.getOutOfWindowPos(gameMainWidgetWidth, gameMainWidgetHeight);

        // 根据卡牌尺寸计算左上角
        QPoint actualEndPoint(
            endCenterPoint.x() - currentCardSize.width() / 2,
            endCenterPoint.y() - currentCardSize.height() / 2
            );

        // 动画
        auto* posAnimation = new QPropertyAnimation(tempSlot, "pos");
        posAnimation->setDuration(700);
        posAnimation->setStartValue(tempSlot->pos());
        posAnimation->setEndValue(actualEndPoint);
        posAnimation->setEasingCurve(QEasingCurve::InQuad);

        animationGroup->addAnimation(posAnimation);

        // 卡槽动画完毕后删除自身
        connect(posAnimation, &QPropertyAnimation::finished,
                tempSlot, &SlotWidget::deleteLater);
    }

    // 所有卡牌处理后清理映射
    m_main->m_animatedSlotToPlayerMap.clear();

    connect(animationGroup, &QParallelAnimationGroup::finished,
            m_main, [this, pd]() {
                emit responseUserInput(pd.autoInput);
            });

    // 延迟1.5秒再启动动画
    QTimer::singleShot(1500, m_main, [animationGroup]() {
        animationGroup->start(QAbstractAnimation::DeleteWhenStopped);
    });
}
