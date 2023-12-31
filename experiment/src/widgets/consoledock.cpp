/*
 * consoledock.cpp
 * Copyright 2013, Samuli Tuomola <samuli.tuomola@gmail.com>
 * Copyright 2018-2019, Thorbjørn Lindeijer <bjorn@lindeijer.nl>
 *
 * This file is part of Tiled.
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the Free
 * Software Foundation; either version 2 of the License, or (at your option)
 * any later version.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for
 * more details.
 *
 * You should have received a copy of the GNU General Public License along with
 * this program. If not, see <http://www.gnu.org/licenses/>.
 */

#include "consoledock.h"

#include <QCoreApplication>
#include <QLineEdit>
#include <QMenu>
#include <QPlainTextEdit>
#include <QPushButton>
#include <QShortcut>
#include <QVBoxLayout>


class ConsoleOutputWidget : public QPlainTextEdit
{
public:
    using QPlainTextEdit::QPlainTextEdit;

protected:
    void contextMenuEvent(QContextMenuEvent *event) override;
};

void ConsoleOutputWidget::contextMenuEvent(QContextMenuEvent *event)
{
//    std::unique_ptr<QMenu> menu { createStandardContextMenu(event->pos()) };

    auto clearIcon = QIcon::fromTheme(QStringLiteral("edit-clear"));
//    menu->addSeparator();
//    menu->addAction(clearIcon,
//                    QCoreApplication::translate("Tiled::ConsoleDock", "Clear Console"),
//                    this, &QPlainTextEdit::clear);

//    menu->exec(event->globalPos());
}


ConsoleDock::ConsoleDock(QWidget *parent)
    : QDockWidget(parent)
    , mPlainTextEdit(new ConsoleOutputWidget)
{
    setObjectName(QLatin1String("ConsoleDock"));

    QWidget *widget = new QWidget(this);
    QVBoxLayout *layout = new QVBoxLayout(widget);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(0);

    mPlainTextEdit->setReadOnly(true);

    QPalette p = mPlainTextEdit->palette();
    p.setColor(QPalette::Base, Qt::black);
    p.setColor(QPalette::Text, Qt::lightGray);
    mPlainTextEdit->setPalette(p);

//    mClearButton = new QPushButton("清空");
//    connect(mClearButton, &QPushButton::clicked, mPlainTextEdit, &QPlainTextEdit::clear);

    auto bottomBar = new QHBoxLayout;
//    bottomBar->addWidget(mClearButton);

    layout->addWidget(mPlainTextEdit);
    layout->addLayout(bottomBar);

    setWidget(widget);
    setWindowTitle("终端");
}

ConsoleDock::~ConsoleDock()
{
//    mBash->kill();
//    mBash->waitForFinished(-1);
}

bool ConsoleDock::connectProcess(QProcess *process, QByteArray* cache)
{
    if(mProcess || !process){
        return false;
    }
    this->mProcess = process;
    this->mCache = cache;
    connect(mProcess,&QProcess::started,this,[this](){
        appendScript(mProcess->arguments()[1]);
    });
    connect(mProcess,&QProcess::readyReadStandardOutput,this,[this](){
        *mCache = mProcess->readAllStandardOutput();
        this->appendScriptResult(QString::fromUtf8(*mCache));
    });
    connect(mProcess,&QProcess::readyReadStandardError,this,[this](){
        *mCache = mProcess->readAllStandardError();
        this->appendError(QString::fromUtf8(*mCache));
    });
    return true;
}

void ConsoleDock::appendInfo(const QString &str)
{
    mPlainTextEdit->appendHtml(QLatin1String("<pre>") + str.toHtmlEscaped() +
                               QLatin1String("</pre>"));
}

void ConsoleDock::appendWarning(const QString &str)
{
    mPlainTextEdit->appendHtml(QLatin1String("<pre style='color:orange'>") + str.toHtmlEscaped() +
                               QLatin1String("</pre>"));
}

void ConsoleDock::appendError(const QString &str)
{
    mPlainTextEdit->appendHtml(QLatin1String("<pre style='color:red'>") + str.toHtmlEscaped() +
                               QLatin1String("</pre>"));
}

void ConsoleDock::appendScript(const QString &str)
{
    mPlainTextEdit->appendHtml(QLatin1String("<pre style='color:lightblue'>") + QDir::currentPath().toHtmlEscaped() +
                               QLatin1String("</pre>") +
                               QLatin1String("<pre style='color:lightgreen'>&gt; ") + str.toHtmlEscaped() +
                               QLatin1String("</pre>"));
}

void ConsoleDock::appendScriptResult(const QString &str)
{
     mPlainTextEdit->appendHtml(QLatin1String("<pre style='color:lightgrey'>") + str.toHtmlEscaped() +
                               QLatin1String("</pre>"));
}
