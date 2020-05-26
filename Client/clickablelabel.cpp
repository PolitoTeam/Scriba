#include "clickablelabel.h"
#include <QDebug>
#include <QPainter>
#include <QEvent>
#include <QMouseEvent>

ClickableLabel::ClickableLabel(QWidget* parent, Qt::WindowFlags f)
    : QLabel(parent) {
}

ClickableLabel::~ClickableLabel() {}

void ClickableLabel::setCustomPixmap(const QPixmap& p){
    this->pixmap=p;
    ClickableLabel::update();
}
void ClickableLabel::mousePressEvent(QMouseEvent* event) {
    emit clicked();

}

void ClickableLabel::paintEvent(QPaintEvent *event)
{
    QPixmap scaled = pixmap.scaled(width(),height(), Qt::KeepAspectRatioByExpanding, Qt::SmoothTransformation);
    qDebug()<<"width: "<<width()<<" hegiht: "<<height();
    QBrush brush(scaled);
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);
    painter.setBrush(brush);
    painter.drawRoundedRect(2, 2,width()-10, height()-10, 100, 100);
    QLabel::paintEvent(event);
}

void ClickableLabel::enterEvent(QEvent *ev)
    {
        qDebug()<<"here";
        setToolTip("Upload...");
    }

void ClickableLabel::leaveEvent(QEvent *ev)
    {
    qDebug()<<"e";
     // this->setStyleSheet("{ border-color: rgb(252, 1, 7); }");
    }




