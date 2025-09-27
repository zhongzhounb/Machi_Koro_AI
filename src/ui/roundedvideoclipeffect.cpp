#include "RoundedVideoClipEffect.h"

RoundedVideoClipEffect::RoundedVideoClipEffect(QObject *parent)
    : QGraphicsEffect(parent)
{
    // 确保效果器在需要时会重新绘制
    // setCacheEnabled(true); // <-- 移除此行，解决编译错误
}

void RoundedVideoClipEffect::setBorderRadius(qreal radius)
{
    if (m_borderRadius != radius) {
        m_borderRadius = radius;
        update(); // 请求重绘以应用新的圆角
    }
}

void RoundedVideoClipEffect::draw(QPainter *painter)
{
    QPixmap source = sourcePixmap(Qt::LogicalCoordinates);

    if (source.isNull()) {
        drawSource(painter);
        return;
    }

    QPainterPath path;
    path.addRoundedRect(source.rect(), m_borderRadius, m_borderRadius);

    painter->setRenderHint(QPainter::Antialiasing, true);
    painter->setClipPath(path);
    painter->drawPixmap(0, 0, source);
}
