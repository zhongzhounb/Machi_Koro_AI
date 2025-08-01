#ifndef BUTTONAREAWIDGET_H
#define BUTTONAREAWIDGET_H

#include <QWidget>
#include <QHBoxLayout> // 仍然需要，用于 DiceAreaWidget 自身的顶层布局
#include <QLabel>
#include "global.h"
#include "buttonwidget.h"
#include"mainwindow.h"

class ButtonAreaWidget : public QWidget
{
    Q_OBJECT
public:
    explicit ButtonAreaWidget(MainWindow* w,QWidget* parent = nullptr);
    ~ButtonAreaWidget(){};

    void addButton(OptionData op);
    void clearButtons();

private:
    // DiceAreaWidget 自身的顶层布局，用于包含 AdaptiveLayoutWidget
    QHBoxLayout* m_mainLayout;
    QList<ButtonWidget*> m_buttons;
};



#endif // BUTTONAREAWIDGET_H
