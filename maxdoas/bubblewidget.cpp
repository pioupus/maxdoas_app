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

void TBubbleWidget::SetGainRes(int Gain, int Resolution, float ResolutionBorder, float MaxTilt){
    this->Resolution = Resolution;
    this->Gain = Gain;
    this->ResolutionBorder = ResolutionBorder;
    this->MaxTilt = MaxTilt;
}

void TBubbleWidget::SetTilt(float TiltX, float TiltY){
    TiltValue.setX(TiltX);
    TiltValue.setY(TiltY);
}

void TBubbleWidget::SetTiltOffset(QPointF Offset){
    float TiltX,TiltY;
    TiltX = 2*2.048*(float)Offset.x()/((float)Resolution*(float)Gain);
    TiltY = 2*2.048*(float)Offset.y()/((float)Resolution*(float)Gain);
    if(TiltX > 1)
        TiltX = 1.0;
    if(TiltX < -1)
        TiltX = -1.0;

    if(TiltY > 1)
        TiltY = 1.0;
    if(TiltY < -1)
        TiltY = -1.0;

    TiltX=asin((float)TiltX)*180/M_PI;
    TiltY=asin((float)TiltY)*180/M_PI;

    TiltOffset.setX(TiltX);
    TiltOffset.setY(TiltY);
}

void TBubbleWidget::resizeEvent(QResizeEvent *event){
    event->size();
}

QRectF TBubbleWidget::paintCircle(float sizeDeg, float OffsetX, float OffsetY){
    float centerLeft;
    float centerTop;
    float OuterDiameter;
    float CircleDiameter;
    float CircleZero;
    float CircleTop;
    float CircleLeft;
    (void)OffsetX;
    (void)OffsetY;
    if (width() > height()){
        OuterDiameter = height()/1.1;
    }else{
        OuterDiameter = width()/1.1;
    }
    CircleZero = OuterDiameter*BubbleSizeDeg/(MaxDegree);

    centerLeft = width()/2;
    centerTop = height()/2;
    CircleDiameter = CircleZero+(OuterDiameter-CircleZero)*sizeDeg/(MaxDegree);
    OffsetX = (OuterDiameter-CircleZero)*OffsetX/(MaxDegree);
    OffsetY = (OuterDiameter-CircleZero)*OffsetY/(MaxDegree);
    OffsetX /= 2;
    OffsetY /= 2;
    CircleLeft = centerLeft-CircleDiameter/2.0;
    CircleTop = centerTop-CircleDiameter/2.0;
    return QRectF(CircleLeft+OffsetX, CircleTop+OffsetY,  CircleDiameter, CircleDiameter);
}

void TBubbleWidget::paintEvent(QPaintEvent *)
{
    QRectF ResolutionBorderRect;
    QRectF ZeroBorder;
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing, true);
    //painter.translate(width() / 2, height() / 2);
    MaxDegree = 20.0;
    BubbleSizeDeg = 1.0;

    painter.setPen(QPen(QColor(Qt::darkBlue), 2));
    painter.setBrush(QBrush(QColor(Qt::darkBlue),Qt::SolidPattern));

    painter.drawEllipse(paintCircle(0,TiltValue.x(),TiltValue.y()));

    painter.setBrush(QBrush(QColor(Qt::darkBlue),Qt::NoBrush));

    painter.setPen(QPen(QColor(Qt::gray), 2));


    ResolutionBorderRect = paintCircle(MaxTilt,0,0);

    painter.drawEllipse(ResolutionBorderRect);

   // painter.drawText(round(width()/2)+2, ResolutionBorderRect.top()-2, "10");

    ZeroBorder = paintCircle(0,0,0);
    ResolutionBorderRect = paintCircle(ResolutionBorder,-TiltOffset.x(),-TiltOffset.y());

   // painter.drawText(round(width()/2)+2, ResolutionBorderRect.top()-2, "5");

    painter.drawRoundedRect (ResolutionBorderRect, ZeroBorder.width()/2, ZeroBorder.width()/2);

    painter.setPen(QPen(QColor(Qt::black), 1));

    painter.drawLine(0,round(height()/2),width(),round(height()/2));
    painter.drawLine(round(width()/2),0,round(width()/2),height());
}

