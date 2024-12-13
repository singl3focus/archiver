#ifndef MENUBAR_H
#define MENUBAR_H

#include <QMainWindow>

class MenuBar {
public:
    MenuBar(QMainWindow *parent = nullptr);
    void setupMenuBar(QMenuBar *menuBar); // Метод для настройки меню

    // Методы для получения действий
    QAction* getOpenAction() const { return openAction; }
    QAction* getCopyAction() const { return copyAction; }
    QAction* getPasteAction() const { return pasteAction; }
    QAction* getHighliteAllAction() const { return highliteAllAction; }
    QAction* getNoneHighliteAction() const { return noneHighliteAction; }
    QAction* getExitAction() const { return exitAction; }

    QAction* getAddAction() const { return addAction; }
    QAction* getExtratToFolAction() const { return extratToFolAction; }
    QAction* getShowAction() const { return showAction; }
    QAction* getDeleteAction() const { return deleteAction; }
    QAction* getRenameAction() const { return renameAction; }
    QAction* getInfoAction() const { return infoAction; }

    QAction* getManualAction() const { return manualAction; }
    QAction* getAboutAction() const { return aboutAction; }

private:
    QMainWindow *mainWindow; // Указатель на главное окно

    // Указатели на действия
    QAction *openAction = nullptr;
    QAction *copyAction = nullptr;
    QAction *pasteAction = nullptr;
    QAction *highliteAllAction = nullptr;
    QAction *noneHighliteAction = nullptr;
    QAction *exitAction = nullptr;

    QAction *addAction = nullptr;
    QAction *extratToFolAction = nullptr;
    QAction *showAction = nullptr;
    QAction *deleteAction = nullptr;
    QAction *renameAction = nullptr;
    QAction *infoAction = nullptr;

    QAction *manualAction = nullptr;
    QAction *aboutAction = nullptr;

    void setupFileMenu(QMenuBar *menuBar);
    void setupCommandsMenu(QMenuBar *menuBar);
    void setupHelpMenu(QMenuBar *menuBar);
};

#endif // MENUBAR_H
