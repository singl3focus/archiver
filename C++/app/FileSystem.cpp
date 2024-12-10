#include "FileSystem.h"
#include <QLineEdit>
#include <QToolButton>
#include <QMessageBox>
#include <QDesktopServices>
#include <QVBoxLayout>
#include <QHBoxLayout>

FileSystemWidget::FileSystemWidget(QMainWindow *parent)
    : QWidget(parent),
    tree(new QTreeView(this)),
    pathLineEdit(new QLineEdit(this)),
    backButton(new QToolButton(this)),
    model(new CustomFileSystemModel(this))
{
    setupUi();
    connectSignals();
}

void FileSystemWidget::setupUi(){
    model->setRootPath(QDir::rootPath());
    tree->setModel(model);

    // Настройка виджетов
    tree->setRootIndex(model->index("C:/"));
    tree->setExpandsOnDoubleClick(false);
    tree->setItemsExpandable(false);
    tree->setRootIsDecorated(false);
    tree->setSelectionMode(QAbstractItemView::ExtendedSelection);
    tree->setEditTriggers(QAbstractItemView::SelectedClicked | QAbstractItemView::EditKeyPressed);

    backButton->setIcon(QIcon(":/icons/icons/back.svg"));
    backButton->setStyleSheet(R"(
        QToolButton {
            padding: 5px 5px;
        }
        QToolButton:hover {}
    )");

    QString initialPath = QDir::drives().isEmpty() ? QString() : QDir::drives().first().absolutePath();
    pathLineEdit->setReadOnly(false);
    pathLineEdit->setText(initialPath);

    QVBoxLayout *mainLayout = new QVBoxLayout();
    QHBoxLayout *pathLayout = new QHBoxLayout();
    pathLayout->addWidget(backButton);
    pathLayout->addWidget(pathLineEdit);

    mainLayout->addLayout(pathLayout);  // Добавляем горизонтальную компоновку в основную вертикальную компоновку
    mainLayout->addWidget(tree);
}

void FileSystemWidget::connectSignals(){

    // Логика для кнопки "back"
    connect(backButton, &QToolButton::clicked, [this]() {
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
            tree->setRootIndex(QModelIndex());  // Возврат к отображению списка дисков
            pathLineEdit->clear();
        } else {
            QModelIndex parentIndex = model->parent(currentRoot);
            if (parentIndex.isValid()) {
                tree->setRootIndex(parentIndex);
                pathLineEdit->setText(model->filePath(parentIndex));
            }
        }
    });

    // Обработка двойного клика
    connect(tree, &QTreeView::doubleClicked, [this](const QModelIndex &index) {
        QString path = model->filePath(index);

        if (model->isDir(index)) {
            tree->setRootIndex(index);
            pathLineEdit->setText(path);
        } else {
            // Если это файл, открыть его
            if (!QDesktopServices::openUrl(QUrl::fromLocalFile(path))) {
                QMessageBox::warning(this, QObject::tr("Error"), QObject::tr("Could not open the file."));
            }
        }
    });

    connect(pathLineEdit, &QLineEdit::returnPressed, [this]() {
        QString newPath = pathLineEdit->text();
        QDir dir(newPath);
        if (dir.exists()) {
            tree->setRootIndex(model->index(newPath));
        } else {
            QMessageBox::warning(this, QObject::tr("Error"), QObject::tr("The specified path does not exist!"));
        }
    });
}
