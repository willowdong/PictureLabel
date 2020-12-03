#ifndef REGION_H
#define REGION_H

#include <QString>
#include <QPoint>


//extern bool mode;
//extern QString cur_file_name;

struct Region
{
    QString img_name;
    QString label;
    QPoint start;
    QPoint end;
    int shape=0;          //区域为矩形或椭圆,0->rect, 1->circle

    Region(QString & name,QString & label1,QPoint & startp,QPoint &endp,int _shape)
    {
        img_name = name;
        label=label1;
        start = startp;
        end = endp;
        shape=_shape;
    }

    Region(){};
};


#endif // REGION_H
