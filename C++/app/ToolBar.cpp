#include "ToolBar.h"
#include <QToolBar>
#include <QToolButton>

ToolBar::ToolBar(QMainWindow *parent)
    : mainWindow(parent) {}

void ToolBar::setupToolBar(QToolBar *toolBar){
    addButton = new QToolButton(toolBar);
    addButton->setText(QObject::tr("Add"));
    addButton->setIcon(QIcon (":/icons/icons/add.svg"));
    addButton->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
    toolBar->addWidget(addButton);

    extractButton = new QToolButton(toolBar);
    extractButton->setText(QObject::tr("Extract to"));
    extractButton->setIcon(QIcon (":/icons/icons/extract.svg"));
    extractButton->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
    toolBar->addWidget(extractButton);

    viewButton = new QToolButton(toolBar);
    viewButton->setText(QObject::tr("View"));
    viewButton->setIcon(QIcon (":/icons/icons/view.svg"));
    viewButton->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
    toolBar->addWidget(viewButton);

    deleteButton = new QToolButton(toolBar);
    deleteButton->setText(QObject::tr("Delete"));
    deleteButton->setIcon(QIcon (":/icons/icons/delete.svg"));
    deleteButton->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
    toolBar->addWidget(deleteButton);

    infoButton = new QToolButton(toolBar);
    infoButton->setText(QObject::tr("Information"));
    infoButton->setIcon(QIcon (":/icons/icons/information.svg"));
    infoButton->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
    toolBar->addWidget(infoButton);
}
