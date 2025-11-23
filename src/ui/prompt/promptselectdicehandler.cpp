#include "PromptSelectDiceHandler.h"
#include "MainWindow.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QGraphicsDropShadowEffect>
#include <QPropertyAnimation>
#include <QPauseAnimation>
#include <QEvent>
#include "dice.h"
#include "dices/diceareawidget.h"

PromptSelectDiceHandler::PromptSelectDiceHandler(MainWindow* main)
    : PromptHandlerBase(main)
{}

// 辅助类：用于处理按钮的鼠标进入和离开事件，以触发骰子动画
// IMPORTANT: 这个类现在将持有并管理骰子动画的状态变量
class DiceOptionEventFilter : public QObject {
public:
    // 构造函数现在接受 animatingDice 和 rollAnimationTimer 的 QPointer
    DiceOptionEventFilter(QObject* parent, QPointer<Dice> animatingDice, QPointer<QTimer> rollAnimationTimer)
        : QObject(parent),
        m_animatingDice(animatingDice),
        m_rollAnimationTimer(rollAnimationTimer),
        m_currentRollIndex1(0),
        m_currentRollIndex2(0),
        m_diceToAnimateCount(0)
    {
        // 连接计时器的 timeout 信号，用于更新骰子数字
        // lambda 捕获 'this'，并直接访问成员变量 (QPointer会自动处理空值)
        connect(m_rollAnimationTimer, &QTimer::timeout, this, [this]() {
            if (!m_animatingDice) return; // QPointer 检查

            const QList<int> rollSequence1 = {1, 3, 5, 2, 4, 6};
            const QList<int> rollSequence2 = {2, 4, 6, 1, 3, 5};

            if (m_diceToAnimateCount > 0) {
                m_animatingDice->setFirstNum(rollSequence1.at(m_currentRollIndex1));
                m_currentRollIndex1 = (m_currentRollIndex1 + 1) % rollSequence1.size();

                if (m_diceToAnimateCount == 2) {
                    m_animatingDice->setSecondNum(rollSequence2.at(m_currentRollIndex2));
                    m_currentRollIndex2 = (m_currentRollIndex2 + 1) % rollSequence2.size();
                } else {
                    m_animatingDice->setSecondNum(0);
                }
            }
        });
    }

    // eventFilter 是一个 const 方法，所以它调用的成员函数也必须是 const-callable
    bool eventFilter(QObject *obj, QEvent *event) override {
        QPushButton* button = qobject_cast<QPushButton*>(obj);
        // 仅处理 QPushButton 且按钮必须是启用的
        if (!button || !button->isEnabled()) {
            return QObject::eventFilter(obj, event);
        }

        if (event->type() == QEvent::Enter) {
            int optionId = button->property("optionId").toInt(); // 获取按钮存储的选项ID
            if (optionId == 1) { // 抛一个骰子
                startDiceAnimation(1);
            } else if (optionId == 2) { // 抛两个骰子
                startDiceAnimation(2);
            }
        } else if (event->type() == QEvent::Leave) {
            stopDiceAnimation(); // 停止动画并重置骰子
        }
        return QObject::eventFilter(obj, event);
    }

private:
    // 这些是 DiceOptionEventFilter 的私有成员函数，它们可以修改 DiceOptionEventFilter 的状态
    void startDiceAnimation(int numDice) {
        m_diceToAnimateCount = numDice;
        m_currentRollIndex1 = 0; // 重置索引
        m_currentRollIndex2 = 0; // 重置索引
        if (m_rollAnimationTimer) { // QPointer 检查
            m_rollAnimationTimer->start();
        }
    }

    void stopDiceAnimation() {
        if (m_rollAnimationTimer) { // QPointer 检查
            m_rollAnimationTimer->stop();
        }
        m_diceToAnimateCount = 0; // 重置计数
        if (m_animatingDice) { // QPointer 检查
            m_animatingDice->setFirstNum(1);
            m_animatingDice->setSecondNum(0);
        }
    }

    QPointer<Dice> m_animatingDice; // 更改为 QPointer
    QPointer<QTimer> m_rollAnimationTimer; // 更改为 QPointer
    int m_currentRollIndex1;
    int m_currentRollIndex2;
    int m_diceToAnimateCount;
};

void PromptSelectDiceHandler::handle(const PromptData& pd)
{
    int opId = pd.autoInput;
    QString promptMessage = pd.promptMessage;
    QList<OptionData> options = pd.options;
    int delay = pd.delay;

    // ===== 自动输入 =====
    if (pd.isAutoInput) {
        QTimer::singleShot(500, m_main, [this, opId]() {
            if (m_main->m_animationOverlayWidget)
                m_main->m_animationOverlayWidget->setAttribute(Qt::WA_TransparentForMouseEvents, true);

            emit responseUserInput(opId);
        });
        return;
    }

    // ===== 交互式选择 =====
    QTimer::singleShot(0, m_main, [this, opId, promptMessage, options, delay]() {

        if (!m_main->m_animationOverlayWidget) {
            qWarning() << "SelectDice (delayed): Animation overlay widget is null.";
            emit responseUserInput(opId);
            return;
        }

        // 激活交互 UI
        m_main->m_isInteractivePromptActive = true;
        m_main->m_animationOverlayWidget->setAttribute(Qt::WA_TransparentForMouseEvents, false);
        m_main->m_animationOverlayWidget->show();

        // ===== 容器 UI =====
        QWidget* selectDiceContainer = new QWidget(m_main->m_animationOverlayWidget);
        selectDiceContainer->setStyleSheet("background-color: rgba(0, 0, 0, 180); border-radius: 10px;");
        selectDiceContainer->setWindowOpacity(0.0);

        QSize overlaySize = m_main->m_animationOverlayWidget->size();
        int containerWidth = overlaySize.width() * 0.6;
        int containerHeight = overlaySize.height() * 0.5;

        QRect containerRect(
            QPoint((overlaySize.width() - containerWidth) / 2,
                   (overlaySize.height() - containerHeight) / 2),
            QSize(containerWidth, containerHeight)
            );
        selectDiceContainer->setGeometry(containerRect);

        QVBoxLayout* mainLayout = new QVBoxLayout(selectDiceContainer);
        mainLayout->setAlignment(Qt::AlignCenter);
        mainLayout->setContentsMargins(20, 20, 20, 20);
        mainLayout->setSpacing(20);

        // ===== 标题 =====
        QLabel* messageLabel = new QLabel(promptMessage, selectDiceContainer);
        messageLabel->setStyleSheet("color: white; font-size: 24px; font-weight: bold;");
        messageLabel->setAlignment(Qt::AlignCenter);
        mainLayout->addWidget(messageLabel);

        // ===== 骰子 + 阴影 =====
        Dice* animatingDice = new Dice(nullptr);
        animatingDice->setFirstNum(1);
        animatingDice->setSecondNum(0);

        DiceAreaWidget* animatingDiceAreaWidget =
            new DiceAreaWidget(animatingDice, selectDiceContainer);
        animatingDice->setParent(animatingDiceAreaWidget);

        animatingDiceAreaWidget->setFixedSize(containerWidth * 0.4, containerHeight * 0.4);
        mainLayout->addWidget(animatingDiceAreaWidget, 0, Qt::AlignCenter);

        QGraphicsDropShadowEffect *shadowEffect = new QGraphicsDropShadowEffect(animatingDiceAreaWidget);
        shadowEffect->setBlurRadius(25);
        shadowEffect->setColor(QColor(0, 0, 0, 150));
        shadowEffect->setOffset(6, 6);
        animatingDiceAreaWidget->setGraphicsEffect(shadowEffect);

        animatingDiceAreaWidget->show();

        // ===== 骰子滚动计时器 =====
        QTimer* rollAnimationTimer = new QTimer(selectDiceContainer);
        rollAnimationTimer->setInterval(70);

        DiceOptionEventFilter* diceEventFilter =
            new DiceOptionEventFilter(selectDiceContainer,
                                      QPointer<Dice>(animatingDice),
                                      QPointer<QTimer>(rollAnimationTimer));

        // ===== 选项按钮 =====
        QHBoxLayout* optionsLayout = new QHBoxLayout();
        optionsLayout->setAlignment(Qt::AlignCenter);
        mainLayout->addLayout(optionsLayout);

        // ===== 清理 + 回调 =====
        auto finalCleanupAndRespond =
            [this,
             selectDiceContainer_ptr = QPointer<QWidget>(selectDiceContainer),
             rollAnimationTimer_ptr = QPointer<QTimer>(rollAnimationTimer)]
            (int selectedId)
        {
            if (rollAnimationTimer_ptr)
                rollAnimationTimer_ptr->stop();

            if (!selectDiceContainer_ptr) {
                if (m_main->m_animationOverlayWidget)
                    m_main->m_animationOverlayWidget->setAttribute(Qt::WA_TransparentForMouseEvents, true);
                m_main->m_isInteractivePromptActive = false;
                emit responseUserInput(selectedId);
                return;
            }

            QPropertyAnimation* fadeOutAnim =
                new QPropertyAnimation(selectDiceContainer_ptr, "windowOpacity", selectDiceContainer_ptr);

            fadeOutAnim->setDuration(300);
            fadeOutAnim->setStartValue(selectDiceContainer_ptr->windowOpacity());
            fadeOutAnim->setEndValue(0.0);
            fadeOutAnim->setEasingCurve(QEasingCurve::InQuad);

            QObject::connect(fadeOutAnim, &QPropertyAnimation::finished,
                             selectDiceContainer_ptr,
                             [this, selectDiceContainer_ptr, selectedId]() {

                                 if (selectDiceContainer_ptr)
                                     selectDiceContainer_ptr->deleteLater();

                                 if (m_main->m_animationOverlayWidget)
                                     m_main->m_animationOverlayWidget->setAttribute(Qt::WA_TransparentForMouseEvents, true);

                                 m_main->m_isInteractivePromptActive = false;
                                 emit responseUserInput(selectedId);
                             });

            fadeOutAnim->start(QAbstractAnimation::DeleteWhenStopped);
        };

        // ===== 生成每个按钮 =====
        for (const OptionData& option : options) {
            QPushButton* button = new QPushButton(option.name, selectDiceContainer);

            button->setFixedSize(containerWidth * 0.3, containerHeight * 0.15);
            QFont font = button->font();
            font.setPointSize(16);
            button->setFont(font);
            button->setCursor(Qt::PointingHandCursor);
            button->setProperty("optionId", option.id);

            if (option.state == 0) {
                button->setEnabled(false);
                button->setStyleSheet(
                    "QPushButton { background-color: #555; color: #aaa; border: 1px solid #777; border-radius: 5px; }"
                    "QPushButton:disabled { background-color: #555; color: #aaa; }"
                    );
            } else {
                button->setStyleSheet(
                    "QPushButton { background-color: #4CAF50; color: white; border: 1px solid #388E3C; border-radius: 5px; }"
                    "QPushButton:hover { background-color: #66BB6A; }"
                    "QPushButton:pressed { background-color: #388E3C; }"
                    );

                QObject::connect(button, &QPushButton::clicked,
                                 m_main, [finalCleanupAndRespond, option]() {
                                     finalCleanupAndRespond(option.id);
                                 });

                button->installEventFilter(diceEventFilter);
            }

            optionsLayout->addWidget(button);
        }

        // ===== 渐入动画 =====
        QPropertyAnimation* initialFadeInAnim =
            new QPropertyAnimation(selectDiceContainer, "windowOpacity", selectDiceContainer);

        initialFadeInAnim->setDuration(300);
        initialFadeInAnim->setStartValue(0.0);
        initialFadeInAnim->setEndValue(1.0);
        initialFadeInAnim->setEasingCurve(QEasingCurve::OutQuad);
        initialFadeInAnim->start(QAbstractAnimation::DeleteWhenStopped);

        selectDiceContainer->show();
    });
}
