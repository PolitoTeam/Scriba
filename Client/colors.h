#ifndef COLORS_H
#define COLORS_H

#include <QColor>
#include <QList>
#include <QMap>

class Colors {
private:
  QSet<int> colors;
  QList<QColor> list_colors;

public:
  Colors() {
    colors = QSet<int>();
    list_colors =
        QList<QColor>({QColor(255, 0, 255, 255),   QColor(255, 51, 255, 255),
                       QColor(204, 0, 204, 255),   QColor(255, 102, 255, 255),
                       QColor(204, 51, 204, 255),  QColor(153, 0, 153, 255),
                       QColor(255, 153, 255, 255), QColor(204, 102, 204, 255),
                       QColor(153, 51, 153, 255),  QColor(102, 0, 102, 255),
                       QColor(255, 204, 255, 255), QColor(204, 153, 204, 255),
                       QColor(153, 102, 153, 255), QColor(17, 117, 232, 255),
                       QColor(102, 51, 102, 255),  QColor(51, 0, 51, 255),
                       QColor(204, 0, 255, 255),   QColor(204, 51, 255, 255),
                       QColor(153, 0, 204, 255),   QColor(204, 102, 255, 255),
                       QColor(153, 51, 204, 255),  QColor(102, 0, 153, 255),
                       QColor(204, 153, 255, 255), QColor(153, 102, 204, 255),
                       QColor(102, 51, 153, 255),  QColor(51, 0, 102, 255),
                       QColor(153, 0, 255, 255),   QColor(153, 51, 255, 255),
                       QColor(102, 0, 204, 255),   QColor(153, 102, 255, 255),
                       QColor(102, 51, 204, 255),  QColor(51, 0, 153, 255),
                       QColor(102, 0, 255, 255),   QColor(102, 51, 255, 255),
                       QColor(51, 0, 204, 255),    QColor(51, 0, 255, 255),
                       QColor(0, 0, 255, 255),     QColor(51, 51, 255, 255),
                       QColor(0, 0, 204, 255),     QColor(102, 102, 255, 255),
                       QColor(51, 51, 204, 255),   QColor(0, 0, 153, 255),
                       QColor(153, 153, 255, 255), QColor(102, 102, 204, 255),
                       QColor(51, 51, 153, 255),   QColor(0, 0, 102, 255),
                       QColor(204, 204, 255, 255), QColor(153, 153, 204, 255),
                       QColor(102, 102, 153, 255), QColor(51, 51, 102, 255),
                       QColor(0, 0, 51, 255),      QColor(0, 51, 255, 255),
                       QColor(51, 102, 255, 255),  QColor(0, 51, 204, 255),
                       QColor(0, 102, 255, 255),   QColor(102, 153, 255, 255),
                       QColor(51, 102, 204, 255),  QColor(0, 51, 153, 255),
                       QColor(51, 153, 255, 255),  QColor(0, 102, 204, 255),
                       QColor(0, 153, 255, 255),   QColor(153, 204, 255, 255),
                       QColor(102, 153, 204, 255), QColor(51, 102, 153, 255),
                       QColor(0, 51, 102, 255),    QColor(102, 204, 255, 255),
                       QColor(51, 153, 204, 255),  QColor(0, 102, 153, 255),
                       QColor(51, 204, 255, 255),  QColor(0, 153, 204, 255),
                       QColor(0, 204, 255, 255),   QColor(0, 255, 255, 255),
                       QColor(51, 255, 255, 255),  QColor(0, 204, 204, 255),
                       QColor(102, 255, 255, 255), QColor(51, 204, 204, 255),
                       QColor(0, 153, 153, 255),   QColor(153, 255, 255, 255),
                       QColor(102, 204, 204, 255), QColor(51, 153, 153, 255),
                       QColor(0, 102, 102, 255),   QColor(204, 255, 255, 255),
                       QColor(153, 204, 204, 255), QColor(102, 153, 153, 255),
                       QColor(51, 102, 102, 255),  QColor(0, 51, 51, 255),
                       QColor(0, 255, 204, 255),   QColor(51, 255, 204, 255),
                       QColor(0, 204, 153, 255),   QColor(102, 255, 204, 255),
                       QColor(51, 204, 153, 255),  QColor(0, 153, 102, 255),
                       QColor(153, 255, 204, 255), QColor(102, 204, 153, 255),
                       QColor(51, 153, 102, 255),  QColor(0, 102, 51, 255),
                       QColor(0, 255, 153, 255),   QColor(51, 255, 153, 255),
                       QColor(0, 204, 102, 255),   QColor(102, 255, 153, 255),
                       QColor(51, 204, 102, 255),  QColor(0, 153, 51, 255),
                       QColor(0, 255, 102, 255),   QColor(51, 255, 102, 255),
                       QColor(0, 204, 51, 255),    QColor(0, 255, 51, 255),
                       QColor(0, 255, 0, 255),     QColor(51, 255, 51, 255),
                       QColor(0, 204, 0, 255),     QColor(102, 255, 102, 255),
                       QColor(51, 204, 51, 255),   QColor(0, 153, 0, 255),
                       QColor(153, 255, 153, 255), QColor(102, 204, 102, 255),
                       QColor(51, 153, 51, 255),   QColor(0, 102, 0, 255),
                       QColor(204, 255, 204, 255), QColor(153, 204, 153, 255),
                       QColor(102, 153, 102, 255), QColor(51, 102, 51, 255),
                       QColor(0, 51, 0, 255),      QColor(51, 255, 0, 255),
                       QColor(102, 255, 51, 255),  QColor(51, 204, 0, 255),
                       QColor(102, 255, 0, 255),   QColor(153, 255, 102, 255),
                       QColor(102, 204, 51, 255),  QColor(51, 153, 0, 255),
                       QColor(153, 255, 51, 255),  QColor(102, 204, 0, 255),
                       QColor(153, 255, 0, 255),   QColor(204, 255, 153, 255),
                       QColor(153, 204, 102, 255), QColor(102, 153, 51, 255),
                       QColor(51, 102, 0, 255),    QColor(204, 255, 102, 255),
                       QColor(153, 204, 51, 255),  QColor(102, 153, 0, 255),
                       QColor(204, 255, 51, 255),  QColor(153, 204, 0, 255),
                       QColor(204, 255, 0, 255),   QColor(255, 255, 0, 255),
                       QColor(255, 255, 51, 255),  QColor(204, 204, 0, 255),
                       QColor(255, 255, 102, 255), QColor(204, 204, 51, 255),
                       QColor(153, 153, 0, 255),   QColor(255, 255, 153, 255),
                       QColor(204, 204, 102, 255), QColor(153, 153, 51, 255),
                       QColor(102, 102, 0, 255),   QColor(255, 255, 204, 255),
                       QColor(204, 204, 153, 255), QColor(153, 153, 102, 255),
                       QColor(102, 102, 51, 255),  QColor(51, 51, 0, 255),
                       QColor(255, 204, 0, 255),   QColor(255, 204, 51, 255),
                       QColor(204, 153, 0, 255),   QColor(255, 204, 102, 255),
                       QColor(204, 153, 51, 255),  QColor(153, 102, 0, 255),
                       QColor(255, 204, 153, 255), QColor(204, 153, 102, 255),
                       QColor(153, 102, 51, 255),  QColor(102, 51, 0, 255),
                       QColor(255, 153, 0, 255),   QColor(255, 153, 51, 255),
                       QColor(204, 102, 0, 255),   QColor(255, 153, 102, 255),
                       QColor(204, 102, 51, 255),  QColor(153, 51, 0, 255),
                       QColor(255, 102, 0, 255),   QColor(255, 102, 51, 255),
                       QColor(204, 51, 0, 255),    QColor(255, 51, 0, 255),
                       QColor(255, 0, 0, 255),     QColor(255, 51, 51, 255),
                       QColor(204, 0, 0, 255),     QColor(255, 102, 102, 255),
                       QColor(204, 51, 51, 255),   QColor(153, 0, 0, 255),
                       QColor(255, 153, 153, 255), QColor(204, 102, 102, 255),
                       QColor(153, 51, 51, 255),   QColor(102, 0, 0, 255),
                       QColor(255, 204, 204, 255), QColor(204, 153, 153, 255),
                       QColor(153, 102, 102, 255), QColor(102, 51, 51, 255),
                       QColor(51, 0, 0, 255),      QColor(255, 0, 51, 255),
                       QColor(255, 51, 102, 255),  QColor(204, 0, 51, 255),
                       QColor(255, 0, 102, 255),   QColor(255, 102, 153, 255),
                       QColor(204, 51, 102, 255),  QColor(153, 0, 51, 255),
                       QColor(255, 51, 153, 255),  QColor(204, 0, 102, 255),
                       QColor(255, 0, 153, 255),   QColor(255, 153, 204, 255),
                       QColor(204, 102, 153, 255), QColor(153, 51, 102, 255),
                       QColor(102, 0, 51, 255),    QColor(255, 102, 204, 255),
                       QColor(204, 51, 153, 255),  QColor(153, 0, 102, 255),
                       QColor(255, 51, 204, 255),  QColor(204, 0, 153, 255),
                       QColor(255, 0, 204, 255)});
  }

  void clear() { colors.clear(); }

  int getIndex() {
    int n = 0; // Number of iterations
    int i = 0;
    while (n < list_colors.size()) {
      n++;
      i = rand() % list_colors.size();

      if (!colors.contains(i))
        return i;
    }
    // If no colors are free, assign a random busy color
    return i;
  }

  void freeColor(int x) { colors.remove(x); }

  QColor getColor(int index) {
    if (index >= 0 && index < list_colors.size()) {
      return list_colors[index];
    } else if (index == -1) { // local
      return QColor(124, 252, 0, 255);
    } else if (index == -2) { // remote but offline
      return QColor(192, 192, 192, 255);
    } else {
      return QColor("white");
    }
  }
};

#endif // COLORS_H
