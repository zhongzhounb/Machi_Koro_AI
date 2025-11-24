#include "PromptBuyCardAnimationHandler.h"
#include "MainWindow.h"
#include "card.h"
#include "cards/cardstoreareawidget.h"
#include "cards/slotwidget.h"
#include "cards/cardwidget.h"

PromptBuyCardAnimationHandler::PromptBuyCardAnimationHandler(MainWindow* main)
    : PromptHandlerBase(main)
{
}

void PromptBuyCardAnimationHandler::handle(const PromptData& pd)
{
    Player* buyer = pd.buyer;
    Card* cardToBuy = pd.card;
    int opId = pd.autoInput;

    QTimer::singleShot(0, m_main, [this, buyer, cardToBuy, opId]() {

        auto* overlay = m_main->m_animationOverlayWidget;
        auto* cardStoreArea = m_main->m_cardStoreArea;
        auto* gameMain = m_main->m_gameMainWidget;

        overlay->setAttribute(Qt::WA_TransparentForMouseEvents, true);

        if (!buyer || !cardToBuy || !cardStoreArea || !overlay) {
            qWarning() << "BuyCardAnimation (delayed): Invalid data or widgets.";
            return;
        }

        int gameMainWidth = gameMain->width();
        int gameMainHeight = gameMain->height();

        // 初始化所有玩家的窗口外坐标
        for (Player* p : m_main->m_playerLayoutConfigs.keys()) {
            const PlayerLayoutConfig& cfg = m_main->m_playerLayoutConfigs.value(p);
            m_main->m_playerOutOfWindowTargetPos.insert(
                p,
                cfg.getOutOfWindowPos(gameMainWidth, gameMainHeight)
                );
        }

        int cardPosInStore = -1;
        CardStore* sourceStore = m_main->findCardStoreForCard(cardToBuy, cardPosInStore);
        if (!sourceStore || cardPosInStore == -1) {
            qWarning() << "BuyCardAnimation: store not found for card" << cardToBuy->getName();
            //此处目前判定为正常状态，因为购买地标建筑没有动画效果，此类卡找不着正常
            responseUserInput(opId);
            return;
        }

        // --- 1. 坐标与尺寸计算 ---
        QPoint startPosInCardStoreArea =
            cardStoreArea->getStoreSlotCenterPos(sourceStore, cardPosInStore);

        QPoint startPosGlobal = cardStoreArea->mapToGlobal(startPosInCardStoreArea);
        QPoint startPosInOverlay = overlay->mapFromGlobal(startPosGlobal);

        QList<SlotWidget*> storeSlots =
            cardStoreArea->m_storeToSlotsMap.value(sourceStore);

        SlotWidget* sourceSlot =
            (cardPosInStore < storeSlots.size()) ? storeSlots.at(cardPosInStore) : nullptr;

        QSize startSize = sourceSlot ? sourceSlot->size() : QSize(80, 120);

        const PlayerLayoutConfig& buyerCfg =
            m_main->m_playerLayoutConfigs.value(buyer);

        QPoint outOfWindowPosInGameMain =
            m_main->m_playerOutOfWindowTargetPos.value(
                buyer,
                QPoint(gameMainWidth / 2, -200) // fallback
                );

        QPoint outOfWindowPosGlobal = gameMain->mapToGlobal(outOfWindowPosInGameMain);
        QPoint outOfWindowPosInOverlay = overlay->mapFromGlobal(outOfWindowPosGlobal);

        // --- 中间点 (显示位置) ---
        QPoint midPointInOverlay;
        if (buyerCfg.getDisplayPos) {
            QPoint displayPosInGameMain =
                buyerCfg.getDisplayPos(gameMainWidth, gameMainHeight);
            QPoint displayPosGlobal = gameMain->mapToGlobal(displayPosInGameMain);
            midPointInOverlay = overlay->mapFromGlobal(displayPosGlobal);
        } else {
            qWarning() << "BuyCardAnimation: getDisplayPos not set, using fallback.";
            midPointInOverlay =
                startPosInOverlay + (outOfWindowPosInOverlay - startPosInOverlay) / 4;
        }

        QSize midSize = QSize(startSize.width(), startSize.width() * 3 / 2) * 2;
        QSize endSize = midSize;

        QRect startRect(startPosInOverlay - QRect(QPoint(0,0), startSize).center(), startSize);
        QRect midRect(midPointInOverlay - QRect(QPoint(0,0), midSize).center(), midSize);
        QRect endRect(outOfWindowPosInOverlay - QRect(QPoint(0,0), endSize).center(), endSize);

        // --- 2. 创建动画卡牌 ---
        CardWidget* animCard = new CardWidget(cardToBuy, ShowType::Detailed, overlay);
        animCard->setAnimated(true);
        animCard->setGeometry(startRect);
        animCard->show();

        QGraphicsDropShadowEffect* shadow = new QGraphicsDropShadowEffect(animCard);
        shadow->setBlurRadius(15);
        shadow->setColor(QColor(0, 0, 0, 120));
        shadow->setOffset(4, 4);
        animCard->setGraphicsEffect(shadow);

        // --- 3. 动画序列 ---
        QSequentialAnimationGroup* group = new QSequentialAnimationGroup(animCard);

        QPropertyAnimation* anim1 = new QPropertyAnimation(animCard, "geometry");
        anim1->setDuration(400);
        anim1->setStartValue(startRect);
        anim1->setEndValue(midRect);
        anim1->setEasingCurve(QEasingCurve::OutQuad);

        QPauseAnimation* pause = new QPauseAnimation(1500);

        QPropertyAnimation* anim2 = new QPropertyAnimation(animCard, "geometry");
        anim2->setDuration(400);
        anim2->setEndValue(endRect);
        anim2->setEasingCurve(QEasingCurve::InQuad);

        group->addAnimation(anim1);
        group->addAnimation(pause);
        group->addAnimation(anim2);

        connect(
            group,
            &QSequentialAnimationGroup::finished,
            this,
            [this, animCard, opId]() {
                animCard->deleteLater();
                responseUserInput(opId);
            }
            );

        group->start(QAbstractAnimation::DeleteWhenStopped);
    });
}
