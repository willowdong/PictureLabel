#ifndef MYLABEL_H
#define MYLABEL_H

#include <QObject>
#include <QLabel>
#include <QPainter>
#include <QList>
#include "region.h"
#include <QFileInfoList>
#include <QStringList>
#include <QPainter>
#include <QDebug>
#include <QWheelEvent>
#include <QMenu>
#include <QFileDialog>
#include <QRect>
#include <QMessageBox>
#include <QInputDialog>
#include <QFont>

class MyLabel : public QLabel
{
    Q_OBJECT
public:
    explicit MyLabel(QWidget *parent = 0);
    ~MyLabel();
    void OnSelectImage();       //选择打开图片
    void OnSave();
    void OnPresetImage();       //图片还原
    void OnZoomInImage();       //图片放大
    void OnZoomOutImage();      //图片缩小
    void OnClearLab();          //清楚标记
    void OnNext();              //下一张
    void OnPre();               //上一张
    void OnOpenDir();           //打开文件夹
    void OnLoad();              //载入保存的结果
    void OnChange();            //切换矩形/椭圆

    QString infor="就绪";
    QList<Region> & returnList(){return _list;}

protected:
    void contextMenuEvent(QContextMenuEvent *event) override;   //右键菜单
    void paintEvent(QPaintEvent *event);                        //QPaint画图
    void wheelEvent(QWheelEvent *event) override;               //鼠标滚轮滚动
    void mousePressEvent(QMouseEvent *event) override;          //鼠标摁下
    void mouseMoveEvent(QMouseEvent *event) override;           //鼠标松开
    void mouseReleaseEvent(QMouseEvent *event) override;        //鼠标发射事件


private slots:

signals:
    void _status();         //更新状态信号



private:
    QImage Image;           //显示的图片
    qreal ZoomValue = 1.0;  //鼠标缩放值
    int XPtInterval = 0;    //平移X轴的值
    int YPtInterval = 0;    //平移Y轴的值
    QPoint OldPos;          //旧的鼠标位置
    bool Pressed = false;   //鼠标是否被摁压
    QString LocalFileName;  //图片名
    QString save_path;      //保存路径

    //标记使用
    int pic=0;                  //绘画标记位
    int type=0;                 //标记绘画类型为矩形或者椭圆
    int x_start;                //分别是鼠标点击和释放点的xy坐标
    int y_start;
    int x_end;
    int y_end;
    int mousestatus = 0;        //鼠标点击状态
    QPoint startpoint;          //绘画起始点
    QPoint endpoint;            //绘画终止点
    QList<Region> _list;        //保存region的列表
    QList<Region> save_list;    //用于保存结果的region列表

    QFileInfoList * file_list;  //文件列表
    int iter = -5;              //文件计数
    QDir * cur_dir;             //文件夹
};

#endif // MYLABEL_H
