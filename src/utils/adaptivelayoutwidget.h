#ifndef ADAPTIVELAYOUTWIDGET_H
#define ADAPTIVELAYOUTWIDGET_H

#include <QWidget>
#include <QBoxLayout>
#include <QScrollArea>
#include <QSpacerItem>
#include <QList>

// 包含之前定义的 AspectRatioWidget，它是 AdaptiveLayoutWidget 的一个组件
#include "aspectratiowidget.h"

class AdaptiveLayoutWidget : public QWidget
{
    Q_OBJECT // 添加 Q_OBJECT 宏
public:
    // 构造函数：
    // orientation: 布局方向 (Qt::Vertical 或 Qt::Horizontal)
    // scrollable: 是否允许内容超出时出现滚动条
    explicit AdaptiveLayoutWidget(Qt::Orientation orientation, bool scrollable, QWidget *parent = nullptr);
    ~AdaptiveLayoutWidget(); // 析构函数，用于清理资源

    // 添加子控件
    // child: 要添加的 QWidget
    // aspectRatio: 仅在 scrollable 为 true 或在非滚动模式下需要强制比例时有效。
    //              如果 <= 0.0f，则表示不强制比例，子控件会平分空间并拉伸。
    //              例如：1.0f 表示 1:1 比例，0.5f 表示 1:2 比例
    void addWidget(QWidget *child, float aspectRatio = 0.0f);

    // 移除所有子控件
    void clearWidgets();

protected:
    // 当控件尺寸改变时触发，用于调整内部布局策略
    void resizeEvent(QResizeEvent *event) override;

private:
    Qt::Orientation m_orientation; // 布局方向
    bool m_scrollable;             // 是否启用滚动

    QBoxLayout *m_internalLayout; // 实际放置子控件的布局
    QScrollArea *m_scrollArea;    // 如果 m_scrollable 为 true，则使用滚动区域
    QWidget *m_contentWidget;     // 如果 m_scrollable 为 true，QScrollArea 的内容控件

    QList<QWidget*> m_originalChildren; // 存储用户添加的原始子控件
    QList<QWidget*> m_wrappedChildren;  // 存储实际添加到布局中的控件（可能是 AspectRatioWidget 或原始控件）

    // 用于非滚动模式下的居中和填充的弹簧
    QSpacerItem *m_leadingSpacer;
    QSpacerItem *m_trailingSpacer;

    // 辅助函数：更新非滚动模式下的布局逻辑
    void updateNonScrollableLayout(const QSize& newSize);
};

#endif // ADAPTIVELAYOUTWIDGET_H
