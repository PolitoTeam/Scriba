#ifndef COLORS_H
#define COLORS_H

#include <QList>
#include <QColor>
#include <QMap>

class Colors {
private:
      QMap<QString,bool> colors_map;
public:
     Colors(){
         for (QString x: QColor::colorNames()){

             if (x == 'white' || x=='black')
                 continue;
             colors_map.insert(x,false);
         }
         colors_map.insert(QString('green'),true);
      }
     void clear(){
         for (QString x: colors_map.keys()){
             colors_map[x]=false;
         }
     }
    QColor getColor(){
        // gestire il fatto di nessun colore disponibile
        for (QString x: colors_map.keys()){
            if (colors_map.value(x)==false){
                colors_map[x]=true;
                QColor color(x);
                color.setAlphaF(1.0);
                return color;
            }

        }
    }
};

#endif // COLORS_H
