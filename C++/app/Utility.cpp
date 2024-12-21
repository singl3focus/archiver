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
#include <QCheckBox>
#include <QToolButton>

#include "Utility.h"

// Функция для архивации без пароля
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
    process->start("./utility/AAR.exe", arguments);

    // Проверяем, что процесс успешно запущен
    if (!process->waitForStarted()) {
        QMessageBox::warning(nullptr, "Error", "Could not start the archiver utility.");
        delete process;
    }
}

// Перегрузка функции для архивации с паролем
void runArchiverUtility(const QString &src, const QString &dst, const QString &format, const QString &password) {
    QProcess *process = new QProcess;

    // Аргументы для утилиты
    QStringList arguments;
    arguments << "--source" << src
              << "--destination" << dst
              << "--format" << format
              << "--password" << password
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
    process->start("./utility/AAR.exe", arguments);

    // Проверяем, что процесс успешно запущен
    if (!process->waitForStarted()) {
        QMessageBox::warning(nullptr, "Error", "Could not start the archiver utility.");
        delete process;
    }
}

// Функция для распаковки без пароля
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
    process->start("./utility/AAR.exe", arguments);

    // Проверяем, что процесс успешно запущен
    if (!process->waitForStarted()) {
        QMessageBox::warning(nullptr, "Error", "Could not start the archiver utility.");
        delete process;
    }
}

// Функция для распаковки с паролем
void runExtractUtility(const QString &src, const QString &dst, const QString &password) {
    QProcess *process = new QProcess;

    // Аргументы для утилиты
    QStringList arguments;
    arguments << "--source" << src
              << "--destination" << dst
              << "--password" << password
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
    process->start("./utility/AAR.exe", arguments);

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

    QLabel *labelPassword = new QLabel("Password:");
    QCheckBox *passwordCheck = new QCheckBox;
    QLineEdit *passwordInput = new QLineEdit;
    passwordInput->setEchoMode(QLineEdit::Password); // Скрытие ввода для пароля
    passwordInput->setVisible(false);
    QToolButton *hideButton = new QToolButton;
    hideButton->setIcon(QIcon(":/icons/icons/showPassword.svg"));
    hideButton->setCheckable(true);
    hideButton->setVisible(false);
    hideButton->setToolButtonStyle(Qt::ToolButtonIconOnly);
    hideButton->setStyleSheet(R"(
        QToolButton {
            padding: 1px 1px;
        }
    )");

    QHBoxLayout *labelLayout = new QHBoxLayout;
    labelLayout->addWidget(labelPassword);
    labelLayout->addWidget(passwordCheck);
    labelLayout->addStretch();

    QHBoxLayout *passwordLayout = new QHBoxLayout;
    passwordLayout->addLayout(labelLayout); // Добавляем лейбл и чекбокс
    passwordLayout->addWidget(passwordInput);
    passwordLayout->addWidget(hideButton);

    QObject::connect(passwordCheck, &QCheckBox::checkStateChanged, [&](int state) {
        // Если чекбокс выбран, показываем строку для ввода и кнопку скрытия пароля, иначе скрываем
        bool isChecked = (state == Qt::Checked);
        passwordInput->setVisible(isChecked);
        hideButton->setVisible(isChecked);

    });

    QObject::connect(hideButton, &QToolButton::toggled, [&](bool checked) {
        if (checked) {
            passwordInput->setEchoMode(QLineEdit::Normal); // Показать пароль
            hideButton->setIcon(QIcon(":/icons/icons/hidePassword.svg")); // Изменить иконку
        } else {
            passwordInput->setEchoMode(QLineEdit::Password); // Скрыть пароль
            hideButton->setIcon(QIcon(":/icons/icons/showPassword.svg")); // Вернуть иконку
        }
    });

    QPushButton *okButton = new QPushButton("OK");
    QPushButton *cancelButton = new QPushButton("Cancel");

    QHBoxLayout *buttonLayout = new QHBoxLayout;
    buttonLayout->addStretch();
    buttonLayout->addWidget(okButton);
    buttonLayout->addWidget(cancelButton);

    QVBoxLayout *mainLayout = new QVBoxLayout(&dialog);
    mainLayout->addLayout(pathLayout);
    mainLayout->addLayout(formatLayout);
    mainLayout->addLayout(passwordLayout);
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
        QString password = passwordInput->text();
        if (!destinationPath.isEmpty()) {
            dialog.accept();
            if (passwordCheck->isChecked()){
                runArchiverUtility(sourcePath, destinationPath, format, password);
            } else {
                runArchiverUtility(sourcePath, destinationPath, format);
            }
        } else {
            QMessageBox::warning(&dialog, "Error", "Please specify a valid path to save the archive.");
        }
    });

    QObject::connect(cancelButton, &QPushButton::clicked, &dialog, &QDialog::reject);

    dialog.exec();
}

void showExtractionDialog(QWidget *parent, const QString &destinationPath, const QString &archivePath, const QString &fileSuffix) {
    QString suffix = fileSuffix;

    if (suffix == "enc") {
        // Вызов модального окна для ввода пароля
        QDialog dialog(parent);
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

        QObject::connect(hideButton, &QToolButton::toggled, [&](bool checked) {
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
}
