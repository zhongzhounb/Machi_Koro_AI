#include "PromptCardInAnimationHandler.h"
#include "MainWindow.h"
#include "cards/slotwidget.h"
#include "cards/cardwidget.h"
#include "Player.h"
#include "players/playerareawidget.h"

PromptCardInAnimationHandler::PromptCardInAnimationHandler(MainWindow* main)
    : PromptHandlerBase(main)
{
}

void PromptCardInAnimationHandler::handle(const PromptData& pd)
{
    if (m_main->m_animationOverlayWidget)
        m_main->m_animationOverlayWidget->setAttribute(Qt::WA_TransparentForMouseEvents, true);

    Player* targetPlayer = pd.buyer;
    if (!targetPlayer) {
        qWarning() << "CardInAnimation: No target player (pd.buyer).";
        emit responseUserInput(pd.autoInput);
        return;
    }

    if (!m_main->m_playerLayoutConfigs.contains(targetPlayer)) {
        qWarning() << "CardInAnimation: layout config not found for target player.";
        emit responseUserInput(pd.autoInput);
        return;
    }

    const PlayerLayoutConfig& config = m_main->m_playerLayoutConfigs.value(targetPlayer);

    int gameMainWidgetWidth = m_main->m_gameMainWidget->width();
    int gameMainWidgetHeight = m_main->m_gameMainWidget->height();

    QPoint startCenterPoint = config.getOutOfWindowPos(gameMainWidgetWidth, gameMainWidgetHeight);
    QPoint endCenterPoint   = config.getDisplayPos(gameMainWidgetWidth, gameMainWidgetHeight);

    QSize targetCardSize(
        static_cast<int>(gameMainWidgetWidth * 0.15),
        static_cast<int>(gameMainWidgetWidth * 0.15 * 3 / 2)
        );

    QPoint actualStartPoint(
        startCenterPoint.x() - targetCardSize.width()  / 2,
        startCenterPoint.y() - targetCardSize.height() / 2
        );

    QPoint actualEndPoint(
        endCenterPoint.x() - targetCardSize.width()  / 2,
        endCenterPoint.y() - targetCardSize.height() / 2
        );

    // ========== 创建动画组 ==========
    QParallelAnimationGroup* animationGroup = new QParallelAnimationGroup(m_main);

    SlotWidget* tempSlot = new SlotWidget(false, Color::BackNone, m_main->m_animationOverlayWidget);

    for (Card* card : pd.cards)
        tempSlot->pushCard(new CardWidget(card, ShowType::Detailed, tempSlot));

    tempSlot->setFixedSize(targetCardSize);
    tempSlot->hide();

    tempSlot->move(actualStartPoint);
    tempSlot->show();

    // 位置动画
    auto* posAnimation = new QPropertyAnimation(tempSlot, "pos");
    posAnimation->setDuration(700);
    posAnimation->setStartValue(actualStartPoint);
    posAnimation->setEndValue(actualEndPoint);
    posAnimation->setEasingCurve(QEasingCurve::OutQuad);

    animationGroup->addAnimation(posAnimation);

    // 记录动画中的卡槽
    m_main->m_currentAnimatedInSlots.append(tempSlot);
    m_main->m_animatedSlotToPlayerMap.insert(tempSlot, targetPlayer);

    connect(animationGroup, &QParallelAnimationGroup::finished, m_main, [this, pd]() {
        emit responseUserInput(pd.autoInput);
    });

    animationGroup->start(QAbstractAnimation::DeleteWhenStopped);
}
