#include "clickablelabel.h"
#include <QDebug>
#include <QEvent>
#include <QMouseEvent>
#include <QPainter>

ClickableLabel::ClickableLabel(QWidget *parent, Qt::WindowFlags f)
    : QLabel(parent) {}

ClickableLabel::~ClickableLabel() {}

void ClickableLabel::setCustomPixmap(const QPixmap &p) {
  this->pixmap = p;
  ClickableLabel::update();
}
void ClickableLabel::mousePressEvent(QMouseEvent *event) { emit clicked(); }

void ClickableLabel::paintEvent(QPaintEvent *event) {
  int w = width();
  int h = height();

  QPixmap scaled = pixmap.scaled(w, h, Qt::KeepAspectRatioByExpanding,
                                 Qt::SmoothTransformation);

  QRect rect(scaled.rect().center().x() - w / 2,
             scaled.rect().center().y() - h / 2, w, h);
  QPixmap cropped = scaled.copy(rect);

  QBrush brush(cropped);
  QPainter painter(this);
  painter.setRenderHint(QPainter::Antialiasing);
  painter.setBrush(brush);
  painter.drawRoundedRect(1, 1, w - 4, h - 4, 100, 100);
  QLabel::paintEvent(event);
}

void ClickableLabel::disableTooltip() { this->tooltip = false; }

void ClickableLabel::enterEvent(QEvent *ev) {
  if (tooltip)
    setToolTip("Upload...");
}

void ClickableLabel::leaveEvent(QEvent *ev) {}
