/****************************************************************************
**
** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the examples of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:BSD$
** You may use this file under the terms of the BSD license as follows:
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
**   * Neither the name of Nokia Corporation and its Subsidiary(-ies) nor
**     the names of its contributors may be used to endorse or promote
**     products derived from this software without specific prior written
**     permission.
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
** $QT_END_LICENSE$
**
****************************************************************************/

#include <QtGui>

#include "bubblewidget.h"

#include <stdlib.h>


TBubbleWidget::TBubbleWidget(QWidget *parent)
    : QWidget(parent)
{
    setBackgroundRole(QPalette::Base);
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
}

QSize TBubbleWidget::minimumSizeHint() const
{
    return QSize(50, 50);
}

QSize TBubbleWidget::sizeHint() const
{
    return QSize(180, 180);
}


void TBubbleWidget::resizeEvent(QResizeEvent *event){
    event->size();
}

void TBubbleWidget::paintEvent(QPaintEvent *)
{
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing, true);
    //painter.translate(width() / 2, height() / 2);
    int left=0;
    int top = 0;
    int left_bu=0;
    int top_bu = 0;
    int OutercircleRad = width();
    if (OutercircleRad > height()){
        OutercircleRad = height();
        left = width()/2-height()/2;
    }else{
        top = height()/2-width()/2;
    }


    painter.setPen(QPen(QColor(Qt::darkBlue), 2));


    painter.drawEllipse(QRectF(left+OutercircleRad / 100.0, top+OutercircleRad / 100.0,
                                       OutercircleRad-OutercircleRad/50, OutercircleRad-OutercircleRad/50));



    int bubbleradiant = OutercircleRad/1.5;
        left_bu = width()/2-bubbleradiant/2;
        top_bu = height()/2-bubbleradiant/2;
    painter.setPen(QPen(QColor(Qt::darkBlue), 2));
    painter.setBrush(QBrush(QColor(Qt::darkBlue),Qt::SolidPattern));
    painter.drawEllipse(QRectF(left_bu+bubbleradiant / 100.0, top_bu+bubbleradiant / 100.0,
                                       bubbleradiant-bubbleradiant/50, bubbleradiant-bubbleradiant/50));

    painter.setPen(QPen(QColor(Qt::black), 1));
    painter.drawLine(0,top+OutercircleRad/2,width(),OutercircleRad/2);
    painter.drawLine(left+OutercircleRad/2,0,left+OutercircleRad/2,height());
}

