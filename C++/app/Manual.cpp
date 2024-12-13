#include <QDialog>
#include <QVBoxLayout>
#include <QSplitter>
#include <QListwidget>
#include <QTextEdit>

#include "Manual.h"

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
