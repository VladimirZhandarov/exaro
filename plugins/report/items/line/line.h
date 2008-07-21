/***************************************************************************
 *   Copyright (C) 2008 by BogDan Vatra                                    *
 *   bogdan@licentia.eu                                                    *
 *                                                                         *
 *   This program is free software: you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation, either version 3 of the License, or     *
 *   (at your option) any later version.                                   *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program.  If not, see <http://www.gnu.org/licenses/>. *
 ***************************************************************************/
#ifndef LINE_H
#define LINE_H

#include <iteminterface.h>

class Line : public Report::ItemInterface
{
	Q_OBJECT
	Q_INTERFACES(Report::ItemInterface);
	Q_ENUMS(LineStyle)
	Q_PROPERTY(LineStyle lineStyle READ lineStyle WRITE setLinestyle)

public:
	enum LineStyle {Vertical, Horizontal, BackwardDiagonal, ForwardDiagonal};
public:
	Line(QGraphicsItem* parent = 0, QObject* parentObject = 0);

	QRectF boundingRect() const;
	void paint(QPainter * painter, const QStyleOptionGraphicsItem * option, QWidget * widget = 0);

	QIcon toolBoxIcon();
	QString toolBoxText();
	QString toolBoxGroup();

	LineStyle lineStyle();
	void setLinestyle(LineStyle lineStyle);

	QObject * createInstance(QGraphicsItem* parent = 0, QObject* parentObject = 0);
private:
	LineStyle m_lineStyle;
};

#endif