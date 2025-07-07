#include "aspectratiowidget.h"
#include <QResizeEvent>

AspectRatioWidget::AspectRatioWidget(QWidget *widget, float width, float height, QWidget *parent) :
    QWidget(parent),
    arWidth(width),
    arHeight(height)
{
    layout = new QBoxLayout(QBoxLayout::LeftToRight, this);
    layout->addItem(new QSpacerItem(0, 0));
    layout->addWidget(widget);
    layout->addItem(new QSpacerItem(0, 0));
}

void AspectRatioWidget::resizeEvent(QResizeEvent *event)
{
    qDebug()<<event->size().width();
    qDebug()<<event->size().height();
    float thisAspectRatio = (float)event->size().width() / event->size().height();
    int widgetStretch, outerStretch;

    if (thisAspectRatio > (arWidth/arHeight))
    {
        // 太宽了 --> 调整为水平布局 --> 在左右两侧的弹簧将控件宽度挤回正常比例
        layout->setDirection(QBoxLayout::LeftToRight);
        widgetStretch = height() * (arWidth/arHeight);
        outerStretch = (width() - widgetStretch) / 2 + 0.5;
    }
    else
    {
        // 太高了 --> 调整为垂直布局 --> 在上下两侧的弹簧将控件高度挤回正常比例
        layout->setDirection(QBoxLayout::TopToBottom);
        widgetStretch = width() * (arHeight/arWidth);
        outerStretch = (height() - widgetStretch) / 2 + 0.5;
    }

    layout->setStretch(0, outerStretch);
    layout->setStretch(1, widgetStretch);
    layout->setStretch(2, outerStretch);
}
