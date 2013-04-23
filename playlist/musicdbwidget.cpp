/* This file is part of Pulsar.
   Copyright 2011, Vasily Kiniv <yuberion@gmail.com>

   Pulsar is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.
   
   Pulsar is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.
   
   You should have received a copy of the GNU General Public License
   along with Pulsar.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "musicdbwidget.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QDebug>
#include <QEvent>
#include <QResizeEvent>

MusicDbWidget::MusicDbWidget(QWidget *parent) :
    QWidget(parent)
{
    setObjectName("musicdbWidget");

    setStyleSheet("QLineEdit { border-radius: 2px; background-color: rgba(55, 55, 55, 210); color: #fff; } QWidget#musicdbWidget {padding-right: 90px; border-radius: 7px; background-color: rgba(55, 55, 55, 210); color: #fff; font-size: 15px;}");

//    setMaximumSize(QSize(420, 320));

    hide();

    QVBoxLayout *layout = new QVBoxLayout(this);
    QHBoxLayout *topLayout = new QHBoxLayout();


    m_weSearch = new QLineEdit(this);
    m_weSearch->setToolTip(tr("Search..."));
    m_weSearch->installEventFilter(this);
    connect(m_weSearch, SIGNAL(textChanged(QString)), SIGNAL(searchChanged(QString)));

    topLayout->addWidget(m_weSearch);

    m_wList = new QListView(this);
    m_wList->setEditTriggers(QAbstractItemView::NoEditTriggers);
    m_wList->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    m_wList->setFrameStyle(0);
    m_wList->installEventFilter(this);
    connect(m_wList, SIGNAL(clicked(QModelIndex)), SIGNAL(itemActivated(QModelIndex)));
    m_wList->setStyleSheet("QListView {padding-bottom: 8px; border-radius: 10px; border-top-left-radius: 0px; border-top-right-radius: 0px; border-bottom: none; border-right: none; background-color: rgba(55, 55, 55, 0); color: #fff; font-size: 12px;} \
                           QListView::item  { height: 20x;} \
                           QListView::item:selected {border: none; color: #fff;} \
                           QListView::item:hover {border-radius: 5px; border: none; background-color: rgba(0, 0, 0, 100);} \
                           QScrollBar:vertical {border: none; background-color: rgba(0, 0, 0, 20); width: 6px; } \
                           QScrollBar::handle:vertical {border: none; border-radius: 2px; background-color: #222;} \
                           QScrollBar::add-line:vertical, QScrollBar::sub-line:vertical {height: 0px; border: none;} \
                           QScrollBar::up-arrow:vertical, QScrollBar::down-arrow:vertical {border: none;} \
                           QScrollBar::add-page:vertical, QScrollBar::sub-page:vertical {background: none;} \
                         ");


    m_wbClose = new StyledButton(tr("Close"), this);
    m_wbClose->setStyleSheet("QPushButton { padding-left: 5px; padding-right: 5px; border-radius: 2px; background-color: rgba(55, 55, 55, 150); color: #fff; } \
                                  QPushButton:hover { background-color: rgba(50, 150, 255, 150);} \
                                 ");
    m_wbClose->setMinimumWidth(50);
    connect(m_wbClose, SIGNAL(clicked()), SLOT(hide()));
    topLayout->addWidget(m_wbClose);

    //m_wbClose->setMaximumHeight(m_weSearch->height()-14);

    // Setup loading label
    m_wlLoading = new QLabel("LOADING", m_wList);
    m_wlLoading->setMinimumWidth(160);

    // Set loading qmovie
    m_LoadingMov = new QMovie(":/icons/loading_light");
    m_wlLoading->setMovie(m_LoadingMov);

    showLoading();

    layout->addLayout(topLayout);
    layout->addWidget(m_wList);
}

void MusicDbWidget::setModel(QSortFilterProxyModel *model)
{
    m_wList->setModel(model);
    m_wList->setIconSize(QSize(50, 50));
}

void MusicDbWidget::clear()
{
    m_weSearch->setText("");
}

bool MusicDbWidget::eventFilter(QObject *target, QEvent *event)
{
    if(target == m_weSearch) {
        QLineEdit *p = (QLineEdit*)target;
        if(event->type() == QEvent::FocusIn) {
            if(p->text() == p->toolTip()) {
                p->blockSignals(true);
                p->setText("");
                p->setStyleSheet("");
                p->blockSignals(false);
            }
        }
        if(event->type() == QEvent::FocusOut || event->type() == QEvent::Show) {
            if(p->text().isEmpty()) {
                p->blockSignals(true);
                p->setText(p->toolTip());
                p->setStyleSheet("color: #999;");
                p->blockSignals(false);
            }
        }
    }

    if(target == m_wList) {
        if(event->type() == QEvent::Resize) {
            QResizeEvent *resizeEvent = (QResizeEvent *)event;
            listResize(resizeEvent->size());
        }
    }

    return false;
}

void MusicDbWidget::listResize(QSize size)
{
    Q_UNUSED(size);
    m_wlLoading->move(m_wList->rect().center() - m_wlLoading->rect().center());
}

void MusicDbWidget::paintEvent(QPaintEvent *)
{
    QStyleOption opt;
    opt.init(this);
    QPainter p(this);
    style()->drawPrimitive(QStyle::PE_Widget, &opt, &p, this);
}

void MusicDbWidget::showLoading()
{
    m_LoadingMov->start();
    m_wlLoading->show();
}

void MusicDbWidget::hideLoading()
{
    m_wlLoading->hide();
    m_LoadingMov->stop();
}


