#include "cardwidget.h"
#include <QDebug>
#include <QPalette>
#include <QBrush>
#include <QColor>
#include <QFile>
#include <QPainter>
#include <QMap>
#include <QResizeEvent> // 确保包含 QResizeEvent

// --- 参考尺寸和位置常量 ---
// 这些值需要你根据在 Designer 中模拟出的最佳布局来精确调整
const int CARD_REF_WIDTH = 100;
const int CARD_REF_HEIGHT = 150;

const QRect BUILDING_IMG_RECT(0, 0, CARD_REF_WIDTH, CARD_REF_HEIGHT); // 建筑图片的位置和大小（全屏）
const QRect ACTIVATION_RANGE_LABEL_RECT(0, 0, 100, 15);
const QRect NAME_LABEL_RECT(0, 20, 100, 20); // x, y, width, height
const QRect COST_LABEL_RECT(0, 130, 100, 20);
const QRect DESCRIPTION_LABEL_RECT(0, 105, 100, 40);

const int NAME_FONT_SIZE = 10;
const int COST_FONT_SIZE = 10;
const int TYPE_ICON_FONT_SIZE = 10; // 字体大小用于类型图标，它现在在 nameLabel 中
const int ACTIVATION_FONT_SIZE = 10;
const int DESCRIPTION_FONT_SIZE = 5;
// --- 参考尺寸和位置常量结束 ---


// 辅助函数定义：将 Color 枚举转换为 QColor
QString colorToQColor(Color color) {
    switch (color) {
    case Color::Landmark: return "#BC4C42"; // 金色
    case Color::Red:      return "#C33540"; // 番茄红
    case Color::Blue:     return "#5771B9"; // 亮天蓝色
    case Color::Green:    return "#506A43"; // 浅绿色
    case Color::Purple:   return "#7A3085"; // 紫罗兰色
    default:              return "white";
    }
}

// 辅助函数：将 Color 枚举转换为背景图片路径
QString colorToImagePath(Color color) {
    switch (color) {
    case Color::Landmark: return ":/resources/images/card/background/landmark.png";
    case Color::Red:      return ":/resources/images/card/background/red.png";
    case Color::Blue:     return ":/resources/images/card/background/blue.png";
    case Color::Green:    return ":/resources/images/card/background/green.png";
    case Color::Purple:   return ":/resources/images/card/background/purple.png";
    default:              return "";
    }
}

// 辅助函数：将卡牌类名转换为建筑图片路径
QString classNameToImagePath(const QString& className) {
    QString path = QString(":/resources/images/card/img/%1.png").arg(className);

#ifdef QT_DEBUG
    if (!QFile::exists(path)) {
        qDebug() << "DEBUG: Image resource file does NOT exist for class:" << className << " at path:" << path;
    }
#endif
    return path;
}


CardWidget::CardWidget(Card* card, QWidget* parent)
    : QFrame(parent)
    , m_card(card)
    , m_nameLabel(new QLabel(this))
    , m_costLabel(new QLabel(this))
    , m_activationRangeLabel(new QLabel(this))
    , m_descriptionLabel(new QLabel(this))
    , m_stateOverlayLabel(new QLabel("CLOSED", this)) // m_stateOverlayLabel 已经正确初始化
    , m_img(new QLabel(this))
{
    setFrameShape(QFrame::Box);

    setupUI(); // 初始化 QLabel 实例
    updateCardUI(); // 填充数据
    updateLabelGeometries(); // 初始设置标签位置

    if (m_card) {
        connect(m_card, &Card::cardStateChanged, this, &CardWidget::onCardStateChanged);
        connect(m_card, &Card::cardValueChanged, this, &CardWidget::onCardValueChanged);
    }
}

CardWidget::~CardWidget()
{
    // 子部件已设置为当前部件的父对象，会在当前部件析构时自动删除。
}

void CardWidget::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event);

    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    QPixmap backgroundPixmap(colorToImagePath(m_card->getColor()));
    if (!backgroundPixmap.isNull()) {
        painter.drawPixmap(rect(), backgroundPixmap);
    } else {
        painter.fillRect(rect(), QColor());
    }

    QFrame::paintEvent(event); // 调用基类绘制边框和子部件
}

void CardWidget::resizeEvent(QResizeEvent *event)
{
    QFrame::resizeEvent(event); // 调用基类的 resizeEvent

    updateLabelGeometries(); // 每当 CardWidget 大小改变时，更新标签的几何形状
    m_stateOverlayLabel->setGeometry(rect()); // <--- 修正：使用 m_stateOverlayLabel
}

void CardWidget::updateLabelGeometries()
{
    qreal scaleX = static_cast<qreal>(width()) / CARD_REF_WIDTH;
    qreal scaleY = static_cast<qreal>(height()) / CARD_REF_HEIGHT;

    qreal fontScale = qMin(scaleX, scaleY);

    // 名称标签
    m_nameLabel->setGeometry(
        NAME_LABEL_RECT.x() * scaleX,
        NAME_LABEL_RECT.y() * scaleY,
        NAME_LABEL_RECT.width() * scaleX,
        NAME_LABEL_RECT.height() * scaleY
        );
    m_nameLabel->setFont(QFont("YouYuan", NAME_FONT_SIZE * fontScale, QFont::Bold));
    m_nameLabel->setAlignment(Qt::AlignCenter);

    // 花费标签
    m_costLabel->setGeometry(
        COST_LABEL_RECT.x() * scaleX,
        COST_LABEL_RECT.y() * scaleY,
        COST_LABEL_RECT.width() * scaleX,
        COST_LABEL_RECT.height() * scaleY
        );
    m_costLabel->setFont(QFont("YouYuan", COST_FONT_SIZE * fontScale, QFont::Bold));
    m_costLabel->setAlignment(Qt::AlignRight | Qt::AlignVCenter);

    // 建筑图片标签
    m_img->setGeometry(
        BUILDING_IMG_RECT.x() * scaleX,
        BUILDING_IMG_RECT.y() * scaleY,
        BUILDING_IMG_RECT.width() * scaleX,
        BUILDING_IMG_RECT.height() * scaleY
        );
    m_img->setAlignment(Qt::AlignCenter);

    // 激活范围标签
    m_activationRangeLabel->setGeometry(
        ACTIVATION_RANGE_LABEL_RECT.x() * scaleX,
        ACTIVATION_RANGE_LABEL_RECT.y() * scaleY,
        ACTIVATION_RANGE_LABEL_RECT.width() * scaleX,
        ACTIVATION_RANGE_LABEL_RECT.height() * scaleY
        );
    m_activationRangeLabel->setFont(QFont("YouYuan", ACTIVATION_FONT_SIZE * fontScale, QFont::Bold));
    m_activationRangeLabel->setAlignment(Qt::AlignCenter);

    // 描述标签
    m_descriptionLabel->setGeometry(
        DESCRIPTION_LABEL_RECT.x() * scaleX,
        DESCRIPTION_LABEL_RECT.y() * scaleY,
        DESCRIPTION_LABEL_RECT.width() * scaleX,
        DESCRIPTION_LABEL_RECT.height() * scaleY
        );
    m_descriptionLabel->setFont(QFont("SimHei", DESCRIPTION_FONT_SIZE * fontScale));
    m_descriptionLabel->setAlignment(Qt::AlignCenter);
    m_descriptionLabel->setWordWrap(true);

}


void CardWidget::setupUI()
{
    // 这里不再使用布局管理器，所有子部件的定位都将在 updateLabelGeometries() 中手动完成
    // 确保所有 QLabel 实例都被创建并设置为当前部件的子部件
    m_nameLabel->setParent(this);
    m_costLabel->setParent(this);
    m_activationRangeLabel->setParent(this);
    m_descriptionLabel->setParent(this);
    m_stateOverlayLabel->setParent(this);
    m_img->setParent(this);

    // 设置一些基本属性，如缩放模式和对齐方式
    m_img->setScaledContents(true); // 确保建筑图片缩放
    // m_stateOverlayLabel 的样式和对齐将在 applyCardStyle() 中设置
    m_stateOverlayLabel->hide(); // 默认隐藏

    setMinimumSize(100, 150); // 确保卡牌有最小尺寸
    setMaximumWidth(150); // 限制宽度，如果需要
}


void CardWidget::updateCardUI()
{
    if (!m_card) {
        qWarning() << "CardWidget: No card assigned.";
        return;
    }

    m_nameLabel->setText(typeToImg(m_card->getType()) + m_card->getName());
    m_costLabel->setText(QString("%1🪙").arg(m_card->getCost()));
    m_descriptionLabel->setText(m_card->getDescription());

    if (m_card->getActLNum() == 0 && m_card->getActRNum() == 0) {
        m_activationRangeLabel->setText("No Activation");
    } else if (m_card->getActLNum() == m_card->getActRNum()) {
        m_activationRangeLabel->setText(QString("%1").arg(m_card->getActLNum()));
    } else {
        m_activationRangeLabel->setText(QString("%1 - %2").arg(m_card->getActLNum()).arg(m_card->getActRNum()));
    }

    // 设置建筑图片
    QPixmap cardImagePixmap(classNameToImagePath(m_card->metaObject()->className()));
    if (!cardImagePixmap.isNull()) {
        m_img->setPixmap(cardImagePixmap);
    } else {
        qWarning() << "Failed to load card image for class:" << m_card->metaObject()->className() << " Path:" << classNameToImagePath(m_card->metaObject()->className());
        m_img->clear();
    }

    applyCardStyle();
    onCardStateChanged(m_card, m_card->getState());
    update(); // 强制重绘
}

void CardWidget::applyCardStyle()
{
    setStyleSheet("background-color: transparent;");

    QString textStyle = "QLabel { color:white; }";
    QString nameStyle= QString("QLabel { color: %1; }").arg(colorToQColor(m_card->getColor()));

    // 应用基本样式
    m_nameLabel->setStyleSheet(nameStyle);
    m_costLabel->setStyleSheet(textStyle );
    m_activationRangeLabel->setStyleSheet(textStyle);
    m_descriptionLabel->setStyleSheet(textStyle );
    m_img->setStyleSheet(m_img->styleSheet()); // 建筑图片标签的边框

    // 状态覆盖层标签的样式（还没动）
    m_stateOverlayLabel->setStyleSheet("background-color: rgba(0, 0, 0, 150); color: white; font-weight: bold; font-size: 16px; border: 1px solid cyan;"); // 调试边框
    m_stateOverlayLabel->setAlignment(Qt::AlignCenter);
}

void CardWidget::onCardStateChanged(Card* card, State newState)
{
    if (card != m_card) return;

    if (newState == State::Closing) {
        m_stateOverlayLabel->setGeometry(rect()); // <--- 修正：使用 m_stateOverlayLabel
        m_stateOverlayLabel->show(); // <--- 修正：使用 m_stateOverlayLabel
        QGraphicsColorizeEffect* effect = new QGraphicsColorizeEffect(this);
        effect->setColor(QColor(0, 0, 0, 100));
        effect->setStrength(0.5);
        setGraphicsEffect(effect);
    } else {
        m_stateOverlayLabel->hide(); // <--- 修正：使用 m_stateOverlayLabel
        if (graphicsEffect()) {
            delete graphicsEffect();
            setGraphicsEffect(nullptr);
        }
    }
    update();
}

void CardWidget::onCardValueChanged(Card* card)
{
    if (card != m_card) return;
    updateCardUI();
}
