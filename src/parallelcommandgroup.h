#ifndef PARALLELCOMMANDGROUP_H
#define PARALLELCOMMANDGROUP_H

#include <QObject>
#include <QList>
#include <QTimer>
#include "gamecommand.h"
#include "gamestate.h"
#include "gamecontroller.h"

class ParallelCommandGroup : public QObject {
    Q_OBJECT
public:
    explicit ParallelCommandGroup(QObject* parent = nullptr);

    void addCommand(GameCommand* cmd);
    void start(GameState* state, GameController* controller);

signals:
    void finished(GameCommand* representative);

private slots:
    void executeNext();
    void executeNextPhase();

private:
    QList<GameCommand*> m_commands;
    GameState* m_state = nullptr;
    GameController* m_controller = nullptr;

    int m_currentIndex = 0;
    int m_phase = 1; // 1 = 第一次执行, 2 = 第二次执行
    int m_finishedCount = 0;
};

#endif // PARALLELCOMMANDGROUP_H
