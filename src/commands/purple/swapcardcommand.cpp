#include "swapcardcommand.h"
#include "player.h"
#include "card.h"
#include "gamestate.h"
#include "gamecontroller.h"

SwapCardCommand::SwapCardCommand(Player* sourcePlayer, Card* card, QObject* parent, bool isFailed, const QString& failureMessage)
    : GameCommand(CommandType::SwapCard, sourcePlayer,parent,card,nullptr,isFailed,failureMessage){
}

// 检查是否需要用户交互
bool SwapCardCommand::isNeedInput()const {
    return true;
};

void SwapCardCommand::execute(GameState* state, GameController* controller) {
    //读取选项
    QVariantList chooes=m_userChoice.value("valueList").toList();
    int cardId1=chooes.indexOf(0);
    int cardId2=chooes.indexOf(1);
    Card* card1=nullptr, *card2=nullptr;
    Player* player2=nullptr;
    //找这两张卡在哪
    for(Player* player:state->getPlayers())
        for(QList<Card*>cards:player->getCards())
            if(cards.first()->getId()==cardId1)
                card1=cards.first();
            else if(cards.first()->getId()==cardId2){
                card2=cards.first();
                player2=player;
            }
    //日志记录
    m_cardName1=card1->getName();
    m_cardName2=card2->getName();
    m_playerName=player2->getName();
    //操作
    m_sourcePlayer->addCard(card2);
    player2->addCard(card1);
    m_sourcePlayer->delCard(card1);
    player2->delCard(card2);
}

QString SwapCardCommand::getLog()const {
    QString log=QString("【%1】 %2 ").arg(m_card->getName()).arg(m_sourcePlayer->getName());

    if(m_isFailed)
        log+=QString("%1。").arg(m_failureMessage);
    else
        log+=QString("用【%1】交换了 %2 的【%3】。").arg(m_cardName1).arg(m_playerName).arg(m_cardName2);

    return log;
}
