#include <QApplication>
#include <QTreeView>
#include <QFileSystemModel>
#include <QMenu>
#include <QMenuBar>
#include <QMainWindow>
#include <QAction>
#include <QHeaderView>
#include <QMessageBox>
#include <QToolButton>
#include <QToolBar>
#include <QIcon>
#include <QLineEdit>
#include <QVBoxLayout>
#include <QDir>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    QMainWindow mainWindow;

    QMenuBar *menuBar = mainWindow.menuBar();
    QMenu *fileMenu = menuBar->addMenu(QObject::tr("&File"));
    QMenu *commandsMenu = menuBar->addMenu(QObject::tr("&Commands"));
    QMenu *operationsMenu = menuBar->addMenu(QObject::tr("&Operations"));
    QMenu *helpMenu = menuBar->addMenu(QObject::tr("&Help"));



    //Список пунктов File
    QAction *openAction = new QAction(QObject::tr("&Open archive"), &mainWindow);
    openAction->setShortcut(QKeySequence(Qt::CTRL | Qt::Key_O));
    fileMenu->addAction(openAction);

    QAction *chooseAction = new QAction(QObject::tr("&Choose CD"), &mainWindow);
    chooseAction->setShortcut(QKeySequence(Qt::CTRL | Qt::Key_D));
    fileMenu->addAction(chooseAction);

    QAction *defaultAction = new QAction(QObject::tr("&Set as default"), &mainWindow);
    defaultAction->setShortcut(QKeySequence(Qt::CTRL | Qt::Key_P));
    fileMenu->addAction(defaultAction);

    QAction *copyAction = new QAction(QObject::tr("&Copy"), &mainWindow);
    copyAction->setShortcut(QKeySequence(Qt::CTRL | Qt::Key_C));
    fileMenu->addAction(copyAction);

    QAction *pasteAction = new QAction(QObject::tr("&Paste"), &mainWindow);
    pasteAction->setShortcut(QKeySequence(Qt::CTRL | Qt::Key_V));
    fileMenu->addAction(pasteAction);

    QAction *highliteAllAction = new QAction(QObject::tr("&Highlite all"), &mainWindow);
    highliteAllAction->setShortcut(QKeySequence(Qt::CTRL | Qt::Key_A));
    fileMenu->addAction(highliteAllAction);

    QAction *highliteGroupAction = new QAction(QObject::tr("&Highlite group"), &mainWindow);
    highliteGroupAction->setShortcut(QKeySequence(Qt::Key_Plus));
    fileMenu->addAction(highliteGroupAction);

    QAction *noneHighliteAction = new QAction(QObject::tr("&None Highlite"), &mainWindow);
    noneHighliteAction->setShortcut(QKeySequence(Qt::Key_Minus));
    fileMenu->addAction(noneHighliteAction);

    QAction *exitAction = new QAction(QObject::tr("&Exit"), &mainWindow);
    fileMenu->addAction(exitAction);

    QObject::connect(exitAction, &QAction::triggered, &a, &QApplication::quit);



    //Список пунктов Commands
    QAction *addAction = new QAction(QObject::tr("&Add to an archive"), &mainWindow);
    addAction->setShortcut(QKeySequence(Qt::ALT | Qt::Key_A));
    commandsMenu->addAction(addAction);

    QAction *extratToFolAction = new QAction(QObject::tr("&Extract to folder"), &mainWindow);
    extratToFolAction->setShortcut(QKeySequence(Qt::ALT | Qt::Key_E));
    commandsMenu->addAction(extratToFolAction);

    QAction *testAction = new QAction(QObject::tr("&Test files in archive"), &mainWindow);
    testAction->setShortcut(QKeySequence(Qt::ALT | Qt::Key_T));
    commandsMenu->addAction(testAction);

    QAction *showAction = new QAction(QObject::tr("&Show file contents"), &mainWindow);
    showAction->setShortcut(QKeySequence(Qt::ALT | Qt::Key_V));
    commandsMenu->addAction(showAction);

    QAction *deleteAction = new QAction(QObject::tr("&Delete files/folders"), &mainWindow);
    deleteAction->setShortcut(QKeySequence(Qt::Key_Delete));
    commandsMenu->addAction(deleteAction);

    QAction *renameAction = new QAction(QObject::tr("Rename files/folders&"), &mainWindow);
    renameAction->setShortcut(QKeySequence(Qt::Key_F2));
    commandsMenu->addAction(renameAction);

    QAction *extractWithoutConfirmAction = new QAction(QObject::tr("&Extract without confirmation"), &mainWindow);
    extractWithoutConfirmAction->setShortcut(QKeySequence(Qt::ALT | Qt::Key_W));
    commandsMenu->addAction(extractWithoutConfirmAction);

    QAction *commAction = new QAction(QObject::tr("&Add archive comment"), &mainWindow);
    commAction->setShortcut(QKeySequence(Qt::ALT | Qt::Key_M));
    commandsMenu->addAction(commAction);

    QAction *blockAction = new QAction(QObject::tr("&Block archive"), &mainWindow);
    blockAction->setShortcut(QKeySequence(Qt::ALT | Qt::Key_L));
    commandsMenu->addAction(blockAction);



    //Список пунктов Operations
    QAction *searchAction = new QAction(QObject::tr("&Find files"), &mainWindow);
    searchAction->setShortcut(QKeySequence(Qt::Key_F3));
    operationsMenu->addAction(searchAction);

    QAction *infoAction = new QAction(QObject::tr("&Show info"), &mainWindow);
    infoAction->setShortcut(QKeySequence(Qt::ALT | Qt::Key_I));
    operationsMenu->addAction(infoAction);

    QAction *repAction = new QAction(QObject::tr("&Сreate a report"), &mainWindow);
    repAction->setShortcut(QKeySequence(Qt::ALT | Qt::Key_G));
    operationsMenu->addAction(repAction);

    QAction *benchmarkAction = new QAction(QObject::tr("&Benchmark"), &mainWindow);
    benchmarkAction->setShortcut(QKeySequence(Qt::ALT | Qt::Key_B));
    operationsMenu->addAction(benchmarkAction);



    //Список пунктов Help
    QAction *contentAction = new QAction(QObject::tr("&Content"), &mainWindow);
    helpMenu->addAction(contentAction);

    QAction *aboutAction = new QAction(QObject::tr("&About the program..."), &mainWindow);
    helpMenu->addAction(aboutAction);

    QObject::connect(aboutAction, &QAction::triggered, [&mainWindow]() {
        QMessageBox::about(&mainWindow, QObject::tr("About the program SUAI Archiver"), QObject::tr("SUAI Archiver\nVersion 1.0\n\nThis program is developed to work with file archives"));
    });



    //Панель инструментов
    QToolBar *toolBar = new QToolBar(&mainWindow);
    mainWindow.addToolBar(toolBar);

    QToolButton *addButton = new QToolButton(&mainWindow);
    addButton->setText(QObject::tr("Add"));
    addButton->setIcon(QIcon (":/icons/icons/add.svg"));
    addButton->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
    toolBar->addWidget(addButton);

    QToolButton *extractButton = new QToolButton(&mainWindow);
    extractButton->setText(QObject::tr("Extract to"));
    extractButton->setIcon(QIcon (":/icons/icons/extract.svg"));
    extractButton->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
    toolBar->addWidget(extractButton);

    QToolButton *testButton = new QToolButton(&mainWindow);
    testButton->setText(QObject::tr("Test"));
    testButton->setIcon(QIcon (":/icons/icons/test.svg"));
    testButton->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
    toolBar->addWidget(testButton);

    QToolButton *viewButton = new QToolButton(&mainWindow);
    viewButton->setText(QObject::tr("View"));
    viewButton->setIcon(QIcon (":/icons/icons/view.svg"));
    viewButton->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
    toolBar->addWidget(viewButton);

    QToolButton *deleteButton = new QToolButton(&mainWindow);
    deleteButton->setText(QObject::tr("Delete"));
    deleteButton->setIcon(QIcon (":/icons/icons/delete.svg"));
    deleteButton->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
    toolBar->addWidget(deleteButton);

    QToolButton *searchButton = new QToolButton(&mainWindow);
    searchButton->setText(QObject::tr("Search"));
    searchButton->setIcon(QIcon (":/icons/icons/search.svg"));
    searchButton->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
    toolBar->addWidget(searchButton);

    QToolButton *infoButton = new QToolButton(&mainWindow);
    infoButton->setText(QObject::tr("Information"));
    infoButton->setIcon(QIcon (":/icons/icons/information.svg"));
    infoButton->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
    toolBar->addWidget(infoButton);



    //Стили
    QString styleSheet = R"(

        QMenuBar {
            background-color: #323232;
        }

        QMenu {
            background-color: #323232;
            border: 1px solid #525252;
            border-radius: 5px;
        }

        QMenu::item {
            padding: 5px 20px;
        }

        QMenu::item:selected {
            background-color: #404040;
            border-radius: 5px;
        }

        QToolButton {
            padding: 10px;
            color: #FFFFFF;
            border-radius: 5px;
        }

        QToolButton:hover {
            color: #BB86FC;
            background-color: #323232;
        }

        QTreeView {
            background-color: #121212;
        }

        QHeaderView::section {
            background-color: #323232;
        }

        QTreeView::item:selected {
            background-color: #BB86FC;
            color: #000000;
        }

    )";

    mainWindow.setStyleSheet(styleSheet);

    QFileSystemModel *model = new QFileSystemModel();
    model->setRootPath(QDir::homePath());

    QTreeView *tree = new QTreeView();
    tree->setModel(model);
    tree->setRootIndex(model->index(QDir::homePath()));
    tree->setExpandsOnDoubleClick(false);
    tree->setItemsExpandable(false);
    tree->setRootIsDecorated(false);

    QLineEdit *pathLineEdit = new QLineEdit();
    pathLineEdit->setReadOnly(false);
    pathLineEdit->setText(QDir::homePath());

    QToolButton *backButton = new QToolButton(&mainWindow);
    backButton->setIcon(QIcon(":/img/img/back.svg"));

    backButton->setStyleSheet(R"(

        QToolButton {
            padding: 5px 5px;
        }

        QToolButton:hover {

        }

    )");

    QObject::connect(backButton, &QToolButton::clicked, [&]() {
        QModelIndex currentRoot = tree->rootIndex();
        QModelIndex parentIndex = model->parent(currentRoot);

        if (parentIndex.isValid()) {
            tree->setRootIndex(parentIndex);
            QString path = model->filePath(parentIndex);
            pathLineEdit->setText(path);
        }
    });

    QObject::connect(tree, &QTreeView::doubleClicked, [&](const QModelIndex &index) {
        if (model->isDir(index)) {
            tree->setRootIndex(index);
            QString path = model->filePath(index);
            pathLineEdit->setText(path);
        }
    });

    QObject::connect(pathLineEdit, &QLineEdit::returnPressed, [&]() {
        QString newPath = pathLineEdit->text();
        QDir dir(newPath);
        if (dir.exists()) {
            tree->setRootIndex(model->index(newPath));
        } else {
            QMessageBox::warning(&mainWindow, QObject::tr("Error"), QObject::tr("the specified path does not exist!"));
        }
    });

    QWidget *centralWidget = new QWidget();
    QVBoxLayout *mainLayout = new QVBoxLayout(centralWidget);

    QHBoxLayout *pathLayout = new QHBoxLayout();
    pathLayout->addWidget(backButton);
    pathLayout->addWidget(pathLineEdit);

    mainLayout->addLayout(pathLayout);  // Добавляем горизонтальную компоновку в основную вертикальную компоновку
    mainLayout->addWidget(tree);
    mainWindow.setCentralWidget(centralWidget);

    mainWindow.setWindowTitle(QObject::tr("SUAI Archiver"));
    mainWindow.resize(1280,720);
    mainWindow.show();

    return a.exec();
}
