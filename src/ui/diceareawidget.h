#ifndef DICEAREAWIDGET_H
#define DICEAREAWIDGET_H
#include <QFrame>
#include <QLabel>
#include <QMouseEvent>
#include <QStackedLayout>
#include <QFont>
#include <QResizeEvent> // 需要包含此头文件
class DiceWidget;
class Dice;
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
    QLabel* m_textLabel;
};


#endif // DICEAREAWIDGET_H
