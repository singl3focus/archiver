#ifndef TOOLBAR_H
#define TOOLBAR_H

#include <QMainWindow>

class ToolBar {
public:
    ToolBar(QMainWindow *parent = nullptr);
    void setupToolBar(QToolBar *toolBar);

    QToolButton* getAddButton() const { return addButton; }
    QToolButton* getExtractButton() const { return extractButton; }
    QToolButton* getViewButton() const { return viewButton; }
    QToolButton* getDeleteButton() const { return deleteButton; }
    QToolButton* getInfoButton() const { return infoButton; }

private:
    QMainWindow *mainWindow; // Указатель на главное окно

    QToolButton *addButton = nullptr;
    QToolButton *extractButton = nullptr;
    QToolButton *viewButton = nullptr;
    QToolButton *deleteButton = nullptr;
    QToolButton *infoButton = nullptr;
};

#endif // TOOLBAR_H
