#include "aspectratiowidget.h"
#include <QResizeEvent>
#include <QDebug>
#include <QBoxLayout>
AspectRatioWidget::AspectRatioWidget(QWidget *widget, float targetWidth, float targetHeight, QWidget *parent) :
    QWidget(parent),
    m_wrappedWidget(widget),
    m_targetAspectRatio(targetWidth / targetHeight)
{
    m_layout = new QBoxLayout(static_cast<QBoxLayout::Direction>(Qt::Horizontal), this);
    m_layout->setContentsMargins(0, 0, 0, 0);
    m_layout->setSpacing(0);

    m_leadingSpacer = new QSpacerItem(0, 0, QSizePolicy::Expanding, QSizePolicy::Minimum);
    m_trailingSpacer = new QSpacerItem(0, 0, QSizePolicy::Expanding, QSizePolicy::Minimum);

    m_layout->addItem(m_leadingSpacer);
    m_layout->addWidget(m_wrappedWidget);
    m_layout->addItem(m_trailingSpacer);

    m_wrappedWidget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
}

void AspectRatioWidget::resizeEvent(QResizeEvent *event)
{
    QWidget::resizeEvent(event);

    if (event->size().height() == 0 || m_targetAspectRatio == 0 || m_wrappedWidget == nullptr) {
        return;
    }

    float currentAspectRatio = (float)event->size().width() / event->size().height();

    int wrappedWidgetWidth, wrappedWidgetHeight;

    if (currentAspectRatio > m_targetAspectRatio)
    {
        wrappedWidgetHeight = event->size().height();
        wrappedWidgetWidth = qRound(wrappedWidgetHeight * m_targetAspectRatio);

        m_layout->setDirection(QBoxLayout::LeftToRight);
        m_leadingSpacer->changeSize(1, 0, QSizePolicy::Expanding, QSizePolicy::Minimum);
        m_trailingSpacer->changeSize(1, 0, QSizePolicy::Expanding, QSizePolicy::Minimum);
    }
    else
    {
        wrappedWidgetWidth = event->size().width();
        wrappedWidgetHeight = qRound(wrappedWidgetWidth / m_targetAspectRatio);

        m_layout->setDirection(QBoxLayout::TopToBottom);
        m_leadingSpacer->changeSize(0, 1, QSizePolicy::Minimum, QSizePolicy::Expanding);
        m_trailingSpacer->changeSize(0, 1, QSizePolicy::Minimum, QSizePolicy::Expanding);
    }

    m_wrappedWidget->setFixedSize(wrappedWidgetWidth, wrappedWidgetHeight);

    m_layout->invalidate();
    m_layout->update();
}
