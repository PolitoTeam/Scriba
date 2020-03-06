#ifndef COLORS_H
#define COLORS_H

#include <QList>
#include <QColor>

class Colors {
private:
      QList<QString> list_color = QColor::colorNames();
public:
    QColor getColor(int index){
        return QColor(list_color.at(index%list_color.size()));
    }
};

#endif // COLORS_H
