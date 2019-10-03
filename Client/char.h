#ifndef CHAR_H
#define CHAR_H
#include <QTextCharFormat>


class Char : public QTextCharFormat
{
    Q_OBJECT
public:
    Char(QWidget *parent = nullptr);
    ~Char();

private:

};

#endif // CHAR_H
