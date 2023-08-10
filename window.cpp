/****************************************************************************
**
** Copyright (C) 2016 The Qt Company Ltd.
** Contact: https://www.qt.io/licensing/
**
** This file is part of the examples of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:BSD$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see https://www.qt.io/terms-conditions. For further
** information use the contact form at https://www.qt.io/contact-us.
**
** BSD License Usage
** Alternatively, you may use this file under the terms of the BSD license
** as follows:
**
** "Redistribution and use in source and binary forms, with or without
** modification, are permitted provided that the following conditions are
** met:
**   * Redistributions of source code must retain the above copyright
**     notice, this list of conditions and the following disclaimer.
**   * Redistributions in binary form must reproduce the above copyright
**     notice, this list of conditions and the following disclaimer in
**     the documentation and/or other materials provided with the
**     distribution.
**   * Neither the name of The Qt Company Ltd nor the names of its
**     contributors may be used to endorse or promote products derived
**     from this software without specific prior written permission.
**
**
** THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
** "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
** LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
** A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
** OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
** SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
** LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
** DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
** THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
** (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
** OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE."
**
** $QT_END_LICENSE$
**
****************************************************************************/

#include "window.h"

#ifndef QT_NO_SYSTEMTRAYICON

#include <QAction>
#include <QCheckBox>
#include <QComboBox>
#include <QCoreApplication>
#include <QCloseEvent>
#include <QGroupBox>
#include <QLabel>
#include <QLineEdit>
#include <QMenu>
#include <QPushButton>
#include <QSpinBox>
#include <QTextEdit>
#include <QVBoxLayout>
#include <QMessageBox>
#include <QFileDialog>
#include <QProcess>
#include <QSettings>
#include <QDateTime>

//! [0]
Window::Window()
{
    createIconGroupBox();
    createMessageGroupBox();

    iconLabel->setMinimumWidth(durationLabel->sizeHint().width());

    createActions();
    createTrayIcon();

    connect(showMessageButton, &QAbstractButton::clicked, this, &Window::showMessage);
    connect(showIconCheckBox, &QAbstractButton::toggled, trayIcon, &QSystemTrayIcon::setVisible);
    connect(iconComboBox, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, &Window::setIcon);
    connect(trayIcon, &QSystemTrayIcon::messageClicked, this, &Window::messageClicked);
    connect(trayIcon, &QSystemTrayIcon::activated, this, &Window::iconActivated);

    QVBoxLayout *mainLayout = new QVBoxLayout;
    mainLayout->addWidget(iconGroupBox);
    mainLayout->addWidget(messageGroupBox);
    setLayout(mainLayout);

    iconComboBox->setCurrentIndex(1);
    trayIcon->show();

    setWindowTitle(tr("Systray"));
    resize(400, 300);


}
//! [0]

//! [1]
void Window::setVisible(bool visible)
{

    QDialog::setVisible(visible);
}
//! [1]

//! [2]
void Window::closeEvent(QCloseEvent *event)
{
#ifdef Q_OS_OSX
    if (!event->spontaneous() || !isVisible()) {
        return;
    }
#endif
    if (trayIcon->isVisible()) {
        QMessageBox::information(this, tr("Systray"),
                                 tr("The program will keep running in the "
                                    "system tray. To terminate the program, "
                                    "choose <b>Quit</b> in the context menu "
                                    "of the system tray entry."));
        hide();
        event->ignore();
    }
}
//! [2]

//! [3]
void Window::setIcon(int index)
{
    QIcon icon = iconComboBox->itemIcon(index);
    trayIcon->setIcon(icon);
    setWindowIcon(icon);
    QSettings setting(QCoreApplication::applicationDirPath()+"/doc_time_machine.ini", QSettings::IniFormat);
    //创建第一个分组(创建分组的第一种方法)
    setting.beginGroup("group1");
    //添加第一个分组中的键值对
    QString path =setting.value("path").toString();
    //设置当前分组结束
    setting.endGroup();
    trayIcon->setToolTip(path);
}
//! [3]

//! [4]
void Window::iconActivated(QSystemTrayIcon::ActivationReason reason)
{
    switch (reason) {
    case QSystemTrayIcon::Trigger:
        break;
    case QSystemTrayIcon::DoubleClick:
        handleSave();
        break;
    case QSystemTrayIcon::MiddleClick:
        showMessage();
        break;
    default:
        ;
    }
}
//! [4]

//! [5]
void Window::showMessage()
{
    showIconCheckBox->setChecked(true);
    int selectedIcon = typeComboBox->itemData(typeComboBox->currentIndex()).toInt();
    QSystemTrayIcon::MessageIcon msgIcon = QSystemTrayIcon::MessageIcon(selectedIcon);

    if (selectedIcon == -1) { // custom icon
        QIcon icon(iconComboBox->itemIcon(iconComboBox->currentIndex()));
        trayIcon->showMessage(titleEdit->text(), bodyEdit->toPlainText(), icon,
                          durationSpinBox->value() * 1000);
    } else {
        trayIcon->showMessage(titleEdit->text(), bodyEdit->toPlainText(), msgIcon,
                          durationSpinBox->value() * 1000);
    }
}
//! [5]

//! [6]
void Window::messageClicked()
{
    QMessageBox::information(nullptr, tr("Systray"),
                             tr("Sorry, I already gave what help I could.\n"
                                "Maybe you should try asking a human?"));
}
//! [6]

void Window::createIconGroupBox()
{
    iconGroupBox = new QGroupBox(tr("Tray Icon"));

    iconLabel = new QLabel("Icon:");

    iconComboBox = new QComboBox;
    iconComboBox->addItem(QIcon(":/images/file_time_machine.png"), tr("Bad"));
    iconComboBox->addItem(QIcon(":/images/file_time_machine.png"), tr("Heart"));
    iconComboBox->addItem(QIcon(":/images/file_time_machine.png"), tr("Trash"));

    showIconCheckBox = new QCheckBox(tr("Show icon"));
    showIconCheckBox->setChecked(true);

    QHBoxLayout *iconLayout = new QHBoxLayout;
    iconLayout->addWidget(iconLabel);
    iconLayout->addWidget(iconComboBox);
    iconLayout->addStretch();
    iconLayout->addWidget(showIconCheckBox);
    iconGroupBox->setLayout(iconLayout);
}

void Window::createMessageGroupBox()
{
    messageGroupBox = new QGroupBox(tr("Balloon Message"));

    typeLabel = new QLabel(tr("Type:"));

    typeComboBox = new QComboBox;
    typeComboBox->addItem(tr("None"), QSystemTrayIcon::NoIcon);
    typeComboBox->addItem(style()->standardIcon(
            QStyle::SP_MessageBoxInformation), tr("Information"),
            QSystemTrayIcon::Information);
    typeComboBox->addItem(style()->standardIcon(
            QStyle::SP_MessageBoxWarning), tr("Warning"),
            QSystemTrayIcon::Warning);
    typeComboBox->addItem(style()->standardIcon(
            QStyle::SP_MessageBoxCritical), tr("Critical"),
            QSystemTrayIcon::Critical);
    typeComboBox->addItem(QIcon(), tr("Custom icon"),
            -1);
    typeComboBox->setCurrentIndex(1);

    durationLabel = new QLabel(tr("Duration:"));

    durationSpinBox = new QSpinBox;
    durationSpinBox->setRange(5, 60);
    durationSpinBox->setSuffix(" s");
    durationSpinBox->setValue(15);

    durationWarningLabel = new QLabel(tr("(some systems might ignore this "
                                         "hint)"));
    durationWarningLabel->setIndent(10);

    titleLabel = new QLabel(tr("Title:"));

    titleEdit = new QLineEdit(tr("Cannot connect to network"));

    bodyLabel = new QLabel(tr("Body:"));

    bodyEdit = new QTextEdit;
    bodyEdit->setPlainText(tr("Don't believe me. Honestly, I don't have a "
                              "clue.\nClick this balloon for details."));

    showMessageButton = new QPushButton(tr("Show Message"));
    showMessageButton->setDefault(true);

    QGridLayout *messageLayout = new QGridLayout;
    messageLayout->addWidget(typeLabel, 0, 0);
    messageLayout->addWidget(typeComboBox, 0, 1, 1, 2);
    messageLayout->addWidget(durationLabel, 1, 0);
    messageLayout->addWidget(durationSpinBox, 1, 1);
    messageLayout->addWidget(durationWarningLabel, 1, 2, 1, 3);
    messageLayout->addWidget(titleLabel, 2, 0);
    messageLayout->addWidget(titleEdit, 2, 1, 1, 4);
    messageLayout->addWidget(bodyLabel, 3, 0);
    messageLayout->addWidget(bodyEdit, 3, 1, 2, 4);
    messageLayout->addWidget(showMessageButton, 5, 4);
    messageLayout->setColumnStretch(3, 1);
    messageLayout->setRowStretch(4, 1);
    messageGroupBox->setLayout(messageLayout);
}

void Window::createActions()
{
    minimizeAction = new QAction(tr("CreateRepo"), this);
    connect(minimizeAction, &QAction::triggered, this, &Window::handleCreateRepo);

    maximizeAction = new QAction(tr("Save"), this);
    connect(maximizeAction, &QAction::triggered, this, &Window::handleSave);

    quitAction = new QAction(tr("&Quit"), this);
    connect(quitAction, &QAction::triggered, qApp, &QCoreApplication::quit);
}

void Window::handleSave()
{

    QSettings setting(QCoreApplication::applicationDirPath()+"/doc_time_machine.ini", QSettings::IniFormat);
    //创建第一个分组(创建分组的第一种方法)
    setting.beginGroup("group1");
    //添加第一个分组中的键值对
    QString path =setting.value("path").toString();
    //设置当前分组结束
    setting.endGroup();
    if (path.isEmpty())
    {
        handleCreateRepo();
        return;
    }


    QStringList arguments;
    QString temp;

    QProcess *process = new QProcess();
    process->setWorkingDirectory(path);
    arguments <<"/c" <<"git add .";             //将命令写入列表中（命令前面加/c的作用是执行命令完成后关闭命令窗口）
    process->start("cmd",arguments);           //这里是process的start重载，意思为加载cmd窗口，并将命令写入
    arguments.clear();                         //命令写入完成后清除列表
    process->waitForStarted();                 //等待进程开始
    process->waitForFinished();                //等待进程结束

    temp = QString::fromLocal8Bit(process->readAllStandardOutput()) + QString::fromLocal8Bit(process->readAllStandardError());
    //trayIcon->showMessage("Test Message", temp, QSystemTrayIcon::Information, 3000);
    delete process;
    QDateTime current_date_time =QDateTime::currentDateTime();
    QString current_date =current_date_time.toString("yyyy_MM_dd_hh_mm_ss_zzz");
    process = new QProcess();
    process->setWorkingDirectory(path);
    arguments <<"/c" <<"git commit -m "<<current_date;             //将完戯后的处理封装到process中 " << path;             //将命令写入列表中（命令前面加/c的作用是执行命令完成后关闭命令窗口）
    process->start("cmd",arguments);           //这里是process的start重载，意思为加载cmd窗口，并将命令写入
    arguments.clear();                         //命令写入完成后清除列表
    process->waitForStarted();                 //等待进程开始
    process->waitForFinished();                //等待进程结束

    temp = QString::fromLocal8Bit(process->readAllStandardOutput())+ QString::fromLocal8Bit(process->readAllStandardError());
    trayIcon->showMessage("Save Message", temp, QSystemTrayIcon::Information, 1000);
    delete process;
}

void Window::handleCreateRepo()
{
    //qt open file folder
    QString path = QFileDialog::getExistingDirectory(0, "Select Directory", "./", QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);
    if (path.isEmpty())
    {
        return;
    }
    QStringList arguments;
    QString temp;
    QProcess *process = new QProcess();
    process->setWorkingDirectory(path);
    arguments <<"/c" <<"git init ";             //将命令写入列表中（命令前面加/c的作用是执行命令完成后关闭命令窗口）
    process->start("cmd",arguments);           //这里是process的start重载，意思为加载cmd窗口，并将命令写入
    arguments.clear();                         //命令写入完成后清除列表
    process->waitForStarted();                 //等待进程开始
    process->waitForFinished();                //等待进程结束

    temp = QString::fromLocal8Bit(process->readAllStandardOutput());
    trayIcon->showMessage("Create Message", temp, QSystemTrayIcon::Information, 1000);
    delete process;
    //创建QSettings对象并指定ini文件路径并将格式设置为ini
    QSettings setting(QCoreApplication::applicationDirPath()+"/doc_time_machine.ini", QSettings::IniFormat);
    //创建第一个分组(创建分组的第一种方法)
    setting.beginGroup("group1");
    //添加第一个分组中的键值对
    setting.setValue("path", path);
    //设置当前分组结束
    setting.endGroup();
    return;
}
void Window::createTrayIcon()
{
    QSettings setting(QCoreApplication::applicationDirPath()+"/doc_time_machine.ini", QSettings::IniFormat);
    //创建第一个分组(创建分组的第一种方法)
    setting.beginGroup("group1");
    //添加第一个分组中的键值对
    QString path =setting.value("path").toString();
    //设置当前分组结束
    setting.endGroup();

    trayIconMenu = new QMenu(this);
    if (!path.isEmpty())
    {
        trayIconMenu->addAction(new QAction(path, this));
        trayIconMenu->addSeparator();
        trayIconMenu->addSeparator();
    }
    trayIconMenu->addAction(minimizeAction);
    trayIconMenu->addSeparator();
    trayIconMenu->addAction(maximizeAction);
    trayIconMenu->addSeparator();
    trayIconMenu->addAction(quitAction);

    trayIcon = new QSystemTrayIcon(this);
    trayIcon->setContextMenu(trayIconMenu);
}

#endif
