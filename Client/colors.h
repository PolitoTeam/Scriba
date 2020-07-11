#ifndef COLORS_H
#define COLORS_H

#include <QColor>
#include <QList>
#include <QMap>

// Twenty-two colors of maximum contrast by Kenneth L. Ken
// theory - http://www.iscc-archive.org/pdf/PC54_1724_001.pdf
// kelly's colors - https://i.kinja-img.com/gawker-media/image/upload/1015680494325093012.JPG
// hex values - http://hackerspace.kinja.com/iscc-nbs-number-hex-r-g-b-263-f2f3f4-242-243-244-267-22-1665795040

class Colors {
private:
  QSet<int> colors;
  QList<QColor> list_colors;
  int current=-1;

public:
  Colors() {
    colors = QSet<int>();
    list_colors =
        QList<QColor>({
                          QColor(230, 143,	172),
                          QColor(0,	103,	165),
                          QColor(249,	147,	121),
                          QColor(96,	78,	151),
                          QColor(246,	166,	0),
                          QColor(179,	68,	108),
                          QColor(220,	211,	0),
                          QColor(136,	45,	23),
                          QColor(141,	182,	0),
                          QColor(101,	69,	34),
                          QColor(226,	88,	34),
                          QColor(43,	61,	38),
                          QColor(243,	195,	0),
                          QColor(135,	86,	146),
                          QColor(243,	132,	0),
                          QColor(161,	202,	241),
                          QColor(190,	0,	50),
                          QColor(194,	178,	128),
                          QColor(132,	132,	130),

                       });
  }

  void clear() { colors.clear(); }

  int getIndex() {
    for (int n=0;n<list_colors.size();n++){
        current++;
        if (!colors.contains(current))
          return current;

    }
    // If no colors are free, assign a random busy color
    return rand() % list_colors.size();;
  }

  void freeColor(int x) { colors.remove(x); }

  QColor getColor(int index) {
    if (index >= 0 && index < list_colors.size()) {
      return list_colors[index];
    } else if (index == -1) { // local
      return  QColor(0,	136,	86);
    } else if (index == -2) { // remote but offline
      return QColor(192, 192, 192, 255);
    } else {
      return QColor("white");
    }
  }
};

#endif // COLORS_H
