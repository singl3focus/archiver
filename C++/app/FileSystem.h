#ifndef FILESYSTEM_H
#define FILESYSTEM_H

#include <QWidget>
#include <QMainWindow>
#include <QTreeView>
#include <QFileSystemModel>

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

class FileSystemWidget : public QWidget {
    Q_OBJECT

public:
    explicit FileSystemWidget(QMainWindow *parent = nullptr);

    QTreeView* getTree() const { return tree; }
    CustomFileSystemModel* getModel() const { return model; }
    QLineEdit* getPathLine() const { return pathLineEdit; }
    QToolButton* getBackButton() const { return backButton; }

private:
    QTreeView *tree;
    QLineEdit *pathLineEdit;
    QToolButton *backButton;
    CustomFileSystemModel *model;

    void setupUi();
    void connectSignals();
};
#endif // FILESYSTEM_H
