#include "cardwidget.h"
#include <QDebug>
#include <QColor>
#include <QFile>
#include <QResizeEvent>
#include <QLabel>
#include "card.h"
#include <QGraphicsDropShadowEffect>
#include <QPainterPath>
#include <QBuffer>
#include <QStandardPaths>
#include <QPainter>
#include "autofittextlabel.h"
#include "coins/coinswidget.h"
#include <QHBoxLayout>

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
        //qDebug() << "DEBUG: Image resource file does NOT exist for class:" << className << " at path:" << path;
    }
#endif
    return path;
}

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
    , m_costLabel(new CoinsWidget(this))
    , m_stateOverlayLabel(new QLabel("CLOSED", this))
    , m_hoverDelayTimer(new QTimer(this)) // 初始化计时器
{
    setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Ignored);

    // 启用鼠标跟踪，以便接收 hover 和 mouseMove 事件
    setMouseTracking(true);

    //比例
    if(type==ShowType::Detailed)
        m_aspectRatio=0.67;
    else
        m_aspectRatio=1.0;

    // 隐藏不需要的元素
    if(m_type == ShowType::BackGround){
        m_activationRangeLabel->hide();
        m_nameLabel->hide();
        m_descriptionLabel->hide();
        m_costLabel->hide();
        m_imgLabel->hide();
    }
    else if(m_type== ShowType::Ordinary)
        m_descriptionLabel->hide();

    setContentsMargins(0, 0, 0, 0);

    initUI();
    updateData();

    if (m_card) {
        connect(m_card, &Card::cardStateChanged, this, &CardWidget::onCardStateChanged);
        connect(m_card, &Card::cardValueChanged, this, &CardWidget::onCardValueChanged);
    }

    setLayout(m_mainLayout);

    // 设置悬停计时器
    m_hoverDelayTimer->setSingleShot(true);
    m_hoverDelayTimer->setInterval(500); // 0.5秒
    connect(m_hoverDelayTimer, &QTimer::timeout, this, &CardWidget::onHoverDelayTimeout);
}

CardWidget::~CardWidget(){}

void CardWidget::resizeEvent(QResizeEvent *event)
{
    QFrame::resizeEvent(event);
    const QSize allocatedSize = event->size();
    int width = allocatedSize.width();
    int height = allocatedSize.height();

    if (m_isAnimated) {
        int fontSize = qMax(5, width / 9);
        m_activationRangeLabel->setFont(QFont("YouYuan", fontSize, QFont::Bold));
        m_nameLabel->setFont(QFont("YouYuan", fontSize, QFont::Bold));
        m_costLabel->setFont(QFont("YouYuan", fontSize, QFont::Bold));
        m_descriptionLabel->setFont(QFont("YouYuan", fontSize/2, QFont::Bold));
        return;
    }

    if (m_isResizing) {
        return;
    }
    m_isResizing = true;

    if (m_aspectRatio == 0) { m_isResizing = false; return; }
    if (static_cast<double>(width) / m_aspectRatio < height) {
        height = static_cast<int>(static_cast<double>(width) / m_aspectRatio);
    } else {
        width = static_cast<int>(static_cast<double>(height) * m_aspectRatio);
    }
    QRect newGeometry(0, 0, width, height);
    newGeometry.moveCenter(QRect(QPoint(), allocatedSize).center());
    setGeometry(newGeometry);
    int fontSize = qMax(5, width / 9);
    m_activationRangeLabel->setFont(QFont("YouYuan", fontSize, QFont::Bold));
    m_nameLabel->setFont(QFont("YouYuan", fontSize, QFont::Bold));
    m_costLabel->setFont(QFont("YouYuan", fontSize, QFont::Bold));
    m_descriptionLabel->setFont(QFont("YouYuan", fontSize/2, QFont::Bold));

    m_isResizing = false;
}

void CardWidget::initUI()
{
    m_mainLayout->setStackingMode(QStackedLayout::StackAll);

    m_imgLabel->setScaledContents(true);
    m_backgroundImgLabel->setScaledContents(true);
    m_stateOverlayLabel->setScaledContents(true);

    QPixmap backgroundPixmap(colorToImagePath(m_card->getColor()));
    if (!backgroundPixmap.isNull()) {
        m_backgroundImgLabel->setPixmap(QPixmapToRound(backgroundPixmap.copy(QRect(0,0,720,720/m_aspectRatio)), 50));
    }

    QPixmap cardImagePixmap(classNameToImagePath(m_card->metaObject()->className()));
    if (!cardImagePixmap.isNull()) {
        m_imgLabel->setPixmap(QPixmapToRound(cardImagePixmap.copy(QRect(0,0+170,1900,1900/m_aspectRatio)), 50));
    }

    QPixmap closedPixmap(":/resources/images/card/closing/closed.png");
    if (!cardImagePixmap.isNull()) {
        m_stateOverlayLabel->setPixmap(QPixmapToRound(closedPixmap.copy(QRect(0,0,1125,1125/m_aspectRatio)), 50));
    }

    m_activationRangeLabel->setAlignment(Qt::AlignCenter);
    m_nameLabel->setAlignment(Qt::AlignCenter);
    m_descriptionLabel->setAlignment(Qt::AlignCenter);
    m_descriptionLabel->setWordWrap(true);


    m_textLayout = new QVBoxLayout(m_textContainer);
    m_textLayout->setContentsMargins(0, 0, 0, 0);
    m_textLayout->setSpacing(0);
    m_textLayout->addWidget(m_activationRangeLabel, 10);
    m_textLayout->addWidget(m_nameLabel, 10);
    m_textLayout->addStretch(15);

    QHBoxLayout* costRowLayout = new QHBoxLayout();
    costRowLayout->setContentsMargins(0, 0, 0, 0);
    costRowLayout->setSpacing(0);

    costRowLayout->addWidget(m_costLabel,1);
    costRowLayout->addStretch(3);

    m_textLayout->addLayout(costRowLayout, 15);

    if (m_type == ShowType::Detailed) {
        QHBoxLayout* descriptionRowLayout = new QHBoxLayout();
        descriptionRowLayout->setContentsMargins(0, 0, 0, 0);
        descriptionRowLayout->setSpacing(0);

        descriptionRowLayout->addStretch(1);
        descriptionRowLayout->addWidget(m_descriptionLabel, 18);
        descriptionRowLayout->addStretch(1);

        m_textLayout->addLayout(descriptionRowLayout, 30);
    }

    m_mainLayout->addWidget(m_textContainer);
    m_mainLayout->addWidget(m_stateOverlayLabel);
    m_mainLayout->addWidget(m_imgLabel);
    m_mainLayout->addWidget(m_backgroundImgLabel);
}

void CardWidget::updateData()
{
    if (!m_card) return;

    m_nameLabel->setText(typeToImg(m_card->getType()) + m_card->getName());
    m_costLabel->setCoinsNum(m_card->getCost());
    m_descriptionLabel->setText(m_card->getDescription());

    if (m_card->getActLNum() == 0 && m_card->getActRNum() == 0)
        m_activationRangeLabel->hide();
    else if (m_card->getActLNum() == m_card->getActRNum())
        m_activationRangeLabel->setText(QString("%1").arg(m_card->getActLNum()));
    else
        m_activationRangeLabel->setText(QString("%1~%2").arg(m_card->getActLNum()).arg(m_card->getActRNum()));

    m_stateOverlayLabel->setVisible(m_card->getState() == State::Closing);

    QString textStyle = "QLabel { color:white; }";
    QString nameStyle= QString("QLabel { color: %1; }").arg(colorToQColor(m_card->getColor()).name());
    m_activationRangeLabel->setStyleSheet(textStyle);
    m_nameLabel->setStyleSheet(m_card->getState()==State::Opening?nameStyle:textStyle);
    m_descriptionLabel->setStyleSheet(textStyle);
    m_costLabel->setStyleSheet(textStyle);
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

// 鼠标进入事件处理函数
void CardWidget::enterEvent(QEnterEvent *event)
{
    Q_UNUSED(event);
    // 只有在非动画模式且不是背景显示类型时才应用悬停效果
    if (!m_isAnimated && m_type != ShowType::BackGround) {
        setStyleSheet("CardWidget { border: 4px solid white; border-radius: 10px; }");
        m_lastMousePos = event->globalPosition().toPoint(); // 记录鼠标进入时的全局位置
        m_hoverDelayTimer->start(); // 启动计时器
    }
    emit hovered(m_card); // 仍然发出悬停信号
    QFrame::enterEvent(event);
}

// 鼠标离开事件处理函数
void CardWidget::leaveEvent(QEvent *event)
{
    Q_UNUSED(event);
    // 只有在非动画模式且不是背景显示类型时才清除悬停效果
    if (!m_isAnimated && m_type != ShowType::BackGround) {
        setStyleSheet(""); // 清除样式表，移除边框
    }
    m_hoverDelayTimer->stop(); // 停止计时器
    emit requestHideDetailedCard(); // 立即发出隐藏请求
    QFrame::leaveEvent(event);
}

// 鼠标移动事件处理函数
void CardWidget::mouseMoveEvent(QMouseEvent *event)
{
    // 如果不是详细显示类型且不是背景类型，更新鼠标位置
    if (m_type != ShowType::Detailed && m_type != ShowType::BackGround) {
        m_lastMousePos = event->globalPosition().toPoint();
    }
    QFrame::mouseMoveEvent(event);
}

// 悬停计时器超时槽函数
void CardWidget::onHoverDelayTimeout()
{
    // 计时器超时，发出显示详细卡牌的请求
    emit requestShowDetailedCard(m_card, m_lastMousePos);
}
