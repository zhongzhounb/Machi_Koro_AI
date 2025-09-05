#ifndef DICEAREAWIDGET_H
#define DICEAREAWIDGET_H

#include <QWidget>
#include <QHBoxLayout>
#include <QLabel>
#include "dice.h" // 假设 Dice.h 存在并已正确定义
#include "dicewidget.h" // 假设 DiceWidget.h 存在并已正确定义

class DiceAreaWidget : public QWidget
{
    Q_OBJECT
public:
    explicit DiceAreaWidget(Dice* dice, QWidget* parent = nullptr);
    ~DiceAreaWidget();

public slots:
    void onDicesChanged(Dice* dice);

private:
    Dice* m_dice;
    QHBoxLayout* m_mainLayout;
    DiceWidget* m_diceWidget1;
    DiceWidget* m_diceWidget2;
};

#endif // DICEAREAWIDGET_H
