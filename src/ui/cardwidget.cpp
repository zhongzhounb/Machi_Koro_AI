#include "cardwidget.h"
#include <QDebug>
#include <QColor>
#include <QFile>
#include <QResizeEvent>
#include <QLabel>
#include "card.h"
#include <QGraphicsDropShadowEffect> // 新增：阴影效果头文件
#include <QPainterPath> // 如果未来需要更精细的图片圆角裁剪，可能会用到
#include <QBuffer> // 新增：用于 QPixmap 转 Base64
#include <QStandardPaths> // 用于调试保存图片
#include <QPainter>
#include "autofittextlabel.h"

//图片圆角算法
QPixmap QPixmapToRound(const QPixmap & img, int radius)
{
    if (img.isNull())
    {
        return QPixmap();
    }
    QSize size(img.size());
    QBitmap mask(size);
    QPainter painter(&mask);
    painter.setRenderHint(QPainter::Antialiasing);
    painter.setRenderHint(QPainter::SmoothPixmapTransform);
    painter.fillRect(mask.rect(), Qt::white);
    painter.setBrush(QColor(0, 0, 0));
    painter.drawRoundedRect(mask.rect(), radius, radius);
    QPixmap image = img;// .scaled(size);
    image.setMask(mask);
    return image;
}

// 辅助函数定义：将 Color 枚举转换为 QColor
QColor colorToQColor(Color color) {
    switch (color) {
    case Color::Landmark: return QColor("#BC4C42");
    case Color::Red:      return QColor("#C33540");
    case Color::Blue:     return QColor("#5771B9");
    case Color::Green:    return QColor("#506A43");
    case Color::Purple:   return QColor("#7A3085");
    default:              return QColor("white");
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
    case Color::BackNone: return ":/resources/images/card/background/backnone.png";
    case Color::BackSmall: return ":/resources/images/card/background/backsmall.png";
    case Color::BackBig: return ":/resources/images/card/background/backbig.png";
    case Color::BackPurple: return ":/resources/images/card/background/backpurple.png";
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

// ... (此处应包含您的辅助函数实现, 如 QPixmapToRound, colorToQColor 等)

CardWidget::CardWidget(Card* card, ShowType type, QWidget* parent)
    : QFrame(parent)
    , m_card(card)
    , m_type(type)
    , m_mainLayout(new QStackedLayout(this))
    , m_backgroundImgLabel(new QLabel(this))
    , m_imgLabel(new QLabel(this))
    , m_textContainer(new QWidget(this))
    , m_activationRangeLabel(new QLabel(this))
    , m_nameLabel(new QLabel(this))
    , m_descriptionLabel(new QLabel(this))
    , m_costLabel(new QLabel(this))
    , m_stateOverlayLabel(new QLabel("CLOSED", this))
{
    // *** 核心修改点：设置尺寸策略为 Ignored ***
    // 这会告诉布局系统，完全由它来决定本控件的大小
    setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Ignored);

    // 隐藏不需要的元素
    if(m_type == ShowType::BackGround){
        m_activationRangeLabel->hide();
        m_nameLabel->hide();
        m_descriptionLabel->hide();
        m_costLabel->hide();
        m_imgLabel->hide();
    }
    m_descriptionLabel->hide();

    setContentsMargins(0, 0, 0, 0);

    initUI();
    updateData();

    if (m_card) {
        connect(m_card, &Card::cardStateChanged, this, &CardWidget::onCardStateChanged);
        connect(m_card, &Card::cardValueChanged, this, &CardWidget::onCardValueChanged);
    }

    setLayout(m_mainLayout);
}

CardWidget::~CardWidget(){}

// *** 新增 resizeEvent 的实现 ***
void CardWidget::resizeEvent(QResizeEvent *event)
{
    QFrame::resizeEvent(event);

    if (m_isResizing) {
        return;
    }
    m_isResizing = true;

    const QSize allocatedSize = event->size();
    int width = allocatedSize.width();
    int height = allocatedSize.height();

    // 假设 m_aspectRatio 已经正确初始化
    // 例如：m_aspectRatio = 宽度 / 高度;
    if (m_aspectRatio == 0) { // 避免除以零
        m_isResizing = false;
        return;
    }

    if (static_cast<double>(width) / m_aspectRatio < height) { // 使用 double 进行浮点除法
        height = static_cast<int>(static_cast<double>(width) / m_aspectRatio);
    } else {
        width = static_cast<int>(static_cast<double>(height) * m_aspectRatio);
    }

    QRect newGeometry(0, 0, width, height);
    // 修正后的代码
    newGeometry.moveCenter(QRect(QPoint(), allocatedSize).center());

    // 使用 setGeometry 来调整大小和位置，而不是 setFixedSize
    setGeometry(newGeometry);

    // 根据新的有效高度更新字体
    int fontSize = qMax(5, height / 10);
    m_activationRangeLabel->setFont(QFont("YouYuan", fontSize, QFont::Bold));
    m_nameLabel->setFont(QFont("YouYuan", fontSize, QFont::Bold));
    m_costLabel->setFont(QFont("YouYuan", fontSize, QFont::Bold));

    m_isResizing = false;
}

void CardWidget::initUI()
{
    // initUI 函数体本身不需要改变
    m_mainLayout->setStackingMode(QStackedLayout::StackAll);

    m_imgLabel->setScaledContents(true);
    m_backgroundImgLabel->setScaledContents(true);

    QPixmap backgroundPixmap(colorToImagePath(m_card->getColor()));
    if (!backgroundPixmap.isNull()) {
        m_backgroundImgLabel->setPixmap(QPixmapToRound(backgroundPixmap.copy(QRect(0,0,720,720)), 50));
    }

    QPixmap cardImagePixmap(classNameToImagePath(m_card->metaObject()->className()));
    if (!cardImagePixmap.isNull()) {
        m_imgLabel->setPixmap(QPixmapToRound(cardImagePixmap.copy(QRect(0,0+170,1900,1900)), 50));
    }

    QString textStyle = "QLabel { color:white; }";
    QString nameStyle= QString("QLabel { color: %1; }").arg(colorToQColor(m_card->getColor()).name());
    m_activationRangeLabel->setStyleSheet(textStyle);
    m_nameLabel->setStyleSheet(nameStyle);
    m_descriptionLabel->setStyleSheet(textStyle);
    m_costLabel->setStyleSheet(textStyle);

    m_activationRangeLabel->setAlignment(Qt::AlignCenter);
    m_nameLabel->setAlignment(Qt::AlignCenter);
    m_descriptionLabel->setAlignment(Qt::AlignCenter);
    m_costLabel->setAlignment(Qt::AlignRight | Qt::AlignVCenter);

    m_textLayout = new QVBoxLayout(m_textContainer);
    m_textLayout->setContentsMargins(0, 0, 0, 0);
    m_textLayout->setSpacing(0);
    m_textLayout->addWidget(m_activationRangeLabel, 1);
    m_textLayout->addWidget(m_nameLabel, 1);
    m_textLayout->addStretch(2);
    m_textLayout->addWidget(m_costLabel, 1);

    m_mainLayout->addWidget(m_stateOverlayLabel);
    m_mainLayout->addWidget(m_textContainer);
    m_mainLayout->addWidget(m_imgLabel);
    m_mainLayout->addWidget(m_backgroundImgLabel);
}

void CardWidget::updateData()
{
    // updateData 函数体本身不需要改变
    if (!m_card) return;

    m_nameLabel->setText(typeToImg(m_card->getType()) + m_card->getName());
    m_costLabel->setText(QString("%1🪙").arg(m_card->getCost()));
    m_descriptionLabel->setText(m_card->getDescription());

    if (m_card->getActLNum() == 0 && m_card->getActRNum() == 0)
        m_activationRangeLabel->hide();
    else if (m_card->getActLNum() == m_card->getActRNum())
        m_activationRangeLabel->setText(QString("%1").arg(m_card->getActLNum()));
    else
        m_activationRangeLabel->setText(QString("%1~%2").arg(m_card->getActLNum()).arg(m_card->getActRNum()));

    m_stateOverlayLabel->setVisible(m_card->getState() == State::Closing);
}

void CardWidget::onCardStateChanged(Card* card, State newState)
{
    if (card == m_card) updateData();
}

void CardWidget::onCardValueChanged(Card* card)
{
    if (card == m_card) updateData();
}

void CardWidget::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton) {
        emit clicked(m_card);
    }
    QFrame::mousePressEvent(event);
}
