#include <QApplication>
#include <QTreeView>
#include <QFileSystemModel>
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

#include "Utility.h"
#include "Manual.h"
#include "MenuBar.h"
#include "ToolBar.h"
#include "FileSystem.h"


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


int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    app.setWindowIcon(QIcon(":/icons/icons/appIcon.svg"));

    QMainWindow mainWindow;

    MenuBar menu(&mainWindow);

    // Меню
    menu.setupMenuBar(mainWindow.menuBar());

    QAction *exitAction = menu.getExitAction();
    QObject::connect(exitAction, &QAction::triggered, &app, &QApplication::quit);


    QAction *manualAction = menu.getManualAction();
    QObject::connect(manualAction, &QAction::triggered, [&]() {
        showManualWindow(&mainWindow);
    });

    QAction *aboutAction = menu.getAboutAction();
    QObject::connect(aboutAction, &QAction::triggered, [&mainWindow]() {
        QMessageBox::about(&mainWindow, QObject::tr("About the program AntimAR"),
                           QObject::tr("AntimAR\nVersion 1.0\n\nThis program is developed to work with file archives"));
    });


    // Панель инструментов
    QToolBar *toolBar = new QToolBar(&mainWindow);
    mainWindow.addToolBar(toolBar);

    ToolBar tools(&mainWindow);
    tools.setupToolBar(toolBar);


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


    FileSystemWidget fileSystem(&mainWindow);

    QTreeView *tree = fileSystem.getTree();
    CustomFileSystemModel *model = fileSystem.getModel();
    QLineEdit *pathLineEdit = fileSystem.getPathLine();
    QToolButton *backButton = fileSystem.getBackButton();

    QAction *openAction = menu.getOpenAction();
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

    QToolButton *addButton = tools.getAddButton();
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


    QAction *addAction = menu.getAddAction();
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


    QToolButton *extractButton = tools.getExtractButton();
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


    QAction *extratToFolAction = menu.getExtratToFolAction();
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
    QAction *copyAction = menu.getCopyAction();
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
    QAction *pasteAction = menu.getPasteAction();
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
    QAction *highliteAllAction = menu.getHighliteAllAction();
    QObject::connect(highliteAllAction, &QAction::triggered, [&]() {
        tree->selectAll(); // Выделяет все элементы
    });

    // Действие "Снять выделение"
    QAction *noneHighliteAction = menu.getNoneHighliteAction();
    QObject::connect(noneHighliteAction, &QAction::triggered, [&]() {
        tree->clearSelection(); // Снимает выделение со всех элементов
    });


    QAction *deleteAction = menu.getDeleteAction();
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


    QToolButton *deleteButton = tools.getDeleteButton();
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


    QAction *showAction = menu.getShowAction();
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


    QToolButton *viewButton = tools.getViewButton();
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
    QAction *renameAction = menu.getRenameAction();
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


    QAction *infoAction = menu.getInfoAction();
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


    QToolButton *infoButton = tools.getInfoButton();
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


    // Компоновка
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
