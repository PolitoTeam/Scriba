#ifndef CLICKABLELABEL_H
#define CLICKABLELABEL_H


#include <QLabel>
#include <QWidget>
#include <Qt>

class ClickableLabel : public QLabel {
    Q_OBJECT

public:
    explicit ClickableLabel(QWidget* parent = Q_NULLPTR, Qt::WindowFlags f = Qt::WindowFlags());
    ~ClickableLabel();
    void setCustomPixmap(const QPixmap& p);


signals:
    void clicked();

protected:
    void mousePressEvent(QMouseEvent* event);
    void paintEvent(QPaintEvent* event) override;
    void enterEvent(QEvent *ev) override;
    void leaveEvent(QEvent *ev) override;

private:
    QPixmap pixmap;


};

#endif // CLICKABLELABEL_H
