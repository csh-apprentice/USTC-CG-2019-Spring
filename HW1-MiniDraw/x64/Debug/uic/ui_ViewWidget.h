/********************************************************************************
** Form generated from reading UI file 'ViewWidget.ui'
**
** Created by: Qt User Interface Compiler version 5.12.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_VIEWWIDGET_H
#define UI_VIEWWIDGET_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_ViewWidgetClass
{
public:

    void setupUi(QWidget *ViewWidgetClass)
    {
        if (ViewWidgetClass->objectName().isEmpty())
            ViewWidgetClass->setObjectName(QString::fromUtf8("ViewWidgetClass"));
        ViewWidgetClass->resize(600, 400);

        retranslateUi(ViewWidgetClass);

        QMetaObject::connectSlotsByName(ViewWidgetClass);
    } // setupUi

    void retranslateUi(QWidget *ViewWidgetClass)
    {
        ViewWidgetClass->setWindowTitle(QApplication::translate("ViewWidgetClass", "ViewWidget", nullptr));
    } // retranslateUi

};

namespace Ui {
    class ViewWidgetClass: public Ui_ViewWidgetClass {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_VIEWWIDGET_H
