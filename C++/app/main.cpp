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
#include <QDesktopServices>
#include <QUrl>
#include <QClipboard>
#include <QItemDelegate>
#include <QFileInfo>
#include <QSplitter>
#include <QListwidget>
#include <QTextEdit>
#include <QTextCursor>



class CustomFileSystemModel : public QFileSystemModel {
public:
    using QFileSystemModel::QFileSystemModel;

    Qt::ItemFlags flags(const QModelIndex &index) const override {
        Qt::ItemFlags defaultFlags = QFileSystemModel::flags(index);

        if (index.isValid()) {
            QFileInfo fileInfo = this->fileInfo(index);
            if (!fileInfo.isRoot()) {
                return defaultFlags | Qt::ItemIsEditable;
            }
        }

        return defaultFlags;
    }
};



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
    process->start("D:/archiver/C++/app/AAR.exe", arguments);

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
    process->start("D:/archiver/C++/app/AAR.exe", arguments);

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



bool copyDirectoryContents(const QString &sourceDirPath, const QString &destinationDirPath) {
    QDir sourceDir(sourceDirPath);
    if (!sourceDir.exists()) {
        return false;
    }

    QDir destinationDir(destinationDirPath);
    if (!destinationDir.exists()) {
        destinationDir.mkpath(".");
    }

    foreach (QString fileName, sourceDir.entryList(QDir::Files | QDir::NoDotAndDotDot)) {
        QString srcFilePath = sourceDirPath + "/" + fileName;
        QString destFilePath = destinationDirPath + "/" + fileName;
        if (!QFile::copy(srcFilePath, destFilePath)) {
            return false;
        }
    }

    foreach (QString dirName, sourceDir.entryList(QDir::Dirs | QDir::NoDotAndDotDot)) {
        QString srcDirPath = sourceDirPath + "/" + dirName;
        QString destDirPath = destinationDirPath + "/" + dirName;
        if (!copyDirectoryContents(srcDirPath, destDirPath)) {
            return false;
        }
    }

    return true;
}


void showManualWindow(QWidget *parent) {
    QDialog *helpDialog = new QDialog(parent);
    helpDialog->setWindowTitle(QObject::tr("User Guide"));

    // Основной layout для модального окна
    QVBoxLayout *mainLayout = new QVBoxLayout(helpDialog);

    // Splitter для разделения панели содержания и панели с деталями
    QSplitter *splitter = new QSplitter(Qt::Horizontal, helpDialog);

    // Панель с содержанием (список разделов)
    QListWidget *contentsWidget = new QListWidget();

    // Добавляем пункты содержания с иконками
    QListWidgetItem *aboutItem = new QListWidgetItem(QIcon(":/icons/icons/bookmark.svg"), QObject::tr("About program"));
    contentsWidget->addItem(aboutItem);

    QListWidgetItem *introductionItem = new QListWidgetItem(QIcon(":/icons/icons/bookmark.svg"), QObject::tr("Introduction"));
    contentsWidget->addItem(introductionItem);

    QListWidgetItem *interfaceItem = new QListWidgetItem(QIcon(":/icons/icons/bookmark.svg"), QObject::tr("AntimAR interface"));
    contentsWidget->addItem(interfaceItem);

    QListWidgetItem *futureItem = new QListWidgetItem(QIcon(":/icons/icons/bookmark.svg"), QObject::tr("Future functionality"));
    contentsWidget->addItem(futureItem);

    QListWidgetItem *contactsItem = new QListWidgetItem(QIcon(":/icons/icons/bookmark.svg"), QObject::tr("Developers contacts"));
    contentsWidget->addItem(contactsItem);


    // Панель с подробным описанием выбранного раздела
    QTextEdit *detailsWidget = new QTextEdit();
    detailsWidget->setReadOnly(true); // Только для чтения

    // Подключаем сигнал выбора элемента списка к выводу подробностей
    QObject::connect(contentsWidget, &QListWidget::currentTextChanged, [&](const QString &selectedText) {
        QString h1;
        QString textContent;
        QTextCharFormat h1Format;
        QTextCharFormat contentFormat;

        if (selectedText == QObject::tr("About program")) {
            h1 = QObject::tr("General information about AntimAR\n");
            textContent = QObject::tr("AntimAR is an archiver for Windows — a tool for archiving and managing archives. \n\n"
                                      "This program is available in two versions: \n"
                                      "•    AntimarAR.exe — a version with a graphical interface (GUI); \n"
                                      "•    AAR.exe — a console version launched from the command line and working in text mode.\n\n"
                                      "Some distinctive features of AntimAR:\n"
                                      "•    graphical interface with drag-and-drop support;\n"
                                      "•    ability to use command line interface;\n"
                                      "•    work with archives of the following formats - ZIP, TAR;\n"
                                      "•    the ability to set a password for the archive.\n");

            h1Format.setFontWeight(QFont::Bold);
            h1Format.setFontPointSize(16); // Размер шрифта для заголовка
            h1Format.setForeground(QColor("#BB86FC")); // Цвет заголовка

            contentFormat.setFontPointSize(12); // Размер шрифта для контента
            contentFormat.setForeground(Qt::white); // Цвет текста контента

        } else if (selectedText == QObject::tr("Introduction")) {
            h1 = QObject::tr("Graphical shell and command line modes\n");
            textContent = QObject::tr("AntimAR can be used in two ways: in the graphical shell mode "
                                      "(with the standard interface of Windows programs) and in the command line\n\n"
                                      "Archiving files in a graphical shell:\n"
                                      "•    Open AntimAR;\n"
                                      "•    In the file tree, find and select the desired file or folder;\n"
                                      "•    Click the button in the context menu 'Commands' -> 'Add to archive'\n"
                                      "     or press the key combination Alt + A\n "
                                      "     or click the 'Add' button on the toolbar;\n"
                                      "•    In the window that opens, select the path where you want to save the archive and the archiving format;\n"
                                      "•    Click the 'OK' button. \n\n"
                                      "Extract files in a graphical shell:\n"
                                      "•    Open AntimAR; \n"
                                      "•    In the file tree, find and select the desired archive; \n"
                                      "•    Click the button in the context menu 'Commands' -> 'Extract to folder' \n"
                                      "     or press the key combination Alt + E \n "
                                      "     or click the 'Extract to' button on the toolbar; \n"
                                      "•    In the dialog box that opens, select the folder where you want to extract the archive; \n"
                                      "•    Click the 'Select folder' button. \n\n"
                                      "<------------------------------------------------------------------------------>\n\n"
                                      "Archiving files in command line mode:\n"
                                      "•    Run the executable file AAR.exe from the command line in the following format:\n"
                                      "     AAR.exe <Mode of operation> -s <Source path> -d <Destination path> -f <Data format>\n"
                                      "•    Mode of operation takes the values ​​compress\n"
                                      "•    Source path - path to folder or file\n"
                                      "•    Destination path - the path where the archive will be saved\n"
                                      "•    Date format - archiving format (tar or zip)\n\n"
                                      "Extract files in command line mode:\n"
                                      "•    Run the executable file AAR.exe from the command line in the following format:\n"
                                      "     AAR.exe <Mode of operation> -s <Source path> -d <Destination path>\n"
                                      "•    Mode of operation takes the values de​​compress\n"
                                      "•    Source path - path to archive\n"
                                      "•    Destination path - the path where the files will be extracted\n");

            h1Format.setFontWeight(QFont::Bold);
            h1Format.setFontPointSize(16); // Размер шрифта для заголовка
            h1Format.setForeground(QColor("#BB86FC")); // Цвет заголовка

            contentFormat.setFontPointSize(12); // Размер шрифта для контента
            contentFormat.setForeground(Qt::white); // Цвет текста контента
        } else if (selectedText == QObject::tr("AntimAR interface")) {
            h1 = QObject::tr("Interface\n");
            textContent = QObject::tr("The interface consists of several parts: context menu, toolbar and file tree.\n\n"
                                      "<------------------------------------------------------------------------------>\n\n"
                                      "Context menu\n\n"
                                      "File:\n"
                                      "•    Open archive - opening an archive in a file tree\n"
                                      "•    Copy - copying elements\n"
                                      "•    Paste - inserting elements\n"
                                      "•    Highlite all - select all items open in the file tree\n"
                                      "•    None highlite - removal of selection\n"
                                      "•    Exit - exit the application\n\n"
                                      "Commands:\n"
                                      "•    Add to an archive - adding files to archive\n"
                                      "•    Extract to folder - extract files to folder\n"
                                      "•    Show file contents - show file contents\n"
                                      "•    Delete files/folders - delete file or folder\n"
                                      "•    Rename files/folders - rename file or folder\n"
                                      "•    Show information - shows the information contained in the file\n\n"
                                      "Help:\n"
                                      "•    Manual - brief manual for the application\n"
                                      "•    About the program - brief information about the application\n\n"
                                      "<------------------------------------------------------------------------------>\n\n"
                                      "Toolbar:\n"
                                      "•    Add - adding files to archive\n"
                                      "•    Extract to - extract files to folder\n"
                                      "•    View - show file contents\n"
                                      "•    Delete - delete file or folder\n"
                                      "•    Information - shows information about a file or folder\n");

            h1Format.setFontWeight(QFont::Bold);
            h1Format.setFontPointSize(16); // Размер шрифта для заголовка
            h1Format.setForeground(QColor("#BB86FC")); // Цвет заголовка

            contentFormat.setFontPointSize(12); // Размер шрифта для контента
            contentFormat.setForeground(Qt::white); // Цвет текста контента
        } else if (selectedText == QObject::tr("Future functionality")) {
            h1 = QObject::tr("What features will be added in the future\n");
            textContent = QObject::tr("In the near future, we plan to increase the number of supported compression formats, "
                                      "as well as add the ability to set a password for the archive.\n\n");

            h1Format.setFontWeight(QFont::Bold);
            h1Format.setFontPointSize(16); // Размер шрифта для заголовка
            h1Format.setForeground(QColor("#BB86FC")); // Цвет заголовка

            contentFormat.setFontPointSize(12); // Размер шрифта для контента
            contentFormat.setForeground(Qt::white); // Цвет текста контента
        } else if (selectedText == QObject::tr("Developers contacts")) {
            h1 = QObject::tr("Developers contacts\n");
            textContent = QObject::tr("Tursunov Imran (Backend)\n"
                                      "Email: tursunov.imran@mail.ru\n"
                                      "Telegram: @single_focus\n\n"
                                      "Smirnov Anton (GUI)\n"
                                      "Email: kujik.toxa@gmail.com\n"
                                      "Telegram: @ascom8\n");

            h1Format.setFontWeight(QFont::Bold);
            h1Format.setFontPointSize(16); // Размер шрифта для заголовка
            h1Format.setForeground(QColor("#BB86FC")); // Цвет заголовка

            contentFormat.setFontPointSize(12); // Размер шрифта для контента
            contentFormat.setForeground(Qt::white); // Цвет текста контента
        }

        // Устанавливаем заголовок с его форматированием
        detailsWidget->clear();
        QTextCursor cursor = detailsWidget->textCursor();
        cursor.insertText(h1, h1Format);

        // Переход к новой строке перед содержимым
        cursor.insertBlock();

        // Устанавливаем контент с его форматированием
        cursor.insertText(textContent, contentFormat);

        // Перемещаем курсор в начало текста, чтобы содержимое отображалось с самого верха
        cursor.movePosition(QTextCursor::Start);
        detailsWidget->setTextCursor(cursor);
    });

    // Добавляем виджеты в splitter
    splitter->addWidget(contentsWidget);
    splitter->addWidget(detailsWidget);

    // Настройка пропорций панели
    splitter->setSizes(QList<int>({200, 600})); // Разделение на 200 и 600 пикселей

    // Добавляем splitter в основной layout
    mainLayout->addWidget(splitter);

    helpDialog->resize(800, 600);
    helpDialog->exec(); // Показываем окно модально
}

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    app.setWindowIcon(QIcon(":/icons/icons/appIcon.svg"));

    QMainWindow mainWindow;

    QMenuBar *menuBar = mainWindow.menuBar();
    QMenu *fileMenu = menuBar->addMenu(QObject::tr("&File"));
    QMenu *commandsMenu = menuBar->addMenu(QObject::tr("&Commands"));
    QMenu *helpMenu = menuBar->addMenu(QObject::tr("&Help"));



    //Список пунктов File
    QAction *openAction = new QAction(QObject::tr("&Open archive"), &mainWindow);
    openAction->setShortcut(QKeySequence(Qt::CTRL | Qt::Key_O));
    fileMenu->addAction(openAction);

    QAction *copyAction = new QAction(QObject::tr("&Copy"), &mainWindow);
    copyAction->setShortcut(QKeySequence(Qt::CTRL | Qt::Key_C));
    fileMenu->addAction(copyAction);

    QAction *pasteAction = new QAction(QObject::tr("&Paste"), &mainWindow);
    pasteAction->setShortcut(QKeySequence(Qt::CTRL | Qt::Key_V));
    fileMenu->addAction(pasteAction);

    QAction *highliteAllAction = new QAction(QObject::tr("&Highlite all"), &mainWindow);
    highliteAllAction->setShortcut(QKeySequence(Qt::CTRL | Qt::Key_A));
    fileMenu->addAction(highliteAllAction);

    QAction *noneHighliteAction = new QAction(QObject::tr("&None highlite"), &mainWindow);
    noneHighliteAction->setShortcut(QKeySequence(Qt::CTRL | Qt::Key_Minus));
    fileMenu->addAction(noneHighliteAction);

    QAction *exitAction = new QAction(QObject::tr("&Exit"), &mainWindow);
    fileMenu->addAction(exitAction);

    QObject::connect(exitAction, &QAction::triggered, &app, &QApplication::quit);



    //Список пунктов Commands
    QAction *addAction = new QAction(QObject::tr("&Add to an archive"), &mainWindow);
    addAction->setShortcut(QKeySequence(Qt::ALT | Qt::Key_A));
    commandsMenu->addAction(addAction);

    QAction *extratToFolAction = new QAction(QObject::tr("&Extract to folder"), &mainWindow);
    extratToFolAction->setShortcut(QKeySequence(Qt::ALT | Qt::Key_E));
    commandsMenu->addAction(extratToFolAction);

    QAction *showAction = new QAction(QObject::tr("&Show file contents"), &mainWindow);
    showAction->setShortcut(QKeySequence(Qt::ALT | Qt::Key_V));
    commandsMenu->addAction(showAction);

    QAction *deleteAction = new QAction(QObject::tr("&Delete files/folders"), &mainWindow);
    deleteAction->setShortcut(QKeySequence(Qt::Key_Delete));
    commandsMenu->addAction(deleteAction);

    QAction *renameAction = new QAction(QObject::tr("Rename files/folders&"), &mainWindow);
    renameAction->setShortcut(QKeySequence(Qt::Key_F2));
    commandsMenu->addAction(renameAction);

    QAction *infoAction = new QAction(QObject::tr("&Show information"), &mainWindow);
    infoAction->setShortcut(QKeySequence(Qt::ALT | Qt::Key_I));
    commandsMenu->addAction(infoAction);



    //Список пунктов Help
    QAction *manualAction = new QAction(QObject::tr("&Manual"), &mainWindow);
    helpMenu->addAction(manualAction);

    QObject::connect(manualAction, &QAction::triggered, [&]() {
        showManualWindow(&mainWindow);
    });

    QAction *aboutAction = new QAction(QObject::tr("&About the program"), &mainWindow);
    helpMenu->addAction(aboutAction);

    QObject::connect(aboutAction, &QAction::triggered, [&mainWindow]() {
        QMessageBox::about(&mainWindow, QObject::tr("About the program AntimAR"), QObject::tr("AntimAR\nVersion 1.0\n\nThis program is developed to work with file archives"));
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

    CustomFileSystemModel *model = new CustomFileSystemModel();
    model->setRootPath(QDir::rootPath());


    QTreeView *tree = new QTreeView();
    tree->setModel(model);
    tree->setRootIndex(model->index("C:/"));
    tree->setExpandsOnDoubleClick(false);
    tree->setItemsExpandable(false);
    tree->setRootIsDecorated(false);
    tree->setSelectionMode(QAbstractItemView::ExtendedSelection);
    tree->setEditTriggers(QAbstractItemView::SelectedClicked | QAbstractItemView::EditKeyPressed);


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

    // Обработка двойного клика
    QObject::connect(tree, &QTreeView::doubleClicked, [&](const QModelIndex &index) {
        QString path = model->filePath(index);

        if (model->isDir(index)) {
            tree->setRootIndex(index);
            pathLineEdit->setText(path);
        } else {
            // Если это файл, открыть его
            if (!QDesktopServices::openUrl(QUrl::fromLocalFile(path))) {
                QMessageBox::warning(&mainWindow, QObject::tr("Error"), QObject::tr("Could not open the file."));
            }
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


    QObject::connect(openAction, &QAction::triggered, [&]() {
        // Открытие диалогового окна для выбора файла или папки
        QString selectedPath = QFileDialog::getOpenFileName(&mainWindow, QObject::tr("Open Archive"), QDir::homePath(), QObject::tr("Archives (*.zip *.tar);;All Files (*.*)"));

        if (!selectedPath.isEmpty()) {
            // Проверка, является ли выбранный путь существующим
            QFileInfo fileInfo(selectedPath);
            if (fileInfo.exists()) {
                if (fileInfo.isDir()) {
                    // Если это папка, установить её как корень дерева
                    tree->setRootIndex(model->index(selectedPath));
                    pathLineEdit->setText(selectedPath);
                } else {
                    // Если это файл, получить его родительскую директорию
                    QString parentDir = fileInfo.absolutePath();
                    tree->setRootIndex(model->index(parentDir));
                    pathLineEdit->setText(parentDir);

                    // Автоматическое выделение файла в дереве
                    QModelIndex fileIndex = model->index(selectedPath);
                    if (fileIndex.isValid()) {
                        tree->selectionModel()->select(fileIndex, QItemSelectionModel::Select | QItemSelectionModel::Rows);
                        tree->scrollTo(fileIndex); // Прокрутка до выбранного элемента
                    }
                }
            } else {
                QMessageBox::warning(&mainWindow, QObject::tr("Error"), QObject::tr("The selected path does not exist."));
            }
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


    QString copiedPath;

    // Действие "Копировать"    
    QObject::connect(copyAction, &QAction::triggered, [&]() {
        QItemSelectionModel *selectionModel = tree->selectionModel(); // Получаем модель выделения
        QModelIndexList selectedIndexes = selectionModel->selectedRows(); // Получаем список всех выделенных строк

        if (selectedIndexes.isEmpty()) {
            QMessageBox::warning(&mainWindow, QObject::tr("Error"), QObject::tr("No files or folders selected."));
            return;
        }

        QFileSystemModel *model = qobject_cast<QFileSystemModel *>(tree->model());
        if (!model) return;

        QStringList copiedPaths;
        for (const QModelIndex &index : selectedIndexes) {
            if (index.isValid()) {
                copiedPaths << model->filePath(index); // Добавляем пути в список
            }
        }

        // Объединяем пути и записываем их в буфер обмена
        QApplication::clipboard()->setText(copiedPaths.join("\n"));

        QMessageBox::information(&mainWindow, QObject::tr("Copy"), QObject::tr("Paths copied to clipboard:\n%1").arg(copiedPaths.join("\n")));
    });


    // Действие "Вставить"
    QObject::connect(pasteAction, &QAction::triggered, [&]() {
        QString clipboardText = QApplication::clipboard()->text();
        QStringList pathsToPaste = clipboardText.split("\n", Qt::SkipEmptyParts);

        if (pathsToPaste.isEmpty()) {
            QMessageBox::warning(&mainWindow, QObject::tr("Error"), QObject::tr("No paths in clipboard."));
            return;
        }

        // Получаем текущий индекс выбранной директории в дереве
        QModelIndex currentIndex = tree->currentIndex();
        if (!currentIndex.isValid()) {
            QMessageBox::warning(&mainWindow, QObject::tr("Error"), QObject::tr("No folder selected in the tree."));
            return;
        }

        QFileSystemModel *model = qobject_cast<QFileSystemModel *>(tree->model());
        if (!model) {
            QMessageBox::warning(&mainWindow, QObject::tr("Error"), QObject::tr("Failed to access file model."));
            return;
        }

        QString destinationPath = model->filePath(currentIndex);
        if (destinationPath.isEmpty()) return;

        for (const QString &path : pathsToPaste) {
            QFileInfo fileInfo(path);
            QString targetPath = QDir(destinationPath).filePath(fileInfo.fileName());

            if (fileInfo.isDir()) {
                QDir().mkpath(targetPath);  // Создаем папку назначения
                QDir sourceDir(path);
                QDir targetDir(targetPath);
                for (const QString &entry : sourceDir.entryList(QDir::Files | QDir::Dirs | QDir::NoDotAndDotDot)) {
                    QFile::copy(sourceDir.filePath(entry), targetDir.filePath(entry));
                }
            } else {
                QFile::copy(path, targetPath);
            }
        }

        QMessageBox::information(&mainWindow, QObject::tr("Paste"), QObject::tr("Files pasted successfully."));
    });



    // Действие "Выделить всё"
    QObject::connect(highliteAllAction, &QAction::triggered, [&]() {
        tree->selectAll(); // Выделяет все элементы
    });

    // Действие "Снять выделение"
    QObject::connect(noneHighliteAction, &QAction::triggered, [&]() {
        tree->clearSelection(); // Снимает выделение со всех элементов
    });


    QObject::connect(deleteAction, &QAction::triggered, [&]() {
        QModelIndexList selectedIndexes = tree->selectionModel()->selectedIndexes();

        QFileSystemModel *model = dynamic_cast<QFileSystemModel *>(tree->model());
        if (!model) return;

        for (const QModelIndex &index : selectedIndexes) {
            QString filePath = model->filePath(index);

            QFileInfo fileInfo(filePath);
            if (fileInfo.isDir()) {
                QDir dir(filePath);
                if (!dir.removeRecursively()) {
                    qWarning() << "Failed to delete folder:" << filePath;
                }
            } else if (fileInfo.isFile()) {
                QFile file(filePath);
                if (!file.remove()) {
                    qWarning() << "Failed to delete file:" << filePath;
                }
            }
        }

        // Обновляем модель, сбросив корневой путь
        QString rootPath = model->rootPath();
        model->setRootPath(QString());  // Сброс пути
        model->setRootPath(rootPath);   // Установка обратно
    });


    QObject::connect(deleteButton, &QToolButton::clicked, [&]() {
        QModelIndexList selectedIndexes = tree->selectionModel()->selectedIndexes();

        QFileSystemModel *model = dynamic_cast<QFileSystemModel *>(tree->model());
        if (!model) return;

        for (const QModelIndex &index : selectedIndexes) {
            QString filePath = model->filePath(index);

            QFileInfo fileInfo(filePath);
            if (fileInfo.isDir()) {
                QDir dir(filePath);
                if (!dir.removeRecursively()) {
                    qWarning() << "Failed to delete folder:" << filePath;
                }
            } else if (fileInfo.isFile()) {
                QFile file(filePath);
                if (!file.remove()) {
                    qWarning() << "Failed to delete file:" << filePath;
                }
            }
        }

        // Обновляем модель, сбросив корневой путь
        QString rootPath = model->rootPath();
        model->setRootPath(QString());  // Сброс пути
        model->setRootPath(rootPath);   // Установка обратно
    });


    QObject::connect(showAction, &QAction::triggered, [&]() {
        QModelIndex index = tree->currentIndex();  // Получаем индекс текущего выбранного элемента

        if (!index.isValid()) return;  // Проверяем, что индекс действителен

        QString path = model->filePath(index);

        if (model->isDir(index)) {
            tree->setRootIndex(index);  // Меняем корень дерева на выбранный каталог
            pathLineEdit->setText(path); // Отображаем путь в текстовом поле
        } else {
            // Если это файл, пробуем его открыть
            if (!QDesktopServices::openUrl(QUrl::fromLocalFile(path))) {
                QMessageBox::warning(&mainWindow, QObject::tr("Error"), QObject::tr("Could not open the file."));
            }
        }
    });

    QObject::connect(viewButton, &QToolButton::clicked, [&]() {
        QModelIndex index = tree->currentIndex();  // Получаем индекс текущего выбранного элемента

        if (!index.isValid()) return;  // Проверяем, что индекс действителен

        QString path = model->filePath(index);

        if (model->isDir(index)) {
            tree->setRootIndex(index);  // Меняем корень дерева на выбранный каталог
            pathLineEdit->setText(path); // Отображаем путь в текстовом поле
        } else {
            // Если это файл, пробуем его открыть
            if (!QDesktopServices::openUrl(QUrl::fromLocalFile(path))) {
                QMessageBox::warning(&mainWindow, QObject::tr("Error"), QObject::tr("Could not open the file."));
            }
        }
    });


    // Подключаем действие для переименования
    QObject::connect(renameAction, &QAction::triggered, [&]() {
        QModelIndex index = tree->currentIndex(); // Получаем текущий выбранный элемент

        if (!index.isValid()) {
            QMessageBox::warning(&mainWindow, QObject::tr("Error"), QObject::tr("No item selected."));
            return;
        }

        if (!(model->flags(index) & Qt::ItemIsEditable)) {
            QMessageBox::warning(&mainWindow, QObject::tr("Error"), QObject::tr("This item cannot be renamed."));
            return;
        }

        // Активируем режим редактирования для текущего элемента
        tree->edit(index);
    });


    // Обработка завершения редактирования
    QObject::connect(model, &QFileSystemModel::dataChanged, [&](const QModelIndex &topLeft, const QModelIndex &, const QVector<int> &roles) {
        if (roles.contains(Qt::EditRole)) {
            QFileInfo fileInfo = model->fileInfo(topLeft);

            if (!fileInfo.exists()) {
                QMessageBox::warning(&mainWindow, QObject::tr("Error"), QObject::tr("Failed to rename the item."));
            } else {
                QMessageBox::information(&mainWindow, QObject::tr("Success"), QObject::tr("Item renamed to: %1").arg(fileInfo.fileName()));
            }
        }
    });


    QObject::connect(infoAction, &QAction::triggered, [&]() {
        QModelIndex index = tree->currentIndex();  // Получаем текущий выбранный элемент в QTreeView

        if (!index.isValid()) {
            QMessageBox::warning(&mainWindow, QObject::tr("Error"), QObject::tr("No item selected."));
            return;
        }

        QFileInfo fileInfo = model->fileInfo(index);  // Получаем информацию о файле или папке

        // Если это папка, считаем количество элементов внутри
        QString info;
        if (fileInfo.isDir()) {
            QDir dir(fileInfo.absoluteFilePath());
            QStringList entries = dir.entryList(QDir::NoDotAndDotDot | QDir::Files | QDir::Dirs);  // Все файлы и папки, исключая "." и ".."
            int itemCount = entries.count();

            info = QObject::tr("Name: %1\n"
                               "Path: %2\n"
                               "Type: Directory\n"
                               "Item count: %3\n"
                               "Last Modified: %4")
                       .arg(fileInfo.fileName())
                       .arg(fileInfo.absoluteFilePath())
                       .arg(itemCount)
                       .arg(fileInfo.lastModified().toString());
        } else {
            info = QObject::tr("Name: %1\n"
                               "Path: %2\n"
                               "Size: %3 bytes\n"
                               "Type: File\n"
                               "Last Modified: %4")
                       .arg(fileInfo.fileName())
                       .arg(fileInfo.absoluteFilePath())
                       .arg(fileInfo.size())
                       .arg(fileInfo.lastModified().toString());
        }

        // Отображаем информацию в сообщении
        QMessageBox::information(&mainWindow, QObject::tr("Properties"), info);
    });


    QObject::connect(infoButton, &QToolButton::clicked, [&]() {
        QModelIndex index = tree->currentIndex();  // Получаем текущий выбранный элемент в QTreeView

        if (!index.isValid()) {
            QMessageBox::warning(&mainWindow, QObject::tr("Error"), QObject::tr("No item selected."));
            return;
        }

        QFileInfo fileInfo = model->fileInfo(index);  // Получаем информацию о файле или папке

        // Если это папка, считаем количество элементов внутри
        QString info;
        if (fileInfo.isDir()) {
            QDir dir(fileInfo.absoluteFilePath());
            QStringList entries = dir.entryList(QDir::NoDotAndDotDot | QDir::Files | QDir::Dirs);  // Все файлы и папки, исключая "." и ".."
            int itemCount = entries.count();

            info = QObject::tr("Name: %1\n"
                      "Path: %2\n"
                      "Type: Directory\n"
                      "Item count: %3\n"
                      "Last Modified: %4")
                       .arg(fileInfo.fileName())
                       .arg(fileInfo.absoluteFilePath())
                       .arg(itemCount)
                       .arg(fileInfo.lastModified().toString());
        } else {
            info = QObject::tr("Name: %1\n"
                      "Path: %2\n"
                      "Size: %3 bytes\n"
                      "Type: File\n"
                      "Last Modified: %4")
                       .arg(fileInfo.fileName())
                       .arg(fileInfo.absoluteFilePath())
                       .arg(fileInfo.size())
                       .arg(fileInfo.lastModified().toString());
        }

        // Отображаем информацию в сообщении
        QMessageBox::information(&mainWindow, QObject::tr("Properties"), info);
    });



    QWidget *centralWidget = new QWidget();
    QVBoxLayout *mainLayout = new QVBoxLayout(centralWidget);

    QHBoxLayout *pathLayout = new QHBoxLayout();
    pathLayout->addWidget(backButton);
    pathLayout->addWidget(pathLineEdit);

    mainLayout->addLayout(pathLayout);  // Добавляем горизонтальную компоновку в основную вертикальную компоновку
    mainLayout->addWidget(tree);
    mainWindow.setCentralWidget(centralWidget);

    mainWindow.setWindowTitle(QObject::tr("AntimAR"));
    mainWindow.resize(1024,768);
    mainWindow.show();

    return app.exec();
}
