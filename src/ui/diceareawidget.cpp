#include "diceareawidget.h"
#include "dice.h"
#include "dicewidget.h"
#include <QDebug> // 用于调试输出，如果需要

DiceAreaWidget::DiceAreaWidget(Dice* dice, QWidget* parent)
    : QWidget(parent),
    m_dice(dice),
    // 初始化 DiceAreaWidget 自身的顶层布局
    m_topLevelLayout(new QHBoxLayout(this)),
    // 初始化 AdaptiveLayoutWidget：水平方向，非滚动，父级为 DiceAreaWidget
    m_adaptiveLayoutWidget(new AdaptiveLayoutWidget(Qt::Horizontal, false, this)),
    m_diceWidget1(new DiceWidget(this)), // 将 DiceWidget 的父级设为 DiceAreaWidget，以便生命周期管理
    m_diceWidget2(new DiceWidget(this)), // 同上
    m_textLabel(new QLabel(this))        // 同上
{
    // 设置 DiceAreaWidget 自身的大小策略，使其能够被其父布局管理

    // 将 AdaptiveLayoutWidget 添加到 DiceAreaWidget 的顶层布局中
    // 这样 DiceAreaWidget 的父布局就可以控制 AdaptiveLayoutWidget 的大小
    m_topLevelLayout->addWidget(m_adaptiveLayoutWidget);

    m_adaptiveLayoutWidget->addWidget(m_diceWidget1, 1.0f); // 1.0f 表示 1:1 宽高比
    m_adaptiveLayoutWidget->addWidget(m_diceWidget2, 1.0f); // 1.0f 表示 1:1 宽高比
    m_adaptiveLayoutWidget->addWidget(m_textLabel, 1.0f);

    // 设置 QLabel 的对齐方式，使其内容居中
    m_textLabel->setAlignment(Qt::AlignCenter);
    // 可选：为 QLabel 添加一些样式，使其更易于观察其大小变化
    m_textLabel->setStyleSheet("QLabel { background-color: #e0e0e0; border: 1px solid #c0c0c0; padding: 5px; }");

    //setStyleSheet("QWidget {background: green;}");

    // 连接信号和槽
    connect(m_dice, &Dice::dicesChanged, this, &DiceAreaWidget::onDicesChanged);

    // 初始更新显示
    onDicesChanged(m_dice);
}

DiceAreaWidget::~DiceAreaWidget()
{

}

void DiceAreaWidget::onDicesChanged(Dice* dice)
{
    if (dice != m_dice) return;

    // 根据骰子数值更新 DiceWidget 的显示和可见性
    if (dice->getFirstNum() == 0) {
        m_diceWidget1->hide();
    } else {
        m_diceWidget1->setDiceNum(dice->getFirstNum());
        m_diceWidget1->show();
    }

    if (dice->getSecondNum() == 0) {
        m_diceWidget2->hide();
    } else {
        m_diceWidget2->setDiceNum(dice->getSecondNum());
        m_diceWidget2->show();
    }

    // 更新总和文本
    //m_textLabel->setText(QString("%1").arg(dice->getSum()));
    // 移除 m_textLabel->hide()。现在 AdaptiveLayoutWidget 会正确处理其大小，
    // 所以它应该始终可见，除非有其他逻辑需要隐藏它。
    m_textLabel->hide(); // 确保它在更新后显示出来
}
