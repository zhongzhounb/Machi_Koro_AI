#ifndef ASPECTRATIOWIDGET_H
#define ASPECTRATIOWIDGET_H

#include <QWidget>
#include <QBoxLayout>

class AspectRatioWidget : public QWidget
{
public:
    AspectRatioWidget(QWidget *widget, float width, float height, QWidget *parent = 0);
    void resizeEvent(QResizeEvent *event);

private:
    QBoxLayout *layout;
    float arWidth;
    float arHeight;
};

#endif // ASPECTRATIOWIDGET_H
