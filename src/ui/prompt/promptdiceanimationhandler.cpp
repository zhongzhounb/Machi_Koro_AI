#include "PromptDiceAnimationHandler.h"
#include "MainWindow.h"
#include "dice.h"
#include "dices/diceareawidget.h"
#include <QSequentialAnimationGroup>
#include <QGraphicsDropShadowEffect>
#include <QPropertyAnimation>
#include <QPauseAnimation>
#include <QRandomGenerator>

PromptDiceAnimationHandler::PromptDiceAnimationHandler(MainWindow* main)
    : PromptHandlerBase(main)
{}

void PromptDiceAnimationHandler::handle(const PromptData& pd)
{
    QList<int> diceNumbers = pd.diceNum;
    int opId = pd.autoInput;

    QTimer::singleShot(0, m_main, [this, diceNumbers, opId]() {
        if (m_main->m_animationOverlayWidget)
            m_main->m_animationOverlayWidget->setAttribute(Qt::WA_TransparentForMouseEvents, true);

        if (diceNumbers.isEmpty() || !m_main->m_animationOverlayWidget) {
            qWarning() << "DiceAnimation (delayed): Invalid data or widgets.";
            emit responseUserInput(opId);
            return;
        }

        // 1. 创建 Dice 和 DiceAreaWidget
        Dice* animatingDice = new Dice(nullptr);
        animatingDice->setFirstNum(0);
        animatingDice->setSecondNum(0);

        DiceAreaWidget* animatingDiceAreaWidget =
            new DiceAreaWidget(animatingDice, m_main->m_animationOverlayWidget);

        animatingDice->setParent(animatingDiceAreaWidget);

        // 2. 计算置中区域
        QSize overlaySize = m_main->m_animationOverlayWidget->size();
        int targetWidth = static_cast<int>(overlaySize.width() * 0.4);
        int targetHeight = static_cast<int>(overlaySize.height() * 0.3);
        QRect targetRect(
            QPoint((overlaySize.width() - targetWidth) / 2,
                   (overlaySize.height() - targetHeight) / 2),
            QSize(targetWidth, targetHeight)
            );
        animatingDiceAreaWidget->setGeometry(targetRect);

        // 3. 阴影
        QGraphicsDropShadowEffect* shadowEffect = new QGraphicsDropShadowEffect(animatingDiceAreaWidget);
        shadowEffect->setBlurRadius(25);
        shadowEffect->setColor(QColor(0, 0, 0, 150));
        shadowEffect->setOffset(6, 6);
        animatingDiceAreaWidget->setGraphicsEffect(shadowEffect);

        animatingDiceAreaWidget->show();

        // 4. 动画序列
        QSequentialAnimationGroup* sequentialGroup = new QSequentialAnimationGroup(animatingDiceAreaWidget);

        // 渐入
        QPropertyAnimation* fadeInAnim = new QPropertyAnimation(animatingDiceAreaWidget, "windowOpacity");
        fadeInAnim->setDuration(300);
        fadeInAnim->setStartValue(0.0);
        fadeInAnim->setEndValue(1.0);
        fadeInAnim->setEasingCurve(QEasingCurve::OutQuad);
        sequentialGroup->addAnimation(fadeInAnim);

        // 骰子滚动
        const int rollDuration = 1200;
        const int rollInterval = 70;

        QTimer* rollTimer = new QTimer(animatingDiceAreaWidget);
        QPointer<Dice> animatingDice_ptr = animatingDice;

        QObject::connect(rollTimer, &QTimer::timeout, m_main, [animatingDice_ptr, diceNumbers]() {
            if (!animatingDice_ptr) return;

            animatingDice_ptr->setFirstNum(QRandomGenerator::global()->bounded(1, 7));
            if (diceNumbers.size() > 1)
                animatingDice_ptr->setSecondNum(QRandomGenerator::global()->bounded(1, 7));
            else
                animatingDice_ptr->setSecondNum(0);
        });

        QPauseAnimation* rollingPause = new QPauseAnimation(rollDuration);
        sequentialGroup->addAnimation(rollingPause);

        QObject::connect(sequentialGroup, &QSequentialAnimationGroup::currentAnimationChanged,
                         m_main, [rollTimer, rollInterval, rollingPause](QAbstractAnimation* current) {
                             if (current == rollingPause) rollTimer->start(rollInterval);
                             else rollTimer->stop();
                         });

        QObject::connect(rollingPause, &QPauseAnimation::finished, m_main,
                         [animatingDice_ptr, diceNumbers]() {
                             if (!animatingDice_ptr) return;

                             animatingDice_ptr->setFirstNum(diceNumbers.at(0));
                             if (diceNumbers.size() > 1)
                                 animatingDice_ptr->setSecondNum(diceNumbers.at(1));
                             else
                                 animatingDice_ptr->setSecondNum(0);
                         });

        // 停顿
        QPauseAnimation* settlePause = new QPauseAnimation(500);
        sequentialGroup->addAnimation(settlePause);

        // 渐出
        QPropertyAnimation* fadeOutAnim = new QPropertyAnimation(animatingDiceAreaWidget, "windowOpacity");
        fadeOutAnim->setDuration(500);
        fadeOutAnim->setStartValue(1.0);
        fadeOutAnim->setEndValue(0.0);
        fadeOutAnim->setEasingCurve(QEasingCurve::InQuad);
        sequentialGroup->addAnimation(fadeOutAnim);

        // 清理
        QObject::connect(sequentialGroup, &QSequentialAnimationGroup::finished,
                         m_main, [this, animatingDiceAreaWidget, opId]() {
                             animatingDiceAreaWidget->deleteLater();
                             emit responseUserInput(opId);
                         });

        sequentialGroup->start(QAbstractAnimation::DeleteWhenStopped);
    });
}
