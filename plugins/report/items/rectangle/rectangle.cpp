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
#include <QtCore>
#include <QBrush>
#include <QPainter>
#include <QStyleOptionGraphicsItem>

#include "rectangle.h"
#include "bandinterface.h"

inline void initMyResource()
{
	Q_INIT_RESOURCE(rectangle);
}

Rectangle::Rectangle(QGraphicsItem* parent, QObject* parentObject) : ItemInterface(parent, parentObject), m_xRadius(0), m_yRadius(0)
{
	initMyResource();
}

bool Rectangle::canContain(QObject * object)
{
	return (!dynamic_cast<Report::BandInterface*>(object) && dynamic_cast<Report::ItemInterface*>(object));
}

QRectF Rectangle::boundingRect() const
{
	return QRectF(0, 0, width(), height());
}

qreal Rectangle::xRadius()
{
	return m_xRadius;
}

void Rectangle::setXRadius(qreal radius)
{
	m_xRadius = radius;
	update();
}

qreal Rectangle::yRadius()
{
	return m_yRadius;
}

void Rectangle::setYRadius(qreal radius)
{
	m_yRadius = radius;
	update();
}


void Rectangle::paint(QPainter * painter, const QStyleOptionGraphicsItem * option, QWidget * /*widget*/)
{
	if (option->type != QStyleOption::SO_GraphicsItem)
		emit beforePrint(this);

	QRectF rect = (option->type == QStyleOption::SO_GraphicsItem) ? boundingRect() : option->exposedRect;

	if (option->type == QStyleOption::SO_GraphicsItem)
		drawSelection(painter, rect);

	setupPainter(painter);

	adjustRect(rect);

	painter->drawRoundRect(rect, xRadius(), yRadius());

	if (option->type != QStyleOption::SO_GraphicsItem)
		emit afterPrint(this);
}

QIcon Rectangle::toolBoxIcon()
{
	return QIcon(":/rectangle.png");
};

QString Rectangle::toolBoxText()
{
	return tr("Rectangle");
}

QString Rectangle::toolBoxGroup()
{
	return tr("Shapes");
}

QObject * Rectangle::createInstance(QGraphicsItem* parent, QObject* parentObject)
{
	return new Rectangle(parent, parentObject);
}

Q_EXPORT_PLUGIN2(rectangle, Rectangle)