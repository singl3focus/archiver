#include "MenuBar.h"
#include <QMenuBar>
#include <QAction>

MenuBar::MenuBar(QMainWindow *parent)
    : mainWindow(parent) {}

void MenuBar::setupMenuBar(QMenuBar *menuBar) {
    setupFileMenu(menuBar);
    setupCommandsMenu(menuBar);
    setupHelpMenu(menuBar);
}

void MenuBar::setupFileMenu(QMenuBar *menuBar) {
    QMenu *fileMenu = menuBar->addMenu(QObject::tr("&File"));

    //Список пунктов File
    openAction = new QAction(QObject::tr("&Open archive"), fileMenu);
    openAction->setShortcut(QKeySequence(Qt::CTRL | Qt::Key_O));
    fileMenu->addAction(openAction);

    copyAction = new QAction(QObject::tr("&Copy"), fileMenu);
    copyAction->setShortcut(QKeySequence(Qt::CTRL | Qt::Key_C));
    fileMenu->addAction(copyAction);

    pasteAction = new QAction(QObject::tr("&Paste"), fileMenu);
    pasteAction->setShortcut(QKeySequence(Qt::CTRL | Qt::Key_V));
    fileMenu->addAction(pasteAction);

    highliteAllAction = new QAction(QObject::tr("&Highlite all"), fileMenu);
    highliteAllAction->setShortcut(QKeySequence(Qt::CTRL | Qt::Key_A));
    fileMenu->addAction(highliteAllAction);

    noneHighliteAction = new QAction(QObject::tr("&None highlite"), fileMenu);
    noneHighliteAction->setShortcut(QKeySequence(Qt::CTRL | Qt::Key_Minus));
    fileMenu->addAction(noneHighliteAction);

    exitAction = new QAction(QObject::tr("&Exit"), fileMenu);
    fileMenu->addAction(exitAction);
}

void MenuBar::setupCommandsMenu(QMenuBar *menuBar) {
    QMenu *commandsMenu = menuBar->addMenu(QObject::tr("&Commands"));

    //Список пунктов Commands
    addAction = new QAction(QObject::tr("&Add to an archive"), commandsMenu);
    addAction->setShortcut(QKeySequence(Qt::ALT | Qt::Key_A));
    commandsMenu->addAction(addAction);

    extratToFolAction = new QAction(QObject::tr("&Extract to folder"), commandsMenu);
    extratToFolAction->setShortcut(QKeySequence(Qt::ALT | Qt::Key_E));
    commandsMenu->addAction(extratToFolAction);

    showAction = new QAction(QObject::tr("&Show file contents"), commandsMenu);
    showAction->setShortcut(QKeySequence(Qt::ALT | Qt::Key_V));
    commandsMenu->addAction(showAction);

    deleteAction = new QAction(QObject::tr("&Delete files/folders"), commandsMenu);
    deleteAction->setShortcut(QKeySequence(Qt::Key_Delete));
    commandsMenu->addAction(deleteAction);

    renameAction = new QAction(QObject::tr("Rename files/folders&"), commandsMenu);
    renameAction->setShortcut(QKeySequence(Qt::Key_F2));
    commandsMenu->addAction(renameAction);

    infoAction = new QAction(QObject::tr("&Show information"), commandsMenu);
    infoAction->setShortcut(QKeySequence(Qt::ALT | Qt::Key_I));
    commandsMenu->addAction(infoAction);
}

void MenuBar::setupHelpMenu(QMenuBar *menuBar){
    QMenu *helpMenu = menuBar->addMenu(QObject::tr("&Help"));

    //Список пунктов Help
    manualAction = new QAction(QObject::tr("&Manual"), helpMenu);
    helpMenu->addAction(manualAction);

    aboutAction = new QAction(QObject::tr("&About the program"), helpMenu);
    helpMenu->addAction(aboutAction);
}
