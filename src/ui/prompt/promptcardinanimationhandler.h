#ifndef PROMPT_CARD_IN_ANIMATION_HANDLER_H
#define PROMPT_CARD_IN_ANIMATION_HANDLER_H

#include "PromptHandlerBase.h"
#include <QParallelAnimationGroup>
#include <QPropertyAnimation>
#include <QPointer>

class SlotWidget;
class CardWidget;
class Player;
struct PlayerLayoutConfig;

class PromptCardInAnimationHandler : public PromptHandlerBase {
    Q_OBJECT
public:
    explicit PromptCardInAnimationHandler(MainWindow* main);

    void handle(const PromptData& pd) override;
};

#endif
