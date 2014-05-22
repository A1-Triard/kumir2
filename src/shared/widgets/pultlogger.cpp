//
//  pultlogger.cpp
//  Kumir2
//
//  Created by Denis Khachko on 22.05.14.
//  Copyright (c) 2014 __MyCompanyName__. All rights reserved.
//

#include <iostream>
#include "pultlogger.h"


loggerButton::loggerButton (QDir dir, QWidget* parent):QWidget(parent)
{isUpArrow=false;
	posX=1;
	posY=1;
    buttonImageUp.load(dir.absoluteFilePath("butt.png"));
	buttonImageDown.load(dir.absoluteFilePath("buttd.png")); 
    downFlag=false;
	Parent=parent;
	int mid=buttonImageUp.width()/2;
	upArrow.append(QLine(mid,10,mid-11,15));
	upArrow.append(QLine(mid,10,mid+11,15));
	downArrow.append(QLine(mid,15,mid-11,10));
	downArrow.append(QLine(mid,15,mid+11,10));
};
void loggerButton::loadButtons(QDir dir)
{
    
    //    qDebug()<<"Dir"<<dir.absoluteFilePath("butt.png");
    //	  //  buttonImageUp.load("/Volumes/Untitled 1/Users/mordol/Kumir2.0/SshGitRep/BuildSecSt12/Debug/Kumir.app/Contents/Resources/actors/turtle/butt.png");
    //	buttonImageDown.load(dir.absoluteFilePath("buttd.png"));    
}
void loggerButton::paintEvent ( QPaintEvent * event )
{
    Q_UNUSED(event);
    QPainter painter(this);
    painter.setRenderHint (QPainter::Antialiasing,true );
    if(!downFlag)
    {
		painter.drawImage(QPoint(posX,posY),buttonImageUp);
		QPen blackPen(QColor(50,50,50));
		blackPen.setWidth(3);
		painter.setPen(blackPen);
		if(isUpArrow)painter.drawLines(upArrow);else painter.drawLines(downArrow);
	}
	else 	{
		painter.drawImage(QPoint(posX,posY),buttonImageDown);
		QPen whitePen(QColor(170,170,170));
		whitePen.setWidth(3);
		painter.setPen(whitePen);
		if(isUpArrow)painter.drawLines(upArrow);else painter.drawLines(downArrow);
	};
};


void loggerButton::mousePressEvent ( QMouseEvent * event )
{
	Q_UNUSED(event);
	qWarning("MousePress");
	emit pressed();
	downFlag=true;
	repaint();
};
void loggerButton::mouseReleaseEvent ( QMouseEvent * event )
{
	Q_UNUSED(event);
	downFlag=false;
	repaint();
};





pultLogger::pultLogger ( QDir resDir,QWidget* parent):QWidget(parent)
{
	mainFrame= new QFrame(parent);
	mainFrame->setLineWidth(2);
	mainFrame->setFrameShadow(QFrame::Sunken);
	mainFrame->setFrameShape(QFrame::Panel);
	mainFrame->setPalette(QPalette(QColor(50,50,50),QColor(100,100,100)));
	mainFrame->setBackgroundRole(QPalette::Window);
	mainFrame->setAutoFillBackground(true);
    
	dummyFrame= new QFrame(mainFrame);
	dummyFrame->setLineWidth(0);
	dummyFrame->setFrameShadow(QFrame::Sunken);
	dummyFrame->setFrameShape(QFrame::Panel);
	dummyFrame->setPalette(QPalette(QColor(50,50,50),QColor(100,100,100)));
	dummyFrame->setBackgroundRole(QPalette::Window);
	dummyFrame->setAutoFillBackground(true);
	dummyFrame->move(2,2);
	W=150;H=160;
	respFrame= new QFrame(mainFrame);
	respFrame->setLineWidth(0);
	respFrame->setFrameShadow(QFrame::Sunken);
	respFrame->setFrameShape(QFrame::Panel);
	respFrame->setPalette(QPalette(QColor(50,50,50),QColor(120,110,110)));
	respFrame->setBackgroundRole(QPalette::Window);
	respFrame->setAutoFillBackground(true);
	respFrame->move(W-RESP_PANEL,2);
    
    
	downButton=new loggerButton(resDir,parent);
    //  upButton->loadButtons(resDir);
	downButton->move(0,H-24);
	downButton->resize(140,24);
	downButton->show();
	upButton=new loggerButton(resDir,parent);
    //  upButton->loadButtons(resDir);
	upButton->move(0,8);
	upButton->resize(140,26);
	upButton->upArrowType(true);
	upButton->show();
	pos=4;
	connect(downButton,SIGNAL(pressed()),this,SLOT(downBtnPressed()));
	connect(upButton,SIGNAL(pressed()),this,SLOT(upBtnPressed()));
	//buttonSize=50;
    
};
pultLogger::~pultLogger ()
{
    //delete mainFrame;
};

void pultLogger::Move(uint x,uint y)
{
	mainFrame->move( x, y+26);
	//dummyFrame->move( x+1, y+27);
	int offset=(W-LOGGER_BUTTONS)/2;
	downButton->move(x+offset,y+H-24);
	upButton->move(x+offset,y);
	//testLabel->move(4,30);
	qDebug()<<"upBtn geom "<<upButton->geometry();
	qWarning("WidgetMove");
	move(x,y);
};

void pultLogger::setSizes(uint w,uint h)
{
	mainFrame->resize ( w, h-50);
	dummyFrame->resize ( w-RESP_PANEL, h);
	respFrame->resize ( RESP_PANEL, h);
	respFrame->move(w-RESP_PANEL,2);
	qDebug()<<"dummy geom"<<dummyFrame->geometry();
	resize(w,h);
	W=w;H=h;
	buttonSize=w;
};


void pultLogger::upBtnPressed()
{
    
    if(lines.count()==0)return;
    if(lines.first().pos()>2)return;
    
    for(int i=0;i<lines.count();i++)lines[i].moveDown();
    pos=pos+SCROLL_STEP;
    qWarning("TEXT MOVE");
};
void pultLogger::downBtnPressed()
{
    if(pos<14)return;
    for(int i=0;i<lines.count();i++)lines[i].moveUp();
    pos=pos-SCROLL_STEP;
};

void pultLogger::ClearLog()
{
    for(int i=0;i<lines.count();i++)lines[i].removeLabels();
    lines.clear();
    pos=4;
    
};
void pultLogger::CopyLog()
{
    if(lines.count()==0)return;
    QClipboard * cp=QApplication::clipboard ();
    QString text;
    for(int i=0;i<lines.count();i++)if(!lines[i].KumCommand().isEmpty())text+=lines[i].KumCommand()+"\n";
    cp->setText(text);
};