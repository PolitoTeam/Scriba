/********************************************************************************
** Form generated from reading UI file 'signup.ui'
**
** Created by: Qt User Interface Compiler version 5.12.5
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_SIGNUP_H
#define UI_SIGNUP_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QGroupBox>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_Signup
{
public:
    QGridLayout *gridLayout_2;
    QGridLayout *gridLayout;
    QSpacerItem *horizontalSpacer_2;
    QSpacerItem *verticalSpacer_4;
    QGroupBox *groupBox;
    QVBoxLayout *verticalLayout;
    QHBoxLayout *horizontalLayout;
    QLabel *labelUsername;
    QLineEdit *lineEditUsername;
    QLabel *labelInfoUser;
    QHBoxLayout *horizontalLayout_2;
    QLabel *labelPassword;
    QLineEdit *lineEditPassword;
    QHBoxLayout *horizontalLayout_4;
    QLabel *labelConfirmPassword;
    QLineEdit *lineEditConfirmPassword;
    QLabel *labelInfoPass;
    QSpacerItem *horizontalSpacer;
    QSpacerItem *verticalSpacer_3;
    QLabel *label;
    QPushButton *pushButtonSignup;
    QPushButton *pushButtonClear;
    QPushButton *pushButtonBackLogin;

    void setupUi(QWidget *Signup)
    {
        if (Signup->objectName().isEmpty())
            Signup->setObjectName(QString::fromUtf8("Signup"));
        Signup->resize(604, 492);
        gridLayout_2 = new QGridLayout(Signup);
        gridLayout_2->setObjectName(QString::fromUtf8("gridLayout_2"));
        gridLayout = new QGridLayout();
        gridLayout->setObjectName(QString::fromUtf8("gridLayout"));
        horizontalSpacer_2 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        gridLayout->addItem(horizontalSpacer_2, 2, 5, 1, 1);

        verticalSpacer_4 = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding);

        gridLayout->addItem(verticalSpacer_4, 0, 2, 1, 3);

        groupBox = new QGroupBox(Signup);
        groupBox->setObjectName(QString::fromUtf8("groupBox"));
        QSizePolicy sizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(groupBox->sizePolicy().hasHeightForWidth());
        groupBox->setSizePolicy(sizePolicy);
        groupBox->setAlignment(Qt::AlignCenter);
        verticalLayout = new QVBoxLayout(groupBox);
        verticalLayout->setObjectName(QString::fromUtf8("verticalLayout"));
        horizontalLayout = new QHBoxLayout();
        horizontalLayout->setObjectName(QString::fromUtf8("horizontalLayout"));
        labelUsername = new QLabel(groupBox);
        labelUsername->setObjectName(QString::fromUtf8("labelUsername"));

        horizontalLayout->addWidget(labelUsername);

        lineEditUsername = new QLineEdit(groupBox);
        lineEditUsername->setObjectName(QString::fromUtf8("lineEditUsername"));
        lineEditUsername->setStyleSheet(QString::fromUtf8(""));
        lineEditUsername->setEchoMode(QLineEdit::Normal);

        horizontalLayout->addWidget(lineEditUsername);


        verticalLayout->addLayout(horizontalLayout);

        labelInfoUser = new QLabel(groupBox);
        labelInfoUser->setObjectName(QString::fromUtf8("labelInfoUser"));
        labelInfoUser->setStyleSheet(QString::fromUtf8("color:red;\n"
"font-style:italic;"));

        verticalLayout->addWidget(labelInfoUser);

        horizontalLayout_2 = new QHBoxLayout();
        horizontalLayout_2->setObjectName(QString::fromUtf8("horizontalLayout_2"));
        labelPassword = new QLabel(groupBox);
        labelPassword->setObjectName(QString::fromUtf8("labelPassword"));

        horizontalLayout_2->addWidget(labelPassword);

        lineEditPassword = new QLineEdit(groupBox);
        lineEditPassword->setObjectName(QString::fromUtf8("lineEditPassword"));
        lineEditPassword->setStyleSheet(QString::fromUtf8(""));
        lineEditPassword->setEchoMode(QLineEdit::Password);

        horizontalLayout_2->addWidget(lineEditPassword);


        verticalLayout->addLayout(horizontalLayout_2);

        horizontalLayout_4 = new QHBoxLayout();
        horizontalLayout_4->setObjectName(QString::fromUtf8("horizontalLayout_4"));
        labelConfirmPassword = new QLabel(groupBox);
        labelConfirmPassword->setObjectName(QString::fromUtf8("labelConfirmPassword"));

        horizontalLayout_4->addWidget(labelConfirmPassword);

        lineEditConfirmPassword = new QLineEdit(groupBox);
        lineEditConfirmPassword->setObjectName(QString::fromUtf8("lineEditConfirmPassword"));
        lineEditConfirmPassword->setEchoMode(QLineEdit::Password);

        horizontalLayout_4->addWidget(lineEditConfirmPassword);


        verticalLayout->addLayout(horizontalLayout_4);

        labelInfoPass = new QLabel(groupBox);
        labelInfoPass->setObjectName(QString::fromUtf8("labelInfoPass"));
        labelInfoPass->setStyleSheet(QString::fromUtf8("color:red;\n"
"font-style:italic;"));

        verticalLayout->addWidget(labelInfoPass);


        gridLayout->addWidget(groupBox, 2, 2, 1, 3);

        horizontalSpacer = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        gridLayout->addItem(horizontalSpacer, 2, 0, 1, 1);

        verticalSpacer_3 = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding);

        gridLayout->addItem(verticalSpacer_3, 5, 2, 1, 3);

        label = new QLabel(Signup);
        label->setObjectName(QString::fromUtf8("label"));
        label->setAlignment(Qt::AlignCenter);
        label->setMargin(0);

        gridLayout->addWidget(label, 1, 2, 1, 3);

        pushButtonSignup = new QPushButton(Signup);
        pushButtonSignup->setObjectName(QString::fromUtf8("pushButtonSignup"));
        pushButtonSignup->setAutoDefault(false);

        gridLayout->addWidget(pushButtonSignup, 3, 4, 2, 1);

        pushButtonClear = new QPushButton(Signup);
        pushButtonClear->setObjectName(QString::fromUtf8("pushButtonClear"));
        pushButtonClear->setAutoDefault(false);

        gridLayout->addWidget(pushButtonClear, 3, 3, 2, 1);

        pushButtonBackLogin = new QPushButton(Signup);
        pushButtonBackLogin->setObjectName(QString::fromUtf8("pushButtonBackLogin"));

        gridLayout->addWidget(pushButtonBackLogin, 3, 2, 2, 1);


        gridLayout_2->addLayout(gridLayout, 0, 0, 1, 1);


        retranslateUi(Signup);

        pushButtonSignup->setDefault(true);
        pushButtonClear->setDefault(true);


        QMetaObject::connectSlotsByName(Signup);
    } // setupUi

    void retranslateUi(QWidget *Signup)
    {
        Signup->setWindowTitle(QApplication::translate("Signup", "Form", nullptr));
        groupBox->setTitle(QApplication::translate("Signup", "SIgnup", nullptr));
        labelUsername->setText(QApplication::translate("Signup", "Username", nullptr));
        lineEditUsername->setText(QString());
        labelInfoUser->setText(QString());
        labelPassword->setText(QApplication::translate("Signup", "Password ", nullptr));
        labelConfirmPassword->setText(QApplication::translate("Signup", "Confirm password", nullptr));
        labelInfoPass->setText(QString());
        label->setText(QString());
        pushButtonSignup->setText(QApplication::translate("Signup", "DONE", nullptr));
        pushButtonClear->setText(QApplication::translate("Signup", "Clear", nullptr));
        pushButtonBackLogin->setText(QApplication::translate("Signup", "Login", nullptr));
    } // retranslateUi

};

namespace Ui {
    class Signup: public Ui_Signup {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_SIGNUP_H
