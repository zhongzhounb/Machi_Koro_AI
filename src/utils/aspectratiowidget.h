#ifndef ASPECTRATIOWIDGET_H
#define ASPECTRATIOWIDGET_H

#include <QWidget>
#include <QBoxLayout>
#include <QSpacerItem>

class AspectRatioWidget : public QWidget
{
    Q_OBJECT // 确保此宏存在！
public:
    explicit AspectRatioWidget(QWidget *widget, float targetWidth, float targetHeight, QWidget *parent = nullptr);

protected:
    void resizeEvent(QResizeEvent *event) override;

private:
    QBoxLayout *m_layout;
    QWidget *m_wrappedWidget;
    float m_targetAspectRatio;

    QSpacerItem *m_leadingSpacer;
    QSpacerItem *m_trailingSpacer;
};

#endif // ASPECTRATIOWIDGET_H
