#include <QProcess>
#include <QFileDialog>
#include <QDialog>
#include <QLabel>
#include <QPushButton>
#include <QMessageBox>
#include <QLineEdit>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QComboBox>

#include "Utility.h"

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
