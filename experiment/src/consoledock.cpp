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
#include "taskmanager.h"

ConsoleDock::ConsoleDock(QWidget *parent)
    : QDockWidget(parent)
{
    setObjectName(QLatin1String("ConsoleDock"));

    mPlainTextEdit = new QPlainTextEdit(this);
    mPlainTextEdit->setReadOnly(true);

    QPalette p = mPlainTextEdit->palette();
    p.setColor(QPalette::Base, Qt::black);
    p.setColor(QPalette::Text, Qt::lightGray);
    mPlainTextEdit->setPalette(p);

    setWidget(mPlainTextEdit);
    setWindowTitle("终端");
}

ConsoleDock::~ConsoleDock()
{
}

//bool ConsoleDock::connectProcess(TaskProcess *process, QByteArray* cache)
//{
//    if(mProcess || !process){
//        return false;
//    }
//    this->mProcess = process;
//    this->mCache = cache;
//    connect(mProcess,&TaskProcess::started,this,[this](){
////        appendStdin(mProcess->arguments()[1]);
//    });
//    connect(mProcess,&QProcess::readyReadStandardOutput,this,[this](){
//        *mCache = mProcess->readAllStandardOutput();
//        this->appendStdout(QString::fromUtf8(*mCache));
//    });
//    connect(mProcess,&QProcess::readyReadStandardError,this,[this](){
//        *mCache = mProcess->readAllStandardError();
//        this->appendStdErr(QString::fromUtf8(*mCache));
//    });
//    return true;
//}

void ConsoleDock::clear()
{
    mPlainTextEdit->clear();
}


void ConsoleDock::appendStderr(QString info)
{
    QString html =
    "<pre style='color:red'>"
      "%1"
    "</pre>";
    html = html.arg(info.toHtmlEscaped());
    mPlainTextEdit->appendHtml(html);

}

void ConsoleDock::appendStdin(QString dir , QString info)
{
    QString html =
    "<pre>"
      "<span style='color:lightblue'>%1</span>"
      "<span style='color:white'>$ </span>"
      "<span style='color:lightgreen'>%2</span>"
    "</pre>";
    html = html.arg(dir.toHtmlEscaped(),info.toHtmlEscaped());
    mPlainTextEdit->appendHtml(html);
}

void ConsoleDock::appendStdout(QString info)
{
    QString html =
    "<pre style='color:lightgrey'>"
      "%1"
    "</pre>";
    html = html.arg(info.toHtmlEscaped());
    mPlainTextEdit->appendHtml(html);
}
