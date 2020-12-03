#include "mylabel.h"


MyLabel::MyLabel(QWidget *parent):QLabel(parent)
{
    //painter1=new QPainter(this);
    cur_dir=new QDir;
    file_list = new QFileInfoList;
}

MyLabel::~MyLabel(){
    delete cur_dir;
    delete file_list;
}

QTextStream & operator <<(QTextStream & o,Region & region)   //重载运算符"<<"以便写数据
{
    return o<<region.img_name<<" "<<region.label<<" "<<region.start.x()<<" "<<region.start.y()<<" "
           <<region.end.x()<<" "<<region.end.y()<<" "<<region.shape<<endl;
}


//QPainter画图
void MyLabel::paintEvent(QPaintEvent *event)
{
    if (LocalFileName.isNull())
        return QWidget::paintEvent(event);

    Image.load(LocalFileName);
    QPainter painter(this);

    // 根据窗口计算应该显示的图片的大小
    int width = qMin(Image.width(), this->width());
    int height = width * 1.0 / (Image.width() * 1.0 / Image.height());
    height = qMin(height, this->height());
    width = height * 1.0 * (Image.width() * 1.0 / Image.height());

    // 平移
    painter.translate(this->width() / 2 + XPtInterval, this->height() / 2 + YPtInterval);

    // 缩放
    painter.scale(ZoomValue, ZoomValue);

    // 绘制图像
    QRect picRect(-width / 2, -height / 2, width, height);
    painter.drawImage(picRect, Image);

    painter.resetTransform();

    painter.setPen(QPen(Qt::black,2));
    QFont font("微软雅黑",15,QFont::Bold,true);
    painter.setFont(font);
    painter.setBrush(QColor(0,0,0,90));//最后一位是设置透明属性（在0-255取值）

    Region temp;
    foreach (temp, _list) {
        if(temp.label=="1"){
            painter.setBrush(QColor(255,0,0,50));//最后一位是设置透明属性（在0-255取值）
            painter.setPen(QPen(Qt::red,2));
        }
        if(temp.label=="2"){
            painter.setBrush(QColor(0,255,0,50));//最后一位是设置透明属性（在0-255取值）
            painter.setPen(QPen(Qt::green,2));
        }
        if(temp.label=="3"){
            painter.setBrush(QColor(0,0,255,50));//最后一位是设置透明属性（在0-255取值）
            painter.setPen(QPen(Qt::blue,2));
        }

        if(temp.shape==0){
            painter.drawRect(QRect(temp.start,temp.end));
        }
        else if (temp.shape==1) {
            painter.drawEllipse(temp.start.rx(),temp.start.ry(),(temp.end.rx()-temp.start.rx()),(temp.end.ry()-temp.start.ry()));
        }
        painter.drawText(temp.start-QPoint(0,10),temp.label);
    }

    if(pic==0){
        if(type==0){
            painter.drawRect(QRect(startpoint,endpoint));
        }
        else if (type==1) {
            painter.drawEllipse(startpoint.rx(),startpoint.ry(),(endpoint.rx()-startpoint.rx()),(endpoint.ry()-startpoint.ry()));
        }
     }
}

//鼠标滚轮滚动
void MyLabel::wheelEvent(QWheelEvent *event)
{
    int value = event->delta();
    if (value > 0)  //放大
        OnZoomInImage();
    else            //缩小
        OnZoomOutImage();


    update();
}
//鼠标摁下
void MyLabel::mousePressEvent(QMouseEvent *event)
{
    if(event->button() == Qt::RightButton){
        OldPos = event->pos();
        Pressed = true;
    }

    if(event->button() == Qt::LeftButton)
    {
        startpoint = event->globalPos();
        startpoint = this->mapFromGlobal(startpoint);
        startpoint=event->pos();
        qDebug()<<startpoint<<endl;
        mousestatus = 1;
        pic=0;
        x_start = startpoint.x();
        y_start = startpoint.y();
    }

}
//鼠标移动
void MyLabel::mouseMoveEvent(QMouseEvent *event)
{

    if(event->buttons() & Qt::LeftButton)
    {
        endpoint = event->globalPos();
        endpoint = this->mapFromGlobal(endpoint);
        endpoint=event->pos();
        qDebug()<<endpoint<<endl;
        update();
    }

    else if(event->buttons()& Qt::RightButton){
        if (!Pressed)
            return QWidget::mouseMoveEvent(event);

        this->setCursor(Qt::SizeAllCursor);
        QPoint pos = event->pos();
        int xPtInterval = pos.x() - OldPos.x();
        int yPtInterval = pos.y() - OldPos.y();

        XPtInterval += xPtInterval;
        YPtInterval += yPtInterval;

        OldPos = pos;
        update();
    }

}
//鼠标发射事件
void MyLabel::mouseReleaseEvent(QMouseEvent *event)
{
    if(event->button() == Qt::RightButton){
        Pressed = false;
        setCursor(Qt::ArrowCursor);
    }
    else if(event->button() == Qt::LeftButton)
    {
        endpoint = event->globalPos();
        endpoint = this->mapFromGlobal(endpoint);
        endpoint=event->pos();
        qDebug()<<endpoint<<endl;
        x_end = endpoint.x();
        y_end = endpoint.y();
        qDebug()<<"mouse released"<<endl;
        //update();

        mousestatus = 0;
        bool ok;
        QString label = QInputDialog::getText(this,tr("添加标签"),tr("请输入标签"),QLineEdit::Normal,0,&ok);
        pic=1;
        if(ok&& !label.isEmpty()){
         _list.push_back(Region(LocalFileName,label,startpoint,endpoint,type));
         pic=0;
        }
        repaint();

//        if(mode)
//        {
//            _list.push_back(Region(cur_file_name,startpoint,endpoint));
//        }
//        else
//        {
//            _list.clear();
//            _list.push_back(Region(cur_file_name,startpoint,endpoint));
//        }
    }

}

//选择打开图片
void MyLabel::OnSelectImage()
{
    LocalFileName = QFileDialog::getOpenFileName(this, "Open Image", "./", tr("Images (*.png *.bmp *.jpg)"));

    QFile file(LocalFileName);
    if (!file.exists())
        return;
    _list.clear();

    ZoomValue = 1.0;
    XPtInterval = 0;
    YPtInterval = 0;

//    QImage *img=new QImage(LocalFileName);
//    this->setPixmap(QPixmap::fromImage(*img));
//    this->resize(QSize(img->width(),img->height()));
    pic=1;
    update();

    infor= "单张图片";
    emit _status();
}

//保存标注信息
void MyLabel::OnSave(){
    save_path = QFileDialog::getOpenFileName(this,"请选择一个文本文件来保存...","","*.txt");
    if(save_path.isEmpty())
    {
        return;
    }

    QFile save_file(save_path);
    if(save_file.exists())
    {
        if(!save_file.open(QIODevice::ReadWrite | QIODevice::Text))
        {
            QMessageBox::information(this, tr("Information"), tr("无法打开文件"));
            return;
        }
        else
        {
            QTextStream stream(&save_file);
            stream.seek(save_file.size());                 //找到文件末尾
            Region temp;
            foreach (temp, _list) {
                stream<<temp;
            }
            _list.clear();
            pic=1;
            save_file.close();
        }
    }
    else
    {
        QMessageBox::information(this, tr("Information"), tr("未选择保存位置，请创建一个文本文档来保存"));
        return;
    }

    infor= "标注信息已保存";
    emit _status();        //发射信号
}

//导入已保存的文件
void MyLabel::OnLoad()
{
    QString load_name = QFileDialog::getOpenFileName(this, "Open TXT", "./", tr("TXT (*.txt)"));
    QFile file(load_name);
    if(!file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        qDebug()<<"Can't open the file!"<<endl;
        return;
    }
    QTextStream stream1(&file);
    QString line_in;
    QStringList lines;
    Region temp;

    stream1.seek(0);//将当前读取文件指针移动到文件开始
    _list.clear();
     while( !stream1.atEnd())
     {
         line_in = stream1.readLine();
         lines = line_in.split(" ");
         temp.img_name=lines[0];
         temp.label=lines[1];
         temp.start=QPoint(lines[2].toInt(),lines[3].toInt());
         temp.end=QPoint(lines[4].toInt(),lines[5].toUInt());
         temp.shape=lines[6].toInt();
         qDebug() << temp.img_name;
         qDebug() << temp.label;
         qDebug() << temp.start;
         qDebug() << temp.end;
         qDebug() << temp.shape;
         _list.push_back(temp);
     }
     LocalFileName=temp.img_name;
     type=temp.shape;
     update();

     infor= "标注信息已导入";
     emit _status();
}

//图片放大
void MyLabel::OnZoomInImage()
{
    ZoomValue += 0.05;
    update();

}
//图片缩小
void MyLabel::OnZoomOutImage()
{
    ZoomValue -= 0.05;
    if (ZoomValue <= 0)
    {
        ZoomValue = 0.05;
        return;
    }

    update();

}
//图片还原
void MyLabel::OnPresetImage()
{
    ZoomValue = 1.0;
    XPtInterval = 0;
    YPtInterval = 0;
    update();
}

//清除标记
void MyLabel::OnClearLab()
{
    _list.clear();
    update();
    pic=1;

    infor= "标注已清除";
    emit _status();
}

//打开文件夹
void MyLabel::OnOpenDir(){
    QString cur_path = QFileDialog::getExistingDirectory(this,"请选择文件夹...","./");
    if(cur_path.isEmpty())
    {
         return;
    }
    qDebug()<<cur_path;

    cur_dir->setPath(cur_path);
    if(!cur_dir->exists()){
        qDebug()<<"fail";
        return;
    }

    QStringList filters;
    filters << "*.jpg"<<"*.bmp"<<"*.png";//设置过滤类型
    cur_dir->setNameFilters(filters);//设置文件名的过滤
    *file_list = cur_dir->entryInfoList();
    if(file_list->isEmpty()){
        QMessageBox::information(this, tr("Information"), tr("本文件夹不含有图片！\n 请重新选择。"));
        return;
    }
    iter = 0; //计数重置为0
    LocalFileName=file_list->at(iter).absoluteFilePath();
    repaint();

    infor= "第 "+QString::number(iter+1)+"/"+QString::number(file_list->size())+" 张图片";
    emit _status();

    _list.clear();
    pic=1;
    update();
}

//打开下一张图片
void MyLabel::OnNext(){
    Region temp;
    if(!_list.isEmpty()){
        foreach(temp,_list){
            save_list.push_back(temp);
        }
        _list.clear();
    }
    qDebug()<<"222";
    //更换图片
    iter++;
    pic=1;
    if(iter>=file_list->size()||iter<0)
    {
        QMessageBox::information(this, tr("Information"), tr("已经是最后一张图片，请选择下一个文件夹"));
        iter=file_list->size()-1;
        return;
    }
    LocalFileName=file_list->at(iter).absoluteFilePath();
    update();

    infor= "第 "+QString::number(iter+1)+"/"+QString::number(file_list->size())+" 张图片";
    emit _status();
}

//打开上一张图片
void MyLabel::OnPre(){
    Region temp;
    if(!_list.isEmpty()){
        foreach(temp,_list){
            save_list.push_back(temp);
        }
        _list.clear();
    }
    //更换图片
    iter--;
    pic=1;
    if(iter>=file_list->size()||iter<0)
    {
        QMessageBox::information(this, tr("Information"), tr("已经是第一张图片"));
        iter=0;
        return;
    }
    LocalFileName=file_list->at(iter).absoluteFilePath();

    update();

    infor= "第 "+QString::number(iter+1)+"/"+QString::number(file_list->size())+" 张图片";
    emit _status();
}

//切换矩形/椭圆
void MyLabel::OnChange(){
    type=1-type;
    infor=(type==0?"可绘制矩形":"可绘制椭圆");
    emit _status();        //发射信号

}

//右键菜单
void MyLabel::contextMenuEvent(QContextMenuEvent *event)
{
    QPoint pos = event->pos();
    pos = this->mapToGlobal(pos);
    QMenu *menu = new QMenu(this);

    QAction *loadImage = new QAction(this);
    loadImage->setText("选择图片");
    connect(loadImage, &QAction::triggered, this, &MyLabel::OnSelectImage);
    menu->addAction(loadImage);

    QAction *saveAction = new QAction(this);
    saveAction->setText("保存");
    connect(saveAction, &QAction::triggered, this, &MyLabel::OnSave);
    menu->addAction(saveAction);

    QAction *loadAction = new QAction(this);
    loadAction->setText("导入");
    connect(loadAction, &QAction::triggered, this, &MyLabel::OnLoad);
    menu->addAction(loadAction);

    menu->addSeparator();

//    QAction *zoomInAction = new QAction(this);
//    zoomInAction->setText("放大");
//    connect(zoomInAction, &QAction::triggered, this, &MyLabel::OnZoomInImage);
//    menu->addAction(zoomInAction);

//    QAction *zoomOutAction = new QAction(this);
//    zoomOutAction->setText("缩小");
//    connect(zoomOutAction, &QAction::triggered, this, &MyLabel::OnZoomOutImage);
//    menu->addAction(zoomOutAction);

    QAction *presetAction = new QAction(this);
    presetAction->setText("还原");
    connect(presetAction, &QAction::triggered, this, &MyLabel::OnPresetImage);
    menu->addAction(presetAction);

    QAction *clearAction = new QAction(this);
    clearAction->setText("清除");
    connect(clearAction, &QAction::triggered, this, &MyLabel::OnClearLab);
    menu->addAction(clearAction);


    menu->exec(pos);
}
