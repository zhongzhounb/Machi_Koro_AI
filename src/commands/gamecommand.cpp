#include "gamecommand.h"
#include "gamestate.h" // 需要访问 GameState 来查找 Player*
#include "player.h"    // 需要 Player 类的定义

GameCommand::GameCommand(CommandType type, int priority, Player* sourcePlayer, QObject* parent)
    : QObject(parent)
    , m_type(type)
    , m_priority(priority)
    , m_sourcePlayer(sourcePlayer)
{}

// 通用序列化：保存命令类型、优先级和来源玩家
QVariantMap GameCommand::serialize() const {
    QVariantMap data;
    data["type"] = static_cast<int>(m_type);
    data["priority"] = static_cast<int>(m_priority);
    if (m_sourcePlayer) {
        data["sourcePlayerId"] = m_sourcePlayer->getId();
    }
    if (requiresUserInput()) {
        data["userChoice"] = m_userChoice; // 保存用户选择
    }
    return data;
}

// 通用反序列化：恢复命令类型、优先级和来源玩家
void GameCommand::deserialize(const QVariantMap& data, GameState* state) {
    m_type = static_cast<CommandType>(data["type"].toInt());
    m_priority = static_cast<int>(data["priority"].toInt());
    // sourcePlayer 需要在 CommandFactory 创建实例后，由 GameController 传入 GameState 来恢复
    if (data.contains("sourcePlayerId")) {
todo://m_sourcePlayer = idToPlayer(data["sourcePlayerId"].toString(), state);
    }
    if (requiresUserInput() && data.contains("userChoice")) {
        m_userChoice = data["userChoice"];
    }
}
