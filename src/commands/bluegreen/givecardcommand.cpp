#include "givecardcommand.h"
#include "player.h"
#include "card.h"
#include "gamestate.h"
#include "gamecontroller.h"

GiveCardCommand::GiveCardCommand(Player* sourcePlayer, Card* card, QObject* parent, bool isFailed, const QString& failureMessage)
    : GameCommand(CommandType::GiveCard, sourcePlayer,parent,card,nullptr,isFailed,failureMessage){
}

// 检查是否需要用户交互（可选交互：如果自己的地标都不够拆，直接全拆了，不用选择）
PromptData CloseLandmarkCommand::getPromptData(GameState* state) const {

};
// 获取默认选项（无选项时禁止调用）
int CloseLandmarkCommand::getAutoInput( const PromptData& promptData ,GameState* state) const {

};
// 设置选项，返回是否要继续获得选项（无选项时禁止调用）
bool CloseLandmarkCommand::setInput(int optionId,GameState* state) {

};

void GiveCardCommand::execute(GameState* state, GameController* controller) {
    //读取选项
    QVariantList choose=m_userChoice.value("valueList").toList();
    int cardId=choose.indexOf(0);
    int playerId=choose.indexOf(1);
    //目前没这个方法
    if(m_isFailed)
        return;
    for(QList<Card*> cards:m_sourcePlayer->getCards())
        if(cards.first()->getId()==cardId)
            for(Player* player:state->getPlayers())
                if(player->getId()==playerId){
                    m_cardName=cards.first()->getName();
                    m_playerName=player->getName();
                    //给卡
                    player->addCard(cards.first());
                    m_sourcePlayer->delCard(cards.first());
                    //计算收益
                    m_coinsSum=m_card->getValue();
                    //赚钱
                    m_sourcePlayer->addCoins(m_coinsSum);
                }

}

QString GiveCardCommand::getLog()const {
    QString log=QString("【%1】 %2 ").arg(m_card->getName()).arg(m_sourcePlayer->getName());

    if(m_isFailed)
        log+=QString("%1。").arg(m_failureMessage);
    else
        log+=QString("将【%1】赠与给 %2 ，并获得了 %3 金币。").arg(m_cardName).arg(m_playerName).arg(m_coinsSum);

    return log;
}
