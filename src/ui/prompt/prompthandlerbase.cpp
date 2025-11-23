#include "prompthandlerbase.h"
#include "mainwindow.h"

PromptHandlerBase::PromptHandlerBase(MainWindow* main)
    : QObject(main)
    , m_main(main)
{
}
