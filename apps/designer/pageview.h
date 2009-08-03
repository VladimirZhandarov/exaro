/***************************************************************************
 *   This file is part of the eXaro project                                *
 *   Copyright (C) 2009 by Alexander Mikhalov  (aka alFoX)                 *
 *              alexmi3@rambler.ru                                         *
 **                   GNU General Public License Usage                    **
 *                                                                         *
 *   This library is free software: you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation, either version 3 of the License, or     *
 *   (at your option) any later version.                                   *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program.  If not, see <http://www.gnu.org/licenses/>. *
 *                                                                         *
 **                  GNU Lesser General Public License                    **
 *                                                                         *
 *   This library is free software: you can redistribute it and/or modify  *
 *   it under the terms of the GNU Lesser General Public License as        *
 *   published by the Free Software Foundation, either version 3 of the    *
 *   License, or (at your option) any later version.                       *
 *   You should have received a copy of the GNU Lesser General Public      *
 *   License along with this library.                                      *
 *   If not, see <http://www.gnu.org/licenses/>.                           *
 *                                                                         *
 *   This library is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 ****************************************************************************/

#ifndef PAGEVIEW_H
#define PAGEVIEW_H

#include <QWidget>
#include <QGraphicsView>
#include <QMouseEvent>
#include "iteminterface.h"

class Selecter;
class QGraphicsScene;
class GraphicsView;
class QRuler;
class QUnit;


class PageView : public QWidget
{
    Q_OBJECT
public:
    PageView(QGraphicsScene * scene = 0, QWidget * parent = 0, Qt::WindowFlags f = 0);
    ~PageView();

    QGraphicsScene * scene() const;
    QGraphicsView * view() const;
    Selecter * selecter() const;
    qreal zoom();

public slots:
    void setZoom(qreal zoom);
    void setZoomFitToPage();

private:
    void setActiveRange(QRect rect);

protected:
//    virtual void resizeEvent ( QResizeEvent * event );
//    virtual void scrollContentsBy ( int dx, int dy );
//    virtual void mouseMoveEvent ( QMouseEvent * e );

private slots:
    void doHorizontalScroll(int value);
    void doVerticalScroll(int value);
    void mousePositionChanged(QPoint pos);
    void sceneDestroyed();

private:
    QGraphicsScene * m_scene;
    GraphicsView * m_view;
    QRuler * m_horizontalRuler;
    QRuler * m_verticalRuler;
    QUnit * m_unit;
    qreal m_zoom;
    QRect m_range;
    Selecter * m_selecter;
};

class GraphicsView: public QGraphicsView
{
    Q_OBJECT
public:
    GraphicsView ( QGraphicsScene * scene, QWidget * parent = 0 ): QGraphicsView(scene, parent) {};
signals:
    void mousePositionChanged (QPoint);
    void selectionMoved(Report::ItemInterface *, QPointF);
protected:
    void mouseMoveEvent ( QMouseEvent * e )
    {
	emit mousePositionChanged ( e->pos () );
	QGraphicsView::mouseMoveEvent( e );
    }
};


#endif // PAGEVIEW_H
