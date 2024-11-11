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
#include <QProcess>
#include <QFileDialog>
#include <QDialog>
#include <QComboBox>
#include <QLabel>
#include <QPushButton>
#include <QHBoxLayout>
#include <QDebug>

void runArchiverUtility(const QString &src, const QString &dst, const QString &format) {
    QProcess *process = new QProcess;

    // Аргументы для утилиты
    QStringList arguments;
    arguments << "--source" << src
              << "--destination" << dst
              << "--format" << format
              << "compress";

    // Подключаемся к сигналу readyReadStandardOutput для чтения вывода
    QObject::connect(process, &QProcess::readyReadStandardOutput, [process]() {
        QByteArray output = process->readAllStandardOutput();
        qDebug() << "Output:" << output;
    });

    // Подключаемся к сигналу finished для обработки завершения процесса
    QObject::connect(process, QOverload<int, QProcess::ExitStatus>::of(&QProcess::finished), [process](int exitCode, QProcess::ExitStatus exitStatus) {
        if (exitStatus == QProcess::NormalExit && exitCode == 0) {
            QMessageBox::information(nullptr, "Success", "Operation completed successfully!");
        } else {
            QByteArray errorOutput = process->readAllStandardError();
            qDebug() << "Error Output:" << errorOutput;
            QMessageBox::warning(nullptr, "Error", "Operation failed: " + QString(errorOutput));
        }
        process->deleteLater();
    });

    // Запускаем утилиту
    process->start("D:/archiver/C++/app/utility.exe", arguments);

    // Проверяем, что процесс успешно запущен
    if (!process->waitForStarted()) {
        QMessageBox::warning(nullptr, "Error", "Could not start the archiver utility.");
        delete process;
    }
}

void runExtractUtility(const QString &src, const QString &dst) {
    QProcess *process = new QProcess;

    // Аргументы для утилиты
    QStringList arguments;
    arguments << "--source" << src
              << "--destination" << dst
              << "decompress";

    // Подключаемся к сигналу readyReadStandardOutput для чтения вывода
    QObject::connect(process, &QProcess::readyReadStandardOutput, [process]() {
        QByteArray output = process->readAllStandardOutput();
        qDebug() << "Output:" << output;
    });

    // Подключаемся к сигналу finished для обработки завершения процесса
    QObject::connect(process, QOverload<int, QProcess::ExitStatus>::of(&QProcess::finished), [process](int exitCode, QProcess::ExitStatus exitStatus) {
        if (exitStatus == QProcess::NormalExit && exitCode == 0) {
            QMessageBox::information(nullptr, "Success", "Extraction completed successfully!");
        } else {
            QByteArray errorOutput = process->readAllStandardError();
            qDebug() << "Error Output:" << errorOutput;
            QMessageBox::warning(nullptr, "Error", "Extraction failed: " + QString(errorOutput));
        }
        process->deleteLater();
    });

    // Запускаем утилиту
    process->start("D:/archiver/C++/app/utility.exe", arguments);

    // Проверяем, что процесс успешно запущен
    if (!process->waitForStarted()) {
        QMessageBox::warning(nullptr, "Error", "Could not start the archiver utility.");
        delete process;
    }
}



// Функция для показа модального окна выбора пути и формата архивации
void showCompressionDialog(QWidget *parent, const QString &sourcePath) {
    QDialog dialog(parent);
    dialog.setWindowTitle("Choose Archive Path and Format");

    QLabel *labelPath = new QLabel("Save to:");
    QLineEdit *lineEditPath = new QLineEdit;
    QPushButton *browseButton = new QPushButton("Browse...");

    QHBoxLayout *pathLayout = new QHBoxLayout;
    pathLayout->addWidget(labelPath);
    pathLayout->addWidget(lineEditPath);
    pathLayout->addWidget(browseButton);

    QLabel *labelFormat = new QLabel("Format:");
    QComboBox *comboBoxFormat = new QComboBox;
    comboBoxFormat->addItems({"zip", "tar"});

    QHBoxLayout *formatLayout = new QHBoxLayout;
    formatLayout->addWidget(labelFormat);
    formatLayout->addWidget(comboBoxFormat);

    QPushButton *okButton = new QPushButton("OK");
    QPushButton *cancelButton = new QPushButton("Cancel");

    QHBoxLayout *buttonLayout = new QHBoxLayout;
    buttonLayout->addStretch();
    buttonLayout->addWidget(okButton);
    buttonLayout->addWidget(cancelButton);

    QVBoxLayout *mainLayout = new QVBoxLayout(&dialog);
    mainLayout->addLayout(pathLayout);
    mainLayout->addLayout(formatLayout);
    mainLayout->addLayout(buttonLayout);

    QObject::connect(browseButton, &QPushButton::clicked, [&]() {
        QString dir = QFileDialog::getExistingDirectory(&dialog, "Select Archive Destination", QDir::homePath());
        if (!dir.isEmpty()) {
            lineEditPath->setText(dir);
        }
    });

    QObject::connect(okButton, &QPushButton::clicked, [&]() {
        QString destinationPath = lineEditPath->text();
        QString format = comboBoxFormat->currentText();
        if (!destinationPath.isEmpty()) {
            dialog.accept();
            runArchiverUtility(sourcePath, destinationPath, format);
        } else {
            QMessageBox::warning(&dialog, "Error", "Please specify a valid path to save the archive.");
        }
    });

    QObject::connect(cancelButton, &QPushButton::clicked, &dialog, &QDialog::reject);

    dialog.exec();
}


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
    model->setRootPath(QDir::rootPath());

    QTreeView *tree = new QTreeView();
    tree->setModel(model);
    tree->setRootIndex(model->index("C:/"));
    tree->setExpandsOnDoubleClick(false);
    tree->setItemsExpandable(false);
    tree->setRootIsDecorated(false);

    // Создание QComboBox для выбора дисков
    QComboBox *diskComboBox = new QComboBox();
    foreach (const QFileInfo &drive, QDir::drives()) {
        diskComboBox->addItem(drive.absolutePath());
    }

    QString initialPath = QDir::drives().isEmpty() ? QString() : QDir::drives().first().absolutePath();
    QLineEdit *pathLineEdit = new QLineEdit();
    pathLineEdit->setReadOnly(false);
    pathLineEdit->setText(initialPath);

    QToolButton *backButton = new QToolButton(&mainWindow);
    backButton->setIcon(QIcon(":/icons/icons/back.svg"));

    backButton->setStyleSheet(R"(

        QToolButton {
            padding: 5px 5px;
        }

        QToolButton:hover {

        }

    )");

    // Логика для кнопки "back" (переход к родительской директории или к списку дисков)
    QObject::connect(backButton, &QToolButton::clicked, [&]() {
        QModelIndex currentRoot = tree->rootIndex();
        QString currentPath = model->filePath(currentRoot);

        // Проверка, является ли текущий путь корнем одного из дисков
        bool isRootDrive = false;
        foreach (const QFileInfo &drive, QDir::drives()) {
            if (drive.absolutePath() == currentPath) {
                isRootDrive = true;
                break;
            }
        }

        if (isRootDrive) {
            // Если текущий путь — корень диска, возвращаемся к отображению списка дисков
            tree->setRootIndex(QModelIndex());  // Сбрасываем на начальный уровень (отображение дисков)
            pathLineEdit->clear();  // Очищаем QLineEdit, если показываем список дисков
        } else {
            // Иначе поднимаемся на уровень выше
            QModelIndex parentIndex = model->parent(currentRoot);
            if (parentIndex.isValid()) {
                tree->setRootIndex(parentIndex);
                pathLineEdit->setText(model->filePath(parentIndex));  // Обновляем QLineEdit на новый путь
            }
        }
    });

    // Логика смены диска при выборе в ComboBox
    QObject::connect(diskComboBox, &QComboBox::currentTextChanged, [&](const QString &diskPath) {
        if (QDir(diskPath).exists()) {
            tree->setRootIndex(model->index(diskPath));
            pathLineEdit->setText(diskPath);  // Обновляем QLineEdit при смене диска
        } else {
            QMessageBox::warning(&mainWindow, QObject::tr("Error"), QObject::tr("The selected disk is not accessible!"));
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


    QObject::connect(addButton, &QToolButton::clicked, [&]() {
        QModelIndex selectedIndex = tree->selectionModel()->currentIndex();
        if (selectedIndex.isValid() && model->isDir(selectedIndex)) {
            QString folderPath = model->filePath(selectedIndex);

            // Теперь вызываем диалог для выбора пути и формата архивации
            showCompressionDialog(&mainWindow, folderPath);
        } else {
            QMessageBox::warning(&mainWindow, QObject::tr("Error"), QObject::tr("Please select a folder to add to the archive."));
        }
    });


    QObject::connect(addAction, &QAction::triggered, [&]() {
        QModelIndex selectedIndex = tree->selectionModel()->currentIndex();
        if (selectedIndex.isValid() && model->isDir(selectedIndex)) {
            QString folderPath = model->filePath(selectedIndex);

            // Теперь вызываем диалог для выбора пути и формата архивации
            showCompressionDialog(&mainWindow, folderPath);
        } else {
            QMessageBox::warning(&mainWindow, QObject::tr("Error"), QObject::tr("Please select a folder to add to the archive."));
        }
    });


    QObject::connect(extractButton, &QToolButton::clicked, [&]() {
        QModelIndex selectedIndex = tree->selectionModel()->currentIndex();
        if (selectedIndex.isValid() && !model->isDir(selectedIndex)) {
            QString archivePath = model->filePath(selectedIndex);

            // Показать диалог для выбора папки назначения
            QString destinationPath = QFileDialog::getExistingDirectory(&mainWindow, "Select Destination Folder");
            if (!destinationPath.isEmpty()) {
                runExtractUtility(archivePath, destinationPath);
            } else {
                QMessageBox::warning(&mainWindow, QObject::tr("Error"), QObject::tr("Please select a valid destination folder."));
            }
        } else {
            QMessageBox::warning(&mainWindow, QObject::tr("Error"), QObject::tr("Please select a valid archive file to extract."));
        }
    });


    QObject::connect(extratToFolAction, &QAction::triggered, [&]() {
        QModelIndex selectedIndex = tree->selectionModel()->currentIndex();
        if (selectedIndex.isValid() && !model->isDir(selectedIndex)) {
            QString archivePath = model->filePath(selectedIndex);

            // Показать диалог для выбора папки назначения
            QString destinationPath = QFileDialog::getExistingDirectory(&mainWindow, "Select Destination Folder");
            if (!destinationPath.isEmpty()) {
                runExtractUtility(archivePath, destinationPath);
            } else {
                QMessageBox::warning(&mainWindow, QObject::tr("Error"), QObject::tr("Please select a valid destination folder."));
            }
        } else {
            QMessageBox::warning(&mainWindow, QObject::tr("Error"), QObject::tr("Please select a valid archive file to extract."));
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

    mainWindow.setWindowTitle(QObject::tr("Antim Archiver"));
    mainWindow.resize(1280,720);
    mainWindow.show();

    return a.exec();
}
