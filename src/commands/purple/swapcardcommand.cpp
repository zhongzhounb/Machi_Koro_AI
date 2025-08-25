#include "swapcardcommand.h"
#include "player.h"
#include "card.h"
#include "gamestate.h"
#include "gamecontroller.h"
#include "randomutils.h"
#include "ai/ai.h"
SwapCardCommand::SwapCardCommand(Player* sourcePlayer, QObject* parent,QList<Card*> cards,Player* activePlayer)
    : GameCommand(CommandType::SwapCard, sourcePlayer,parent,cards,activePlayer){
}

PromptData SwapCardCommand::getPromptData(GameState* state) const{
    PromptData pt;
    switch (m_currentStep){
    case 1:{//选择卡阶段
        pt.type=PromptData::SelectCard;
        pt.promptMessage=QString("请选择一张你需要交换出去的卡牌");
        for(QList<Card*> cards:m_sourcePlayer->getCards()){
            Card* card=cards.last();
            if(card->getType()==Type::Landmark)
                pt.options.append(OptionData{card->getId(),card->getName(),0,"你不能交换地标建筑。"});
            else if(card->getType()==Type::Office)
                pt.options.append(OptionData{card->getId(),card->getName(),0,"你不能交换紫色建筑。"});
            else
                pt.options.append(OptionData{card->getId(),card->getName(),1,""});
        }
        //设置默认选项
        pt.autoInput=state->getAI()->getWorstSelfCardId(pt,m_sourcePlayer,state);
        return pt;
    }
    case 2:{//选择阶段
        pt.type=PromptData::SelectCard;
        pt.promptMessage=QString("请选择一张你需要交换回来的卡牌");
        for(Player* player:state->getPlayers())
            if(player!=m_sourcePlayer)
                for(QList<Card*> cards:player->getCards()){
                    Card* card=cards.last();
                    if(card->getType()==Type::Landmark)
                        pt.options.append(OptionData{card->getId(),card->getName(),0,"你不能交换地标建筑。"});
                    else if(card->getType()==Type::Office)
                        pt.options.append(OptionData{card->getId(),card->getName(),0,"你不能交换紫色建筑。"});
                    else
                        pt.options.append(OptionData{card->getId(),card->getName(),1,""});
                }
        //设置默认选项
        pt.autoInput=state->getAI()->getBestOtherCardId(pt,m_sourcePlayer,state);
        return pt;
    }
    case 3:{//确认阶段
        pt.type=PromptData::Popup;
        pt.promptMessage=QString("确认要将【%1】与【%2】交换吗？").arg(state->getCard(m_userInput[0])->getName()).arg(state->getCard(m_userInput[1])->getName());
        pt.options.append(OptionData{1,"确定",1,""});
        pt.options.append(OptionData{0,"重新选择",1,""});
        return pt;
    }
    }

    return pt;
}

// 设置选项，返回是否要继续获得选项（无选项时禁止调用）
bool SwapCardCommand::setInput(int optionId,GameState* state,GameController* controller) {
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
            execute(state,controller);
            return true;
        }

        //否则重新选择
        m_userInput.clear();
        m_currentStep=1;
        return false;
    }
    }
    return true;
};

void SwapCardCommand::execute(GameState* state, GameController* controller) {
    //读取选项
    int cardId1=m_userInput[0];
    int cardId2=m_userInput[1];
    Card* card1=nullptr, *card2=nullptr;
    Player* player2=nullptr;
    //找这两张卡在哪
    for(Player* player:state->getPlayers())
        for(QList<Card*>cards:player->getCards())
            if(cards.last()->getId()==cardId1)
                card1=cards.last();
            else if(cards.last()->getId()==cardId2){
                card2=cards.last();
                player2=player;
            }

    //操作
    m_sourcePlayer->addCard(card2);
    player2->addCard(card1);
    m_sourcePlayer->delCard(card1);
    player2->delCard(card2);

    //日志
    QString log=QString("【%1】 %2 ").arg(m_card->getName()).arg(m_sourcePlayer->getName());
    log+=QString("用【%1】交换了 %2 的【%3】。").arg(card1->getName()).arg(card2->getName()).arg(player2->getName());
}

