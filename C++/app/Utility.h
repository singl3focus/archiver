#ifndef UTILITY_H
#define UTILITY_H

#include <QString>
#include <QWidget>

void runArchiverUtility(const QString &src, const QString &dst, const QString &format);
void runExtractUtility(const QString &src, const QString &dst);
void runExtractUtility(const QString &src, const QString &dst, const QString &format);
void showCompressionDialog(QWidget *parent, const QString &sourcePath);
void showExtractingDialog(QWidget *parent, const QString &destinationPath, const QString &archivePath, const QString &fileSuffix);

#endif // UTILITY_H
