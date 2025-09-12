#include "CitySilhouettePainter.h"
#include <QWidget> // 用于parentWidget()

CitySilhouettePainter::CitySilhouettePainter(QObject *parent)
    : QObject(parent)
{
}

// Setter 实现，并发出信号，请求父Widget重绘
// 修正：函数名从 setColor 改为 setCityColor
void CitySilhouettePainter::setCityColor(const QColor& color)
{
    if (m_cityColor != color) {
        m_cityColor = color;
        emit cityColorChanged();
        if (QWidget* p = qobject_cast<QWidget*>(parent())) p->update();
    }
}

void CitySilhouettePainter::setSize(const QSize& size)
{
    if (m_size != size) { // 只有当尺寸改变时才重新生成路径
        m_size = size;
        createCityPath();
        if (QWidget* p = qobject_cast<QWidget*>(parent())) p->update(); // 尺寸改变也需要重绘
    }
}

void CitySilhouettePainter::paint(QPainter* painter)
{
    if (m_size.isEmpty() || m_cityPath.isEmpty()) return;

    painter->setPen(Qt::NoPen); // 无边框
    painter->setBrush(m_cityColor); // 填充颜色，使用动画属性
    painter->drawPath(m_cityPath); // 绘制城市剪影路径
}

void CitySilhouettePainter::createCityPath()
{
    m_cityPath = QPainterPath();
    if (m_size.isEmpty()) return;

    qreal h = m_size.height(); // Widget高度
    qreal w = m_size.width();  // Widget宽度

    // 城市剪影占据底部约30%的高度，最高建筑达到h*0.7
    // 定义一系列相对高度，模拟波普艺术风格的方块状建筑
    qreal buildingHeights[] = {0.75, 0.7, 0.8, 0.72, 0.85, 0.78, 0.73, 0.82, 0.7, 0.76};
    // 每个建筑的宽度比例
    qreal buildingWidthRatio = 1.0 / (sizeof(buildingHeights) / sizeof(buildingHeights[0]));

    // 从左下角开始绘制
    m_cityPath.moveTo(0, h);

    for (int i = 0; i < sizeof(buildingHeights) / sizeof(buildingHeights[0]); ++i) {
        qreal x1 = i * buildingWidthRatio * w;
        qreal x2 = (i + 1) * buildingWidthRatio * w;
        qreal y = buildingHeights[i] * h; // 建筑顶部的高度

        m_cityPath.lineTo(x1, h); // 连接到地面
        m_cityPath.lineTo(x1, y); // 向上到建筑顶部
        m_cityPath.lineTo(x2, y); // 沿着建筑顶部
        m_cityPath.lineTo(x2, h); // 向下到地面
    }

    // 确保路径延伸到右下角并闭合
    m_cityPath.lineTo(w, h);
    m_cityPath.closeSubpath();
}
