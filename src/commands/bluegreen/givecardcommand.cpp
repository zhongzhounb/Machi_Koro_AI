#include "givecardcommand.h"
#include "player.h"
#include "card.h"
#include "gamestate.h"
#include "gamecontroller.h"
#include "randomutils.h"
#include "ai/ai.h"

GiveCardCommand::GiveCardCommand(Player* sourcePlayer, QObject* parent,QList<Card*> cards,Player* activePlayer)
    : GameCommand(CommandType::GiveCard, sourcePlayer,parent,cards,activePlayer){
}

PromptData GiveCardCommand::getPromptData(GameState* state) const{
    PromptData pt;
    switch (m_currentStep){
    case 1:{//选择卡阶段
        pt.type=PromptData::SelectCard;
        pt.promptMessage=QString("请选择一张你需要给出的卡牌");
        for(QList<Card*> cards:m_sourcePlayer->getCards()){
            Card* card=cards.last();
            if(card->getType()==Type::Landmark)
                pt.options.append(OptionData{card->getId(),card->getName(),0,"你不能将地标建筑给予其他玩家。"});
            if(card->getType()==Type::Office)
                pt.options.append(OptionData{card->getId(),card->getName(),0,"你不能将紫色建筑给予其他玩家。"});
            else
                pt.options.append(OptionData{card->getId(),card->getName(),1,""});
        }
        //设置默认选项
        pt.autoInput=state->getAI()->getWorstSelfCardId(pt,m_sourcePlayer,state);
        return pt;
    }
    case 2:{//选择玩家阶段
        pt.type=PromptData::SelectPlayer;
        pt.promptMessage=QString("请选择将%1赠予的玩家").arg(state->getCard(m_userInput[0])->getName());
        for(Player* player:state->getPlayers())
            if(player!=m_sourcePlayer)
                pt.options.append(OptionData{player->getId(),player->getName(),1,""});
        //设置默认选项
        pt.autoInput=state->getAI()->getLastPlayerId(pt,m_sourcePlayer,state);
        return pt;
    }
    case 3:{//确认阶段
        pt.type=PromptData::Popup;
        pt.promptMessage=QString("确认要将%1给予给%2吗？").arg(state->getCard(m_userInput[0])->getName()).arg(state->getPlayer(m_userInput[1])->getName());
        pt.options.append(OptionData{1,"确定",1,""});
        pt.options.append(OptionData{0,"重新选择",1,""});
        return pt;
    }
    }

    return pt;
}

// todo：这个不能简单乘操作，需要一张一张卡实现
bool GiveCardCommand::setInput(int optionId,GameState* state, GameController* controller) {
    switch (m_currentStep){
    case 1:{//选择卡阶段
        m_userInput.append(optionId);
        m_currentStep=2;
        return false;
    }
    case 2:{//选择玩家阶段
        m_userInput.append(optionId);
        m_currentStep=3;
        return false;
    }
    case 3:{//选择玩家阶段
        //确认则执行完毕
        if(optionId==1){
            int cardNum=m_sourcePlayer->getCardNum(m_card->getName(),State::Opening);
            //如果完成所有的卡牌
            if(m_userInput.size()==cardNum*2){
                execute(state,controller);
                return true;
            }
            //否则返回1阶段
            m_currentStep=1;
            return false;
        }

        //否则重新选择
        m_userInput.pop_back();
        m_userInput.pop_back();
        m_currentStep=1;
        return false;
    }
    }
    return true;
};

void GiveCardCommand::execute(GameState* state, GameController* controller) {
    //计算有多少卡牌
    int cardNum=m_sourcePlayer->getCardNum(m_card->getName(),State::Opening);
    //计算有多少组合
    int comboNum=m_card->getComboNum(m_sourcePlayer,m_sourcePlayer,state);
    //计算收益
    int coinsSum=cardNum*comboNum*m_card->getValue();
    //赚钱
    m_sourcePlayer->addCoins(coinsSum);

    QString log=QString("【%1】%2 %3").arg(m_card->getName())
                      .arg(cardNum==1?"":QString("*%1").arg(cardNum))
                      .arg(m_sourcePlayer->getName());
    //换卡（貌似要在
    for(int i=0;i<m_userInput.size();i+=2){
        int cardId=m_userInput[i];
        int playerId=m_userInput[i+1];
        for(QList<Card*> cards:m_sourcePlayer->getCards()){
            Card* card=cards.last();
            if(card->getId()==cardId)
                for(Player* player:state->getPlayers())
                    if(player->getId()==playerId){
                        log+=QString("将【%1】增予%2，").arg(card->getName()).arg(player->getName());
                        //给卡
                        player->addCard(cards.first());
                        m_sourcePlayer->delCard(cards.first());

                    }
        }
    }

    state->addLog(log);

}
