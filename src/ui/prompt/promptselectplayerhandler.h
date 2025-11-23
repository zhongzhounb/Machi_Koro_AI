#ifndef PROMPT_SELECT_PLAYER_HANDLER_H
#define PROMPT_SELECT_PLAYER_HANDLER_H

#include "PromptHandlerBase.h"
#include <QTimer>

class PromptSelectPlayerHandler : public PromptHandlerBase {
    Q_OBJECT
public:
    explicit PromptSelectPlayerHandler(MainWindow* main);

    void handle(const PromptData& pd) override;
};

#endif
