#include "buttonhoverwatcher.h"

ButtonHoverWatcher::ButtonHoverWatcher(QString std, QString hover,
                                       QObject *parent)
    : QObject(parent) {
  this->std = std;
  this->hover = hover;
}

bool ButtonHoverWatcher::eventFilter(QObject *watched, QEvent *event) {
  QPushButton *button = qobject_cast<QPushButton *>(watched);
  if (!button) {
    return false;
  }

  if (event->type() == QEvent::Enter) {
    // The push button is hovered by mouse
    if (this->hover.isEmpty())
      button->setIcon(QIcon());
    else
      button->setIcon(QIcon(this->hover));
    return true;
  }

  if (event->type() == QEvent::Leave) {
    button->setIcon(QIcon());
    return true;
  }

  return false;
}
