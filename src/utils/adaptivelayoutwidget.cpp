#include "adaptivelayoutwidget.h"
#include <QResizeEvent>
#include <QDebug>
#include <utility> // 确保包含，用于 std::as_const

AdaptiveLayoutWidget::AdaptiveLayoutWidget(Qt::Orientation orientation, bool scrollable, QWidget *parent) :
    QWidget(parent),
    m_orientation(orientation),
    m_scrollable(scrollable),
    m_scrollArea(nullptr),
    m_contentWidget(nullptr),
    m_leadingSpacer(nullptr),
    m_trailingSpacer(nullptr)
{
    // 总是创建一个 mainLayout 作为 AdaptiveLayoutWidget 自身的顶层布局。
    // 使用 static_cast 明确转换 Qt::Orientation 到 QBoxLayout::Direction，以防旧版本Qt或特定编译器问题。
    QBoxLayout *mainLayout = new QBoxLayout(static_cast<QBoxLayout::Direction>(Qt::Vertical), this);
    mainLayout->setContentsMargins(0, 0, 0, 0);
    mainLayout->setSpacing(0);
    //setStyleSheet("QWidget {background: blue;}");

    if (m_scrollable) {
        // 滚动模式：
        m_contentWidget = new QWidget();
        // m_internalLayout 是 m_contentWidget 的顶层布局
        m_internalLayout = new QBoxLayout(static_cast<QBoxLayout::Direction>(m_orientation), m_contentWidget);
        m_internalLayout->setContentsMargins(0, 0, 0, 0);
        m_internalLayout->setSpacing(0);

        m_scrollArea = new QScrollArea(this);
        m_scrollArea->setWidgetResizable(true);
        m_scrollArea->setWidget(m_contentWidget);
        m_scrollArea->setHorizontalScrollBarPolicy(m_orientation == Qt::Horizontal ? Qt::ScrollBarAsNeeded : Qt::ScrollBarAlwaysOff);
        m_scrollArea->setVerticalScrollBarPolicy(m_orientation == Qt::Vertical ? Qt::ScrollBarAsNeeded : Qt::ScrollBarAlwaysOff);

        mainLayout->addWidget(m_scrollArea); // 将 QScrollArea 添加到 mainLayout
    } else {
        // 非滚动模式：m_internalLayout 是一个子布局，因此在构造时它没有父控件
        m_internalLayout = new QBoxLayout(static_cast<QBoxLayout::Direction>(m_orientation)); // **关键修改：移除父控件参数**
        m_internalLayout->setContentsMargins(0, 0, 0, 0);
        m_internalLayout->setSpacing(0);

        m_leadingSpacer = new QSpacerItem(0, 0, QSizePolicy::Expanding, QSizePolicy::Expanding);
        m_trailingSpacer = new QSpacerItem(0, 0, QSizePolicy::Expanding, QSizePolicy::Expanding);

        m_internalLayout->addItem(m_leadingSpacer);
        m_internalLayout->addItem(m_trailingSpacer);

        mainLayout->addLayout(m_internalLayout); // **关键修改：将 m_internalLayout 添加到 mainLayout**
    }

    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
}

AdaptiveLayoutWidget::~AdaptiveLayoutWidget()
{
    clearWidgets();
    // QBoxLayout 和 QScrollArea 会在父对象析构时自动删除
    // QSpacerItem 实例需要手动删除，因为它们是直接 new 出来的，并且可能在 clearWidgets 后布局不再管理它们
    if (m_leadingSpacer) {
        m_internalLayout->removeItem(m_leadingSpacer); // 确保从布局中移除
        delete m_leadingSpacer;
        m_leadingSpacer = nullptr;
    }
    if (m_trailingSpacer) {
        m_internalLayout->removeItem(m_trailingSpacer); // 确保从布局中移除
        delete m_trailingSpacer;
        m_trailingSpacer = nullptr;
    }
}

void AdaptiveLayoutWidget::addWidget(QWidget *child, float aspectRatio)
{
    if (!child) {
        qWarning() << "AdaptiveLayoutWidget::addWidget: Attempted to add a null widget.";
        return;
    }

    m_originalChildren.append(child);

    QWidget *itemToAdd = child;
    // 只有当 scrollable 为 true 或者在非滚动模式下明确要求保持比例时，才使用 AspectRatioWidget 包装
    if (m_scrollable || (aspectRatio > 0.0f)) {
        itemToAdd = new AspectRatioWidget(child, aspectRatio, 1.0f);
        itemToAdd->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    } else {
        child->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    }

    m_wrappedChildren.append(itemToAdd);

    // 移除可能存在的居中弹簧 (非滚动模式下，在添加子控件前移除)
    // 这样可以确保新添加的控件在弹簧之间
    if (!m_scrollable) {
        if (m_leadingSpacer && m_internalLayout->indexOf(m_leadingSpacer) != -1) {
            m_internalLayout->removeItem(m_leadingSpacer);
        }
        if (m_trailingSpacer && m_internalLayout->indexOf(m_trailingSpacer) != -1) {
            m_internalLayout->removeItem(m_trailingSpacer);
        }
    }

    // 将控件添加到内部布局中
    m_internalLayout->addWidget(itemToAdd);

    // 重新添加居中弹簧 (非滚动模式下，在添加子控件后重新添加)
    if (!m_scrollable) {
        if (m_leadingSpacer && m_internalLayout->indexOf(m_leadingSpacer) == -1) {
            m_internalLayout->insertItem(0, m_leadingSpacer);
        }
        if (m_trailingSpacer && m_internalLayout->indexOf(m_trailingSpacer) == -1) {
            m_internalLayout->addItem(m_trailingSpacer);
        }
    }

    updateGeometry();
}

void AdaptiveLayoutWidget::clearWidgets()
{
    for (QList<QWidget*>::const_iterator it = m_wrappedChildren.constBegin(); it != m_wrappedChildren.constEnd(); ++it) {
        QWidget *wrappedChild = *it;
        if (wrappedChild) {
            m_internalLayout->removeWidget(wrappedChild);
            // **修改：使用 dynamic_cast 替代 qobject_cast**
            if (dynamic_cast<AspectRatioWidget*>(wrappedChild)) {
                delete wrappedChild;
            }
        }
    }
    m_wrappedChildren.clear();
    m_originalChildren.clear();

    if (!m_scrollable) {
        if (m_leadingSpacer && m_internalLayout->indexOf(m_leadingSpacer) == -1) {
            m_internalLayout->insertItem(0, m_leadingSpacer);
        }
        if (m_trailingSpacer && m_internalLayout->indexOf(m_trailingSpacer) == -1) {
            m_internalLayout->addItem(m_trailingSpacer);
        }
        // **修改：使用 setStretch(index, stretch) **
        if (m_leadingSpacer) m_internalLayout->setStretch(m_internalLayout->indexOf(m_leadingSpacer), 1);
        if (m_trailingSpacer) m_internalLayout->setStretch(m_internalLayout->indexOf(m_trailingSpacer), 1);
    }

    updateGeometry();
}

void AdaptiveLayoutWidget::resizeEvent(QResizeEvent *event)
{
    QWidget::resizeEvent(event);

    if (!m_scrollable) {
        updateNonScrollableLayout(event->size());
    }
}

void AdaptiveLayoutWidget::updateNonScrollableLayout(const QSize& newSize)
{
    int numChildren = m_wrappedChildren.count();

    if (numChildren == 0) {
        // **修改：使用 setStretch(index, stretch) **
        if (m_leadingSpacer) m_internalLayout->setStretch(m_internalLayout->indexOf(m_leadingSpacer), 1);
        if (m_trailingSpacer) m_internalLayout->setStretch(m_internalLayout->indexOf(m_trailingSpacer), 1);
        m_internalLayout->invalidate();
        m_internalLayout->update();
        return;
    }

    for (int i = 0; i < m_internalLayout->count(); ++i) {
        QLayoutItem *item = m_internalLayout->itemAt(i);
        // 确保不是我们手动创建的居中弹簧
        if (item && item != m_leadingSpacer && item != m_trailingSpacer) {
            QWidget *widget = item->widget();
            if (widget) {
                // **关键修改：统一使用 setStretch(index, stretch) for widgets**
                m_internalLayout->setStretch(i, 1); // 设置伸展因子为1，让它们平分空间
                widget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding); // 确保子控件可以扩展
            } else if (item->layout()) {
                // 如果是嵌套布局
                m_internalLayout->setStretch(i, 1);
            } else if (item->spacerItem()) {
                // 如果是其他 QSpacerItem (不是我们手动创建的居中弹簧)
                m_internalLayout->setStretch(i, 0); // 默认不伸展，除非有特定需求
            }
        }
    }

    // 调整我们手动创建的居中弹簧的伸缩因子
    if (m_leadingSpacer) m_internalLayout->setStretch(m_internalLayout->indexOf(m_leadingSpacer), 1);
    if (m_trailingSpacer) m_internalLayout->setStretch(m_internalLayout->indexOf(m_trailingSpacer), 1);

    m_internalLayout->invalidate();
    m_internalLayout->update();
}
