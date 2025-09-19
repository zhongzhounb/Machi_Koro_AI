#include "roundedvideoitem.h"
#include <QPainter>

RoundedVideoItem::RoundedVideoItem(QGraphicsItem *parent)
    : QGraphicsVideoItem(parent)
    , m_borderRadius(0.0) // 保持成员变量，但不再用于内部裁剪
{
}

void RoundedVideoItem::setBorderRadius(qreal radius)
{
    if (m_borderRadius != radius) {
        m_borderRadius = radius;
        // update(); // 不再需要更新，因为裁剪由QGraphicsView处理
    }
}

void RoundedVideoItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    // ****** 关键修改：移除QGraphicsVideoItem自身的裁剪 ******
    // 裁剪将由QGraphicsView的视口border-radius处理
    QGraphicsVideoItem::paint(painter, option, widget);
}
