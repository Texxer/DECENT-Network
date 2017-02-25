/*
 *	File      : overview_tab.cpp
 *
 *	Created on: 21 Nov 2016
 *	Created by: Davit Kalantaryan (Email: davit.kalantaryan@desy.de)
 *
 *  This file implements ...
 *
 */
#include "overview_tab.hpp"
#include "gui_wallet_mainwindow.hpp"
#include <QPixmap>
#include <QStackedWidget>
#include <QRect>
#include <QFont>

using namespace gui_wallet;



Overview_tab::Overview_tab(class Mainwindow_gui_wallet* a_pPar)
    : m_pPar(a_pPar)
{

    //table_widget.setMaximumHeight(40);
   // table_widget.setMinimumHeight(40);107 304 529     940

    table_widget.setColumnCount(3);
    table_widget.setRowCount(1);

    QSize tqsTableSize = table_widget.size();
    table_widget.setColumnWidth(0,(tqsTableSize.width()*12)/100);
    table_widget.setColumnWidth(1,(tqsTableSize.width()*32)/100);
    table_widget.setColumnWidth(2,(tqsTableSize.width()*56)/100);

    table_widget.setRowHeight(0,35);

    table_widget.setStyleSheet("QTableView{border : 1px solid lightGray}");

    table_widget.setItem(0,0,new QTableWidgetItem(tr("Info")));
    table_widget.setItem(0,1,new QTableWidgetItem(tr("Asset ID")));
    table_widget.setItem(0,2,new QTableWidgetItem(tr("Author")));

    QFont f( "Arial", 15, QFont::Bold);
    //f.setPointSize(72);
    //font.setBold(true);
    table_widget.item(0,0)->setFont(f);
    table_widget.item(0,1)->setFont(f);
    table_widget.item(0,2)->setFont(f);

    table_widget.item(0,0)->setText("Info");

    table_widget.item(0,0)->setTextAlignment(Qt::AlignHCenter|Qt::AlignVCenter);
    table_widget.item(0,1)->setTextAlignment(Qt::AlignHCenter|Qt::AlignVCenter);
    table_widget.item(0,2)->setTextAlignment(Qt::AlignHCenter|Qt::AlignVCenter);

    table_widget.item(0,0)->setFlags(Qt::ItemIsSelectable|Qt::ItemIsEnabled);
    table_widget.item(0,1)->setFlags(Qt::ItemIsSelectable|Qt::ItemIsEnabled);
    table_widget.item(0,2)->setFlags(Qt::ItemIsSelectable|Qt::ItemIsEnabled);

    table_widget.item(0,0)->setBackground(QColor(228,227,228));
    table_widget.item(0,1)->setBackground(QColor(228,227,228));
    table_widget.item(0,2)->setBackground(QColor(228,227,228));



    table_widget.horizontalHeader()->hide();
    table_widget.verticalHeader()->hide();

    table_widget.setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    table_widget.setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);


    QVBoxLayout* main = new QVBoxLayout();
    main->setContentsMargins(0, 5, 0, 0);
    search.setPlaceholderText(QString("Search"));
    //search.setMaximumHeight(37);
    search.setFixedHeight(40);


    main->addWidget(&search);
    main->addWidget(&table_widget);


    setLayout(main);
}

void Overview_tab::my_slot(int i)
{
    QString str = QString::fromStdString("get_account ") + accounts_names[i];
    std::string tInpuString = str.toStdString();
    SetNewTask(tInpuString,m_pPar,NULL,&Mainwindow_gui_wallet::TaskDoneOverrviewGUI);
}

void Overview_tab::CreateTable()
{
    table_widget.setRowCount(accounts_names.size() + 1);

    for(int i = 0; i < accounts_names.size(); ++i)
    {
        table_widget.setCellWidget(i + 1,0,new NewButton(i));
        table_widget.setItem(i + 1,1,new QTableWidgetItem((accounts_id[i])));
        table_widget.setItem(i + 1,2,new QTableWidgetItem((accounts_names[i])));



        QHBoxLayout* lay = new QHBoxLayout();

        QPixmap image("/Users/vahe/Desktop/info_icon.png");
        QPixmap image1 = image.scaled(QSize(10,10),  Qt::KeepAspectRatio);

        ((NewButton*)table_widget.cellWidget(i + 1,0))->setPixmap(image1);


//        NewButton* new_ptr = new NewButton(i);

//        lay->addWidget(new_ptr);
//        new_ptr = new NewButton(i);
//        new_ptr->setScaledContents(true);
//        new_ptr->setPixmap(image1);

//        lay->addWidget(new_ptr);

//        new_ptr = new NewButton(i);

//        lay->addWidget(new_ptr);

//        table_widget.cellWidget(i + 1 , 0)->setLayout(lay);


       // ((QHBoxLayout*)table_widget.item(i + 1 , 0))->addWidget(new_ptr);


       // ((NewButton*)table_widget.cellWidget(i + 1,0))->setPixmap(image1);

        //new_ptr->setPixmap(image1);

       // new_ptr = new NewButton(i);
       // ((QHBoxLayout*)table_widget.item(i + 1 , 0))->addWidget(new_ptr);

        table_widget.setRowHeight(0,35);

        table_widget.cellWidget(i + 1 , 0)->setStyleSheet("* { background-color: rgb(255,255,255); }");
        table_widget.item(i + 1,1)->setBackground(Qt::white);
        table_widget.item(i + 1,2)->setBackground(Qt::white);

        table_widget.item(i + 1,1)->setTextAlignment(Qt::AlignHCenter|Qt::AlignVCenter);
        table_widget.item(i + 1,2)->setTextAlignment(Qt::AlignHCenter|Qt::AlignVCenter);

        table_widget.item(i + 1,1)->setFlags(Qt::ItemIsSelectable|Qt::ItemIsEnabled);
        table_widget.item(i + 1,2)->setFlags(Qt::ItemIsSelectable|Qt::ItemIsEnabled);

        table_widget.setMouseTracking(true);



    }

    Connects();
    table_widget.setMouseTracking(true);
}

void Overview_tab::Connects()
{
    for(int i = 1; i < accounts_names.size() + 1; ++i)
    {
        connect(table_widget.cellWidget(i,0), SIGNAL(ButtonPushedSignal(int)), this , SLOT(my_slot(int)));

        connect(table_widget.cellWidget(i,0),SIGNAL(mouseWasMoved()),this,SLOT(doRowColor()));
    }
    connect(&table_widget,SIGNAL(mouseMoveEventDid()),this,SLOT(doRowColor()));
}

void Overview_tab::ArrangeSize()
{
    QSize tqsTableSize = table_widget.size();
    table_widget.setColumnWidth(0,(tqsTableSize.width()*12)/100);
    table_widget.setColumnWidth(1,(tqsTableSize.width()*32)/100);
    table_widget.setColumnWidth(2,(tqsTableSize.width()*56)/100);
}

void Overview_tab::resizeEvent(QResizeEvent *a_event)
{
    QWidget::resizeEvent(a_event);
    ArrangeSize();
}

void Overview_tab::doRowColor()
{
    for(int i = 0; i < accounts_names.size(); ++i)
    {
        table_widget.cellWidget(i+1,0)->setStyleSheet("* { background-color: rgb(255,255,255); }");
        table_widget.item(i+1,1)->setBackground(QColor(255,255,255));
        table_widget.item(i+1,2)->setBackground(QColor(255,255,255));
    }
    QPoint mouse_pos = table_widget.mapFromGlobal(QCursor::pos());
    std::cout<<mouse_pos.x()<<mouse_pos.y()<<std::endl;
    QTableWidgetItem *ite = table_widget.itemAt(mouse_pos);

    if(ite != NULL)
    {
        int a = ite->row();
        std::cout<<a<<std::endl;
        if(a != 0)
        {
            table_widget.cellWidget(a , 0)->setStyleSheet("* { background-color: rgb(27,176,104); }");
            table_widget.item(a,1)->setBackgroundColor(QColor(27,176,104));
            table_widget.item(a,2)->setBackgroundColor(QColor(27,176,104));
        }
    }
}

Overview_tab::~Overview_tab()
{

}

TableWidget::TableWidget() : QTableWidget()
{
    this->setMouseTracking(true);
}


void TableWidget::mouseMoveEvent(QMouseEvent *event)
{
    mouseMoveEventDid();
}

