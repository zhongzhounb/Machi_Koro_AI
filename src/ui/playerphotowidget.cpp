#include "playerphotowidget.h"
#include "player.h"
#include "coinswidget.h"

PlayerPhotoWidget::PlayerPhotoWidget(Player* player, QWidget* parent)
    :QWidget(parent)
    ,m_player(player)
    ,m_mainLayout(new QGridLayout(this))
    ,m_nameLabel(new QLabel(this))
    ,m_imgLabel(new QLabel(this))
    ,m_coinsLabel(new CoinsWidget(this)){

    //当前文字居中
    m_nameLabel->setAlignment(Qt::AlignCenter);
    m_nameLabel->setText(player->getName());

    m_coinsLabel->setCoinsNum(player->getCoins());

    m_mainLayout->addWidget(m_imgLabel,0,0,4,4);
    m_mainLayout->addWidget(m_nameLabel,4,0,1,5);
    m_mainLayout->addWidget(m_coinsLabel,2,3,2,2);

    setLayout(m_mainLayout);

}
PlayerPhotoWidget::~PlayerPhotoWidget(){};


void PlayerPhotoWidget::resizeEvent(QResizeEvent *event){
    QWidget::resizeEvent(event);


    const QSize allocatedSize = event->size();
    int width = allocatedSize.width();
    int height = allocatedSize.height();

    if (static_cast<double>(width) / m_aspectRatio < height) {
        height = static_cast<int>(static_cast<double>(width) / m_aspectRatio);
    } else {
        width = static_cast<int>(static_cast<double>(height) * m_aspectRatio);
    }

    QRect newGeometry(0, 0, width, height);
    newGeometry.moveCenter(QRect(QPoint(), allocatedSize).center());

    // 使用 setGeometry 来调整大小和位置，而不是 setFixedSize
    setGeometry(newGeometry);

    // 根据新的有效高度更新字体
    int fontSize = qMax(5, height / 9);
    m_nameLabel->setFont(QFont("YouYuan", fontSize, QFont::Bold));

};

void PlayerPhotoWidget::onCoinsChange(Player *player, int coins){
    if(player!=m_player)
        return;
    m_coinsLabel->setCoinsNum(player->getCoins());
};
