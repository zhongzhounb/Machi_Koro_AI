#include "SkyPainter.h"
#include <QWidget>
#include <QDebug>
// #include <QRadialGradient> // 移除径向渐变头文件，不再使用

SkyPainter::SkyPainter(QObject *parent)
    : QObject(parent)
    , m_currentState(GameBackgroundWidget::Dawn)
{
    initializeCloudData();
}

// Setter 实现，并发出信号，请求父Widget重绘
void SkyPainter::setSkyTopColor(const QColor& color) {
    if (m_skyTopColor != color) {
        m_skyTopColor = color;
        emit skyTopColorChanged();
        if (QWidget* p = qobject_cast<QWidget*>(parent())) p->update();
    }
}
void SkyPainter::setSkyBottomColor(const QColor& color) {
    if (m_skyBottomColor != color) {
        m_skyBottomColor = color;
        emit skyBottomColorChanged();
        if (QWidget* p = qobject_cast<QWidget*>(parent())) p->update();
    }
}
void SkyPainter::setSunMoonColor(const QColor& color) {
    if (m_sunMoonColor != color) {
        m_sunMoonColor = color;
        emit sunMoonColorChanged();
        if (QWidget* p = qobject_cast<QWidget*>(parent())) p->update();
    }
}
void SkyPainter::setCloudColor(const QColor& color) {
    if (m_cloudColor != color) {
        m_cloudColor = color;
        emit cloudColorChanged();
        if (QWidget* p = qobject_cast<QWidget*>(parent())) p->update();
    }
}
void SkyPainter::setStarColor(const QColor& color) {
    if (m_starColor != color) {
        m_starColor = color;
        emit starColorChanged();
        if (QWidget* p = qobject_cast<QWidget*>(parent())) p->update();
    }
}
void SkyPainter::setSunMoonPosition(const QPointF& pos) {
    if (m_sunMoonPosition != pos) {
        m_sunMoonPosition = pos;
        emit sunMoonPositionChanged();
        if (QWidget* p = qobject_cast<QWidget*>(parent())) p->update();
    }
}
void SkyPainter::setCloudBaseOffsetX(qreal offset) {
    if (!qFuzzyCompare(m_cloudBaseOffsetX, offset)) {
        m_cloudBaseOffsetX = offset;
        emit cloudBaseOffsetXChanged();
        if (QWidget* p = qobject_cast<QWidget*>(parent())) p->update();
    }
}

void SkyPainter::setSize(const QSize& size)
{
    if (m_size != size) {
        m_size = size;
        initializeCloudData(); // 尺寸改变后重新初始化云朵数据
        if (QWidget* p = qobject_cast<QWidget*>(parent())) p->update();
    }
}

void SkyPainter::setBackgroundState(GameBackgroundWidget::BackgroundState state)
{
    m_currentState = state;
}

void SkyPainter::paint(QPainter* painter)
{
    if (m_size.isEmpty()) return;

    // 绘制天空渐变背景
    bool isDaytime = (m_currentState == GameBackgroundWidget::Dawn || m_currentState == GameBackgroundWidget::Noon || m_currentState == GameBackgroundWidget::Dusk);

    QLinearGradient skyGradient(0, 0, 0, m_size.height()); // 总是垂直线性渐变

    if (m_currentState == GameBackgroundWidget::Dawn) {
        // 清晨：三层渐变：顶部蓝色，中间黄色，底部橙色
        // 注意：线性渐变 setColorAt(0) 是顶部，setColorAt(1) 是底部
        skyGradient.setColorAt(0, m_skyTopColor);    // 顶部蓝色
        skyGradient.setColorAt(0.5*0.9, QColor("#faedde"));
        skyGradient.setColorAt(0.75*0.9, QColor("#fdd7a8"));
        skyGradient.setColorAt(0.9*0.9, QColor("#ffa688"));
        skyGradient.setColorAt(1, m_skyBottomColor); // 底部橙色
    } else if (m_currentState == GameBackgroundWidget::Noon) {
        // 正午：太阳在顶部，所以天空顶部更亮 (m_skyBottomColor)，底部更暗 (m_skyTopColor)
        skyGradient.setColorAt(0, m_skyTopColor); // 顶部更亮
        skyGradient.setColorAt(1, m_skyBottomColor);    // 底部更暗
    } else if (m_currentState == GameBackgroundWidget::Dusk) {
        // 黄昏：太阳在底部，所以天空顶部更暗 (m_skyTopColor)，底部更亮 (m_skyBottomColor)
        skyGradient.setColorAt(0, m_skyTopColor);    // 顶部更暗
        skyGradient.setColorAt(0.5*0.9, QColor("#faedde"));
        skyGradient.setColorAt(0.75*0.9, QColor("#fdd7a8"));
        skyGradient.setColorAt(0.9*0.9, QColor("#ffa688"));
        skyGradient.setColorAt(1, m_skyBottomColor); // 底部更亮
    } else { // 夜晚和深夜
        // 夜晚：保持顶部暗，底部亮 (月亮在上方，所以顶部暗，底部亮合理)
        skyGradient.setColorAt(0, m_skyTopColor);
        skyGradient.setColorAt(1, m_skyBottomColor);
    }
    painter->fillRect(0, 0, m_size.width(), m_size.height(), skyGradient);

    drawSunMoon(painter);

    if (m_currentState == GameBackgroundWidget::Night || m_currentState == GameBackgroundWidget::DeepNight) {
        drawStars(painter);
    }

    if (isDaytime) { // 只有白天状态才绘制云朵
        drawClouds(painter);
    }
}

void SkyPainter::drawSunMoon(QPainter* painter)
{
    painter->setPen(Qt::NoPen);
    painter->setBrush(m_sunMoonColor);

    qreal sunMoonRadius = qMin(m_size.width(), m_size.height()) * 0.08;

    qreal moonX = m_sunMoonPosition.x();
    qreal moonY = m_sunMoonPosition.y();

    QPainterPath fullMoonPath;
    fullMoonPath.addEllipse(moonX - sunMoonRadius, moonY - sunMoonRadius, sunMoonRadius * 2, sunMoonRadius * 2);

    if (m_currentState == GameBackgroundWidget::DeepNight) {
        QPainterPath subtractPath;
        qreal offset = sunMoonRadius * 0.4;
        subtractPath.addEllipse(moonX - sunMoonRadius + offset, moonY - sunMoonRadius, sunMoonRadius * 2, sunMoonRadius * 2);
        painter->fillPath(fullMoonPath.subtracted(subtractPath), m_sunMoonColor);
    } else {
        painter->fillPath(fullMoonPath, m_sunMoonColor);
    }
}

void SkyPainter::drawStars(QPainter* painter)
{
    if (m_starColor.alpha() == 0) return;

    painter->setPen(Qt::NoPen);
    painter->setBrush(m_starColor);

    int numStars = (m_currentState == GameBackgroundWidget::DeepNight) ? 60 : 30;
    qreal starSize = qMin(m_size.width(), m_size.height()) * 0.005;

    for (int i = 0; i < numStars; ++i) {
        qreal x = RandomUtils::instance().generateInt(0, m_size.width() - 1);
        qreal y = RandomUtils::instance().generateInt(0, m_size.height() * 0.7 - 1);

        painter->drawEllipse(x - starSize / 2, y - starSize / 2, starSize, starSize);
    }
}

void SkyPainter::initializeCloudData() {
    QVector<QVector<CloudPart>> cloudTemplates = {
        { // Cloud Group 1 Template (左侧)
            {0.1, 0.2, 0.15, 0.08},
            {0.15, 0.18, 0.12, 0.07},
            {0.08, 0.19, 0.1, 0.06}
        },
        { // Cloud Group 2 Template (中间偏左)
            {0.35, 0.1, 0.18, 0.09},
            {0.43, 0.08, 0.14, 0.08},
            {0.30, 0.11, 0.1, 0.07}
        },
        { // Cloud Group 3 Template (中间偏右)
            {0.6, 0.15, 0.2, 0.1},
            {0.68, 0.12, 0.15, 0.09},
            {0.55, 0.16, 0.1, 0.07}
        },
        { // Cloud Group 4 Template (右侧)
            {0.85, 0.22, 0.16, 0.08},
            {0.90, 0.20, 0.13, 0.07},
            {0.80, 0.23, 0.11, 0.06}
        }
    };

    m_cloudData.clear();
    for (const auto& templateParts : cloudTemplates) {
        CloudData data;
        data.speedFactor = RandomUtils::instance().generateInt(80, 120) / 100.0;
        data.initialOffsetX = RandomUtils::instance().generateInt(0, m_size.width() > 0 ? m_size.width() : 800);
        data.templateParts = templateParts;
        m_cloudData.append(data);
    }
}

void SkyPainter::drawClouds(QPainter* painter)
{
    if (m_cloudColor.alpha() == 0 || m_size.isEmpty()) return;

    painter->setPen(Qt::NoPen);
    painter->setBrush(m_cloudColor);

    for (const auto& data : m_cloudData) {
        qreal effectiveGroupOffset = data.initialOffsetX - (m_cloudBaseOffsetX * data.speedFactor);

        qreal wrappedOffset = fmod(effectiveGroupOffset, m_size.width());
        if (wrappedOffset < 0) {
            wrappedOffset += m_size.width();
        }

        for (const auto& part : data.templateParts) {
            qreal x = part.relX * m_size.width() + wrappedOffset;
            qreal y = part.relY * m_size.height();
            qreal w = part.relW * m_size.width();
            qreal h = part.relH * m_size.height();
            painter->drawEllipse(x, y, w, h);
        }

        for (const auto& part : data.templateParts) {
            qreal x = part.relX * m_size.width() + wrappedOffset + m_size.width();
            qreal y = part.relY * m_size.height();
            qreal w = part.relW * m_size.width();
            qreal h = part.relH * m_size.height();
            painter->drawEllipse(x, y, w, h);
        }
    }
}
