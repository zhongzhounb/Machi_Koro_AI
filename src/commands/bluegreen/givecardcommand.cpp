#include "givecardcommand.h"
#include "player.h"
#include "card.h"
#include "gamestate.h"
#include "gamecontroller.h"
#include "ai/ai.h"

GiveCardCommand::GiveCardCommand(Player* sourcePlayer, QObject* parent,QList<Card*> cards,Player* activePlayer)
    : GameCommand(CommandType::GiveCard, sourcePlayer,parent,cards,activePlayer){
}

//此command与其他卡牌不同，非常特殊，因为可以给出自己，又可以给出多张，则：
//设计一：选择卡1→选择卡2→选择需要将卡1给的对象→选择需要将卡2给的对象→执行
//问题：因为前端原因写死了只能点击卡片顶部，所以每种卡只能给一张
//设计二：选择卡1→选择需要将卡1给的对象→执行→选择卡2→选择需要将卡2给的对象→执行
//问题：打破了最后执行的规则，但目前就这样设计最佳
PromptData GiveCardCommand::getPromptData(GameState* state) const{
    PromptData pt;
    pt.waitMessage=m_sourcePlayer->getName()+"正在选择给出的卡牌";
    switch (m_currentStep){
    case 1:{//选择卡阶段
        pt.type=PromptData::SelectCard;
        pt.promptMessage=QString("请选择第%1张你需要给出的卡牌").arg(m_userInput.size()/2+1);
        for(QList<Card*> cards:m_sourcePlayer->getCards()){
            Card* card=cards.last();
            if(card->getType()==Type::Landmark)
                continue;
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
        pt.type=PromptData::Popup;
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

//这个不能简单乘操作，需要一张一张卡实现
//需要拷贝一份【搬家公司】的数量，因为【搬家公司】在执行完把自己搬走后，执行次数就会变少，理论上执行次数是不变的。
bool GiveCardCommand::setInput(int optionId,GameState* state, GameController* controller) {
    switch (m_currentStep){
    case 1:{//选择卡阶段
        //因为先调用的getPromptData是const，所以在此存个副本，且只存一次，又因为不可能等于0，所以初始化0
        if(m_cardNum==0)
            m_cardNum=m_sourcePlayer->getCardNum(m_card->getName(),State::Opening);
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
            //先执行，每次读入存入的后两个数
            execute(state,controller);
            //如果完成所有的卡牌（如果大于的话肯定是bug）
            if(m_userInput.size()==m_cardNum*2)
                return true;
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
    //注意此处每个execute只算单张卡
    //计算有多少组合
    int comboNum=m_card->getComboNum(m_sourcePlayer,m_sourcePlayer,state);
    //计算收益
    int coinsSum=comboNum*m_card->getValue();
    //赚钱
    m_sourcePlayer->addCoins(coinsSum);

    QString log=QString("【%1】%2").arg(m_card->getName())
                      .arg(m_sourcePlayer->getName());

    //换卡（读取最后两个位置）
    int pos=m_userInput.size();
    int cardId=m_userInput[pos-2];
    int playerId=m_userInput[pos-1];
    for(QList<Card*> cards:m_sourcePlayer->getCards()){
        Card* card=cards.last();
        if(card->getId()==cardId)
            for(Player* player:state->getPlayers())
                if(player->getId()==playerId){
                    log+=QString("将【%1】增予%2，").arg(card->getName()).arg(player->getName());
                    //给卡
                    player->addCard(card);
                    m_sourcePlayer->delCard(card);
                }
    }

    log+=QString("获得 %1 金币。").arg(coinsSum);

    state->addLog(log);

}
