#include "gamecommand.h"
#include "gamestate.h" // 需要访问 GameState 来查找 Player*
#include "player.h"    // 需要 Player 类的定义
#include "card.h"
// 初始化静态成员变量
int GameCommand::s_commandId=0;

GameCommand::GameCommand(CommandType type,Player* sourcePlayer, QObject* parent,
                         Card* card,Player* activePlayer)
    : m_type(type),QObject(parent),  m_sourcePlayer(sourcePlayer),m_card(card),m_activePlayer(activePlayer){
    //自增id，用以回放
    m_id=s_commandId++;

    /*创建命令自动计算优先级*/
    //1.首先排命令优先级
    m_priority=type*10000;
    //2.命令相同则排相同名称卡
    if(card){
        m_priority+=card->getNameId()*100;
        //3.相同名称卡只有红卡有执行顺序，优先执行上家
        if(card->getColor()==Color::Red)
            m_priority+=(sourcePlayer->getId()-activePlayer->getId()+MAX_PLAYER_NUM)%MAX_PLAYER_NUM;
    }
}

PromptData GameCommand::getPromptData(GameState* state) const{
    PromptData pd;
    if(m_card){
        switch(m_currentStep){
        case 1:{
            pd.type=PromptData::CardInAnimation

        }
        }
    }
    else{
        return pd;
    }
};

bool GameCommand::setInput(int optionId,GameState* state,GameController* controller){
    if(m_card){

    }
    else{
        return true;
    }
};


