#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QLineEdit>

class QTreeView;
class CustomFileSystemModel;
class MenuBar;
class ToolBar;
class FileSystemWidget;

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    explicit MainWindow(QApplication *app, QWidget *parent = nullptr);

private:
    void setupMenuBar();
    void setupToolBar();
    void setupFileSystem();
    void setupActions();
    void setupStyles();

    QApplication *application;

    MenuBar *menu;
    ToolBar *tools;
    FileSystemWidget *fileSystem;

    QTreeView *tree;
    CustomFileSystemModel *model;
    QLineEdit *pathLineEdit;
    QToolButton *backButton;

    void connectActions();
};

#endif // MAINWINDOW_H
