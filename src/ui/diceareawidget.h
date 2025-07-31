#ifndef DICEAREAWIDGET_H
#define DICEAREAWIDGET_H

#include <QWidget>
#include <QHBoxLayout> // 仍然需要，用于 DiceAreaWidget 自身的顶层布局
#include <QLabel>

#include "dice.h"         // 假设 Dice 类存在
#include "dicewidget.h"   // 假设 DiceWidget 类存在
#include "adaptivelayoutwidget.h" // 引入 AdaptiveLayoutWidget

class DiceAreaWidget : public QWidget
{
    Q_OBJECT
public:
    explicit DiceAreaWidget(Dice* dice, QWidget* parent = nullptr);
    ~DiceAreaWidget();

private slots:
    void onDicesChanged(Dice* dice);

private:
    Dice* m_dice;

    // DiceAreaWidget 自身的顶层布局，用于包含 AdaptiveLayoutWidget
    QHBoxLayout* m_topLevelLayout;

    // 负责管理骰子和文本标签的布局和缩放
    AdaptiveLayoutWidget* m_adaptiveLayoutWidget;

    DiceWidget* m_diceWidget1;
    DiceWidget* m_diceWidget2;
    QLabel* m_textLabel;
};

#endif // DICEAREAWIDGET_H
