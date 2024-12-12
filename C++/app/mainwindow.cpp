#include "MainWindow.h"
#include "Utility.h"
#include "Manual.h"
#include "MenuBar.h"
#include "ToolBar.h"
#include "FileSystem.h"

#include <QApplication>
#include <QMainWindow>
#include <QMessageBox>
#include <QToolButton>
#include <QToolBar>
#include <QIcon>
#include <QLineEdit>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFileDialog>
#include <QClipboard>
#include <QDesktopServices>
#include <QMenuBar>
#include <QLabel>
#include <QPushButton>

MainWindow::MainWindow(QApplication *app, QWidget *parent) : QMainWindow(parent), application(app) {
    setupMenuBar();
    setupToolBar();
    setupFileSystem();
    setupStyles();
    connectActions();
}

void MainWindow::setupMenuBar() {
    menu = new MenuBar(this);
    menu->setupMenuBar(menuBar());
}

void MainWindow::setupToolBar() {
    QToolBar *toolBar = new QToolBar(this);
    addToolBar(toolBar);

    tools = new ToolBar(this);
    tools->setupToolBar(toolBar);
}

void MainWindow::setupFileSystem() {
    fileSystem = new FileSystemWidget();
    tree = fileSystem->getTree();
    model = fileSystem->getModel();
    pathLineEdit = fileSystem->getPathLine();
    backButton = fileSystem->getBackButton();

    QWidget *centralWidget = new QWidget();
    setCentralWidget(centralWidget);

    QVBoxLayout *mainLayout = new QVBoxLayout(centralWidget);
    QHBoxLayout *pathLayout = new QHBoxLayout();
    pathLayout->addWidget(backButton);
    pathLayout->addWidget(pathLineEdit);

    mainLayout->addLayout(pathLayout);  // Добавляем горизонтальную компоновку в основную вертикальную компоновку
    mainLayout->addWidget(tree);
}

void MainWindow::setupStyles() {
    // ------------------------------- Стили ------------------------------- //

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

    setStyleSheet(styleSheet);

    // ---------------------------------------------------------------------------- //
}

void MainWindow::connectActions() {
    // ------------------------------- Список Files ------------------------------- //

    connect(menu->getOpenAction(), &QAction::triggered, [&]() {
        // Открытие диалогового окна для выбора файла или папки
        QString selectedPath = QFileDialog::getOpenFileName(this, QObject::tr("Open Archive"), QDir::homePath(), QObject::tr("Archives (*.zip *.tar);;All Files (*.*)"));

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
                QMessageBox::warning(this, QObject::tr("Error"), QObject::tr("The selected path does not exist."));
            }
        }
    });

    QString copiedPath;

    // Действие "Копировать"
    connect(menu->getCopyAction(), &QAction::triggered, [&]() {
        QItemSelectionModel *selectionModel = tree->selectionModel(); // Получаем модель выделения
        QModelIndexList selectedIndexes = selectionModel->selectedRows(); // Получаем список всех выделенных строк

        if (selectedIndexes.isEmpty()) {
            QMessageBox::warning(this, QObject::tr("Error"), QObject::tr("No files or folders selected."));
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

        QMessageBox::information(this, QObject::tr("Copy"), QObject::tr("Paths copied to clipboard:\n%1").arg(copiedPaths.join("\n")));
    });

    // Действие "Вставить"
    connect(menu->getPasteAction(), &QAction::triggered, [&]() {
        QString clipboardText = QApplication::clipboard()->text();
        QStringList pathsToPaste = clipboardText.split("\n", Qt::SkipEmptyParts);

        if (pathsToPaste.isEmpty()) {
            QMessageBox::warning(this, QObject::tr("Error"), QObject::tr("No paths in clipboard."));
            return;
        }

        // Получаем текущий индекс выбранной директории в дереве
        QModelIndex currentIndex = tree->currentIndex();
        if (!currentIndex.isValid()) {
            QMessageBox::warning(this, QObject::tr("Error"), QObject::tr("No folder selected in the tree."));
            return;
        }

        QFileSystemModel *model = qobject_cast<QFileSystemModel *>(tree->model());
        if (!model) {
            QMessageBox::warning(this, QObject::tr("Error"), QObject::tr("Failed to access file model."));
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

        QMessageBox::information(this, QObject::tr("Paste"), QObject::tr("Files pasted successfully."));
    });

    // Действие "Выделить всё"
    QAction *highliteAllAction = menu->getHighliteAllAction();
    connect(menu->getHighliteAllAction(), &QAction::triggered, [&]() {
        tree->selectAll(); // Выделяет все элементы
    });

    // Действие "Снять выделение"
    connect(menu->getNoneHighliteAction(), &QAction::triggered, [&]() {
        tree->clearSelection(); // Снимает выделение со всех элементов
    });

    QObject::connect(menu->getExitAction(), &QAction::triggered, application, &QApplication::quit);

    // ---------------------------------------------------------------------------- //



    // ------------------------------- Список Commands ------------------------------- //

    connect(menu->getAddAction(), &QAction::triggered, [&]() {
        QModelIndex selectedIndex = tree->selectionModel()->currentIndex();
        if (selectedIndex.isValid() && model->isDir(selectedIndex)) {
            QString folderPath = model->filePath(selectedIndex);

            // Теперь вызываем диалог для выбора пути и формата архивации
            showCompressionDialog(this, folderPath);
        } else {
            QMessageBox::warning(this, QObject::tr("Error"), QObject::tr("Please select a folder to add to the archive."));
        }
    });

    connect(menu->getExtratToFolAction(), &QAction::triggered, [&]() {
        QModelIndex selectedIndex = tree->selectionModel()->currentIndex();
        if (selectedIndex.isValid() && !model->isDir(selectedIndex)) {
            QString archivePath = model->filePath(selectedIndex);

            QFileInfo fileInfo = model->fileInfo(selectedIndex);
            QString fileSuffix = fileInfo.suffix();

            // Показать диалог для выбора папки назначения
            QString destinationPath = QFileDialog::getExistingDirectory(this, "Select Destination Folder");

            if (!destinationPath.isEmpty()) {

                if (fileSuffix == "enc") {
                    // Вызов модального окна для ввода пароля
                    QDialog dialog(this);
                    dialog.setWindowTitle("Enter password");

                    QLabel *labelPassword = new QLabel("Password:");
                    QLineEdit *passwordInput = new QLineEdit;
                    passwordInput->setEchoMode(QLineEdit::Password);

                    QToolButton *hideButton = new QToolButton;
                    hideButton->setIcon(QIcon(":/icons/icons/showPassword.svg"));
                    hideButton->setCheckable(true);
                    hideButton->setToolButtonStyle(Qt::ToolButtonIconOnly);
                    hideButton->setStyleSheet(R"(
                        QToolButton {
                            padding: 1px 1px;
                        }
                    )");

                    QHBoxLayout *passwordLayout = new QHBoxLayout;
                    passwordLayout->addWidget(labelPassword);
                    passwordLayout->addWidget(passwordInput);
                    passwordLayout->addWidget(hideButton);

                    connect(hideButton, &QToolButton::toggled, [&](bool checked) {
                        if (checked) {
                            passwordInput->setEchoMode(QLineEdit::Normal); // Показать пароль
                            hideButton->setIcon(QIcon(":/icons/icons/hidePassword.svg")); // Изменить иконку
                        } else {
                            passwordInput->setEchoMode(QLineEdit::Password); // Скрыть пароль
                            hideButton->setIcon(QIcon(":/icons/icons/showPassword.svg")); // Вернуть иконку
                        }
                    });

                    QVBoxLayout *dialogLayout = new QVBoxLayout(&dialog);
                    dialogLayout->addLayout(passwordLayout);

                    QPushButton *okButton = new QPushButton("OK");
                    QPushButton *cancelButton = new QPushButton("Cancel");

                    QHBoxLayout *buttonLayout = new QHBoxLayout;
                    buttonLayout->addWidget(okButton);
                    buttonLayout->addWidget(cancelButton);

                    dialogLayout->addLayout(buttonLayout);

                    QObject::connect(okButton, &QPushButton::clicked, [&]() {
                        QString password = passwordInput->text();
                        dialog.accept();
                        runExtractUtility(archivePath, destinationPath, password);
                    });

                    QObject::connect(cancelButton, &QPushButton::clicked, &dialog, &QDialog::reject);

                    dialog.exec();

                } else {
                    runExtractUtility(archivePath, destinationPath);
                }

            } else {
                QMessageBox::warning(this, QObject::tr("Error"), QObject::tr("Please select a valid destination folder."));
            }

        } else {
            QMessageBox::warning(this, QObject::tr("Error"), QObject::tr("Please select a valid archive file to extract."));
        }
    });

    connect(menu->getShowAction(), &QAction::triggered, [&]() {
        QModelIndex index = tree->currentIndex();  // Получаем индекс текущего выбранного элемента

        if (!index.isValid()) return;  // Проверяем, что индекс действителен

        QString path = model->filePath(index);

        if (model->isDir(index)) {
            tree->setRootIndex(index);  // Меняем корень дерева на выбранный каталог
            pathLineEdit->setText(path); // Отображаем путь в текстовом поле
        } else {
            // Если это файл, пробуем его открыть
            if (!QDesktopServices::openUrl(QUrl::fromLocalFile(path))) {
                QMessageBox::warning(this, QObject::tr("Error"), QObject::tr("Could not open the file."));
            }
        }
    });

    connect(menu->getDeleteAction(), &QAction::triggered, [&]() {
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

    // Подключаем действие для переименования
    connect(menu->getRenameAction(), &QAction::triggered, [&]() {
        QModelIndex index = tree->currentIndex(); // Получаем текущий выбранный элемент

        if (!index.isValid()) {
            QMessageBox::warning(this, QObject::tr("Error"), QObject::tr("No item selected."));
            return;
        }

        if (!(model->flags(index) & Qt::ItemIsEditable)) {
            QMessageBox::warning(this, QObject::tr("Error"), QObject::tr("This item cannot be renamed."));
            return;
        }

        // Активируем режим редактирования для текущего элемента
        tree->edit(index);
    });

    // Обработка завершения редактирования
    connect(model, &QFileSystemModel::dataChanged, [&](const QModelIndex &topLeft, const QModelIndex &, const QVector<int> &roles) {
        if (roles.contains(Qt::EditRole)) {
            QFileInfo fileInfo = model->fileInfo(topLeft);

            if (!fileInfo.exists()) {
                QMessageBox::warning(this, QObject::tr("Error"), QObject::tr("Failed to rename the item."));
            } else {
                QMessageBox::information(this, QObject::tr("Success"), QObject::tr("Item renamed to: %1").arg(fileInfo.fileName()));
            }
        }
    });

    connect(menu->getInfoAction(), &QAction::triggered, [&]() {
        QModelIndex index = tree->currentIndex();  // Получаем текущий выбранный элемент в QTreeView

        if (!index.isValid()) {
            QMessageBox::warning(this, QObject::tr("Error"), QObject::tr("No item selected."));
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
        QMessageBox::information(this, QObject::tr("Properties"), info);
    });

    // ---------------------------------------------------------------------------- //



    // ------------------------------- Список Help ------------------------------- //

    connect(menu->getManualAction(), &QAction::triggered, [&]() {
        showManualWindow(this);
    });

    connect(menu->getAboutAction(), &QAction::triggered, [this]() {
        QMessageBox::about(this, QObject::tr("About the program AntimAR"),
                           QObject::tr("AntimAR\nVersion 1.0\n\nThis program is developed to work with file archives"));
    });

    // ---------------------------------------------------------------------------- //



    // ------------------------------- Список ToolBar ------------------------------- //

    connect(tools->getAddButton(), &QToolButton::clicked, [&]() {
        QModelIndex selectedIndex = tree->selectionModel()->currentIndex();
        if (selectedIndex.isValid() && model->isDir(selectedIndex)) {
            QString folderPath = model->filePath(selectedIndex);

            // Теперь вызываем диалог для выбора пути и формата архивации
            showCompressionDialog(this, folderPath);
        } else {
            QMessageBox::warning(this, QObject::tr("Error"), QObject::tr("Please select a folder to add to the archive."));
        }
    });

    connect(tools->getExtractButton(), &QToolButton::clicked, [&]() {
        QModelIndex selectedIndex = tree->selectionModel()->currentIndex();
        if (selectedIndex.isValid() && !model->isDir(selectedIndex)) {
            QString archivePath = model->filePath(selectedIndex);

            QFileInfo fileInfo = model->fileInfo(selectedIndex);
            QString fileSuffix = fileInfo.suffix();

            // Показать диалог для выбора папки назначения
            QString destinationPath = QFileDialog::getExistingDirectory(this, "Select Destination Folder");

            if (!destinationPath.isEmpty()) {

                if (fileSuffix == "enc") {
                    // Вызов модального окна для ввода пароля
                    QDialog dialog(this);
                    dialog.setWindowTitle("Enter password");

                    QLabel *labelPassword = new QLabel("Password:");
                    QLineEdit *passwordInput = new QLineEdit;
                    passwordInput->setEchoMode(QLineEdit::Password);

                    QToolButton *hideButton = new QToolButton;
                    hideButton->setIcon(QIcon(":/icons/icons/showPassword.svg"));
                    hideButton->setCheckable(true);
                    hideButton->setToolButtonStyle(Qt::ToolButtonIconOnly);
                    hideButton->setStyleSheet(R"(
                        QToolButton {
                            padding: 1px 1px;
                        }
                    )");

                    QHBoxLayout *passwordLayout = new QHBoxLayout;
                    passwordLayout->addWidget(labelPassword);
                    passwordLayout->addWidget(passwordInput);
                    passwordLayout->addWidget(hideButton);

                    connect(hideButton, &QToolButton::toggled, [&](bool checked) {
                        if (checked) {
                            passwordInput->setEchoMode(QLineEdit::Normal); // Показать пароль
                            hideButton->setIcon(QIcon(":/icons/icons/hidePassword.svg")); // Изменить иконку
                        } else {
                            passwordInput->setEchoMode(QLineEdit::Password); // Скрыть пароль
                            hideButton->setIcon(QIcon(":/icons/icons/showPassword.svg")); // Вернуть иконку
                        }
                    });

                    QVBoxLayout *dialogLayout = new QVBoxLayout(&dialog);
                    dialogLayout->addLayout(passwordLayout);

                    QPushButton *okButton = new QPushButton("OK");
                    QPushButton *cancelButton = new QPushButton("Cancel");

                    QHBoxLayout *buttonLayout = new QHBoxLayout;
                    buttonLayout->addWidget(okButton);
                    buttonLayout->addWidget(cancelButton);

                    dialogLayout->addLayout(buttonLayout);

                    QObject::connect(okButton, &QPushButton::clicked, [&]() {
                        QString password = passwordInput->text();
                        dialog.accept();
                        runExtractUtility(archivePath, destinationPath, password);
                    });

                    QObject::connect(cancelButton, &QPushButton::clicked, &dialog, &QDialog::reject);

                    dialog.exec();

                } else {
                    runExtractUtility(archivePath, destinationPath);
                }

            } else {
                QMessageBox::warning(this, QObject::tr("Error"), QObject::tr("Please select a valid destination folder."));
            }

        } else {
            QMessageBox::warning(this, QObject::tr("Error"), QObject::tr("Please select a valid archive file to extract."));
        }
    });

    connect(tools->getViewButton(), &QToolButton::clicked, [&]() {
        QModelIndex index = tree->currentIndex();  // Получаем индекс текущего выбранного элемента

        if (!index.isValid()) return;  // Проверяем, что индекс действителен

        QString path = model->filePath(index);

        if (model->isDir(index)) {
            tree->setRootIndex(index);  // Меняем корень дерева на выбранный каталог
            pathLineEdit->setText(path); // Отображаем путь в текстовом поле
        } else {
            // Если это файл, пробуем его открыть
            if (!QDesktopServices::openUrl(QUrl::fromLocalFile(path))) {
                QMessageBox::warning(this, QObject::tr("Error"), QObject::tr("Could not open the file."));
            }
        }
    });

    connect(tools->getDeleteButton(), &QToolButton::clicked, [&]() {
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

    connect(tools->getInfoButton(), &QToolButton::clicked, [&]() {
        QModelIndex index = tree->currentIndex();  // Получаем текущий выбранный элемент в QTreeView

        if (!index.isValid()) {
            QMessageBox::warning(this, QObject::tr("Error"), QObject::tr("No item selected."));
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
        QMessageBox::information(this, QObject::tr("Properties"), info);
    });

    // ---------------------------------------------------------------------------- //
}
