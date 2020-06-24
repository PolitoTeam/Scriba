#ifndef BUTTONHOVERWATCHER_H
#define BUTTONHOVERWATCHER_H

#include <QObject>
#include <QPushButton>
#include <QEvent>
#include <QIcon>

class ButtonHoverWatcher : public QObject
{
    Q_OBJECT
public:
    explicit ButtonHoverWatcher(QString std, QString hover, QObject * parent = Q_NULLPTR);
    virtual bool eventFilter(QObject * watched, QEvent * event) Q_DECL_OVERRIDE;

private:
    QString std;
    QString hover;
};


#endif // BUTTONHOVERWATCHER_H
