/********************************************************************************
** Form generated from reading UI file 'login.ui'
**
** Created by: Qt User Interface Compiler version 5.12.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_LOGIN_H
#define UI_LOGIN_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QGroupBox>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QStackedWidget>
#include <QtWidgets/QTabWidget>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_Login
{
public:
    QGridLayout *gridLayout;
    QStackedWidget *stackedWidget;
    QWidget *pageLogin;
    QGridLayout *gridLayout_2;
    QTabWidget *tabWidget;
    QWidget *tabLogin;
    QVBoxLayout *verticalLayout_2;
    QGroupBox *groupBox;
    QVBoxLayout *verticalLayout;
    QHBoxLayout *horizontalLayout;
    QLabel *labelUsername;
    QLineEdit *lineEditUsername;
    QHBoxLayout *horizontalLayout_2;
    QLabel *labelPassword;
    QLineEdit *lineEditPassword;
    QPushButton *pushButtonLogin;
    QWidget *tabRegister;
    QVBoxLayout *verticalLayout_4;
    QGroupBox *groupBox_2;
    QVBoxLayout *verticalLayout_3;
    QHBoxLayout *horizontalLayout_3;
    QLabel *labelUsername_2;
    QLineEdit *lineEditUsername_2;
    QHBoxLayout *horizontalLayout_4;
    QLabel *labelPassword_2;
    QLineEdit *lineEditPassword_2;
    QHBoxLayout *horizontalLayout_5;
    QLabel *labelPassword_3;
    QLineEdit *lineEditPassword_3;
    QPushButton *pushButtonLogin_2;
    QSpacerItem *horizontalSpacer_2;
    QSpacerItem *verticalSpacer;
    QSpacerItem *horizontalSpacer;
    QSpacerItem *verticalSpacer_2;

    void setupUi(QWidget *Login)
    {
        if (Login->objectName().isEmpty())
            Login->setObjectName(QString::fromUtf8("Login"));
        Login->resize(397, 329);
        gridLayout = new QGridLayout(Login);
        gridLayout->setSpacing(6);
        gridLayout->setContentsMargins(11, 11, 11, 11);
        gridLayout->setObjectName(QString::fromUtf8("gridLayout"));
        stackedWidget = new QStackedWidget(Login);
        stackedWidget->setObjectName(QString::fromUtf8("stackedWidget"));
        pageLogin = new QWidget();
        pageLogin->setObjectName(QString::fromUtf8("pageLogin"));
        gridLayout_2 = new QGridLayout(pageLogin);
        gridLayout_2->setSpacing(6);
        gridLayout_2->setContentsMargins(11, 11, 11, 11);
        gridLayout_2->setObjectName(QString::fromUtf8("gridLayout_2"));
        tabWidget = new QTabWidget(pageLogin);
        tabWidget->setObjectName(QString::fromUtf8("tabWidget"));
        tabLogin = new QWidget();
        tabLogin->setObjectName(QString::fromUtf8("tabLogin"));
        verticalLayout_2 = new QVBoxLayout(tabLogin);
        verticalLayout_2->setSpacing(6);
        verticalLayout_2->setContentsMargins(11, 11, 11, 11);
        verticalLayout_2->setObjectName(QString::fromUtf8("verticalLayout_2"));
        groupBox = new QGroupBox(tabLogin);
        groupBox->setObjectName(QString::fromUtf8("groupBox"));
        groupBox->setAlignment(Qt::AlignCenter);
        verticalLayout = new QVBoxLayout(groupBox);
        verticalLayout->setSpacing(6);
        verticalLayout->setContentsMargins(11, 11, 11, 11);
        verticalLayout->setObjectName(QString::fromUtf8("verticalLayout"));
        horizontalLayout = new QHBoxLayout();
        horizontalLayout->setSpacing(6);
        horizontalLayout->setObjectName(QString::fromUtf8("horizontalLayout"));
        labelUsername = new QLabel(groupBox);
        labelUsername->setObjectName(QString::fromUtf8("labelUsername"));

        horizontalLayout->addWidget(labelUsername);

        lineEditUsername = new QLineEdit(groupBox);
        lineEditUsername->setObjectName(QString::fromUtf8("lineEditUsername"));
        lineEditUsername->setEchoMode(QLineEdit::Normal);

        horizontalLayout->addWidget(lineEditUsername);


        verticalLayout->addLayout(horizontalLayout);

        horizontalLayout_2 = new QHBoxLayout();
        horizontalLayout_2->setSpacing(6);
        horizontalLayout_2->setObjectName(QString::fromUtf8("horizontalLayout_2"));
        labelPassword = new QLabel(groupBox);
        labelPassword->setObjectName(QString::fromUtf8("labelPassword"));

        horizontalLayout_2->addWidget(labelPassword);

        lineEditPassword = new QLineEdit(groupBox);
        lineEditPassword->setObjectName(QString::fromUtf8("lineEditPassword"));
        lineEditPassword->setEchoMode(QLineEdit::Password);

        horizontalLayout_2->addWidget(lineEditPassword);


        verticalLayout->addLayout(horizontalLayout_2);

        pushButtonLogin = new QPushButton(groupBox);
        pushButtonLogin->setObjectName(QString::fromUtf8("pushButtonLogin"));
        pushButtonLogin->setAutoDefault(false);

        verticalLayout->addWidget(pushButtonLogin);


        verticalLayout_2->addWidget(groupBox);

        tabWidget->addTab(tabLogin, QString());
        tabRegister = new QWidget();
        tabRegister->setObjectName(QString::fromUtf8("tabRegister"));
        verticalLayout_4 = new QVBoxLayout(tabRegister);
        verticalLayout_4->setSpacing(6);
        verticalLayout_4->setContentsMargins(11, 11, 11, 11);
        verticalLayout_4->setObjectName(QString::fromUtf8("verticalLayout_4"));
        groupBox_2 = new QGroupBox(tabRegister);
        groupBox_2->setObjectName(QString::fromUtf8("groupBox_2"));
        groupBox_2->setAlignment(Qt::AlignCenter);
        verticalLayout_3 = new QVBoxLayout(groupBox_2);
        verticalLayout_3->setSpacing(6);
        verticalLayout_3->setContentsMargins(11, 11, 11, 11);
        verticalLayout_3->setObjectName(QString::fromUtf8("verticalLayout_3"));
        horizontalLayout_3 = new QHBoxLayout();
        horizontalLayout_3->setSpacing(6);
        horizontalLayout_3->setObjectName(QString::fromUtf8("horizontalLayout_3"));
        labelUsername_2 = new QLabel(groupBox_2);
        labelUsername_2->setObjectName(QString::fromUtf8("labelUsername_2"));

        horizontalLayout_3->addWidget(labelUsername_2);

        lineEditUsername_2 = new QLineEdit(groupBox_2);
        lineEditUsername_2->setObjectName(QString::fromUtf8("lineEditUsername_2"));

        horizontalLayout_3->addWidget(lineEditUsername_2);


        verticalLayout_3->addLayout(horizontalLayout_3);

        horizontalLayout_4 = new QHBoxLayout();
        horizontalLayout_4->setSpacing(6);
        horizontalLayout_4->setObjectName(QString::fromUtf8("horizontalLayout_4"));
        labelPassword_2 = new QLabel(groupBox_2);
        labelPassword_2->setObjectName(QString::fromUtf8("labelPassword_2"));

        horizontalLayout_4->addWidget(labelPassword_2);

        lineEditPassword_2 = new QLineEdit(groupBox_2);
        lineEditPassword_2->setObjectName(QString::fromUtf8("lineEditPassword_2"));
        lineEditPassword_2->setEchoMode(QLineEdit::Password);

        horizontalLayout_4->addWidget(lineEditPassword_2);


        verticalLayout_3->addLayout(horizontalLayout_4);

        horizontalLayout_5 = new QHBoxLayout();
        horizontalLayout_5->setSpacing(6);
        horizontalLayout_5->setObjectName(QString::fromUtf8("horizontalLayout_5"));
        labelPassword_3 = new QLabel(groupBox_2);
        labelPassword_3->setObjectName(QString::fromUtf8("labelPassword_3"));

        horizontalLayout_5->addWidget(labelPassword_3);

        lineEditPassword_3 = new QLineEdit(groupBox_2);
        lineEditPassword_3->setObjectName(QString::fromUtf8("lineEditPassword_3"));
        lineEditPassword_3->setEchoMode(QLineEdit::Password);

        horizontalLayout_5->addWidget(lineEditPassword_3);


        verticalLayout_3->addLayout(horizontalLayout_5);

        pushButtonLogin_2 = new QPushButton(groupBox_2);
        pushButtonLogin_2->setObjectName(QString::fromUtf8("pushButtonLogin_2"));
        pushButtonLogin_2->setAutoDefault(false);

        verticalLayout_3->addWidget(pushButtonLogin_2);


        verticalLayout_4->addWidget(groupBox_2);

        tabWidget->addTab(tabRegister, QString());

        gridLayout_2->addWidget(tabWidget, 1, 1, 2, 1);

        horizontalSpacer_2 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        gridLayout_2->addItem(horizontalSpacer_2, 1, 2, 1, 1);

        verticalSpacer = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding);

        gridLayout_2->addItem(verticalSpacer, 0, 1, 1, 1);

        horizontalSpacer = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        gridLayout_2->addItem(horizontalSpacer, 1, 0, 1, 1);

        verticalSpacer_2 = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding);

        gridLayout_2->addItem(verticalSpacer_2, 3, 1, 1, 1);

        stackedWidget->addWidget(pageLogin);

        gridLayout->addWidget(stackedWidget, 0, 0, 1, 1);


        retranslateUi(Login);

        tabWidget->setCurrentIndex(0);
        pushButtonLogin->setDefault(true);
        pushButtonLogin_2->setDefault(true);


        QMetaObject::connectSlotsByName(Login);
    } // setupUi

    void retranslateUi(QWidget *Login)
    {
        Login->setWindowTitle(QApplication::translate("Login", "Login", nullptr));
        groupBox->setTitle(QApplication::translate("Login", "Login", nullptr));
        labelUsername->setText(QApplication::translate("Login", "Username", nullptr));
        labelPassword->setText(QApplication::translate("Login", "Password ", nullptr));
        pushButtonLogin->setText(QApplication::translate("Login", "Sign in", nullptr));
        tabWidget->setTabText(tabWidget->indexOf(tabLogin), QApplication::translate("Login", "Login", nullptr));
        groupBox_2->setTitle(QApplication::translate("Login", "Register", nullptr));
        labelUsername_2->setText(QApplication::translate("Login", "Username", nullptr));
        labelPassword_2->setText(QApplication::translate("Login", "Password ", nullptr));
        labelPassword_3->setText(QApplication::translate("Login", "Confirm    ", nullptr));
        pushButtonLogin_2->setText(QApplication::translate("Login", "Sign up", nullptr));
        tabWidget->setTabText(tabWidget->indexOf(tabRegister), QApplication::translate("Login", "Register", nullptr));
    } // retranslateUi

};

namespace Ui {
    class Login: public Ui_Login {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_LOGIN_H
