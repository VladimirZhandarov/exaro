/***************************************************************************
 *   This file is part of the eXaro project                                *
 *   Copyright (C) 2008 by BogDan Vatra                                    *
 *   bog_dan_ro@yahoo.com                                                  *
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

#include <QtCore>
#include <QBrush>
#include <QPainter>
#include <QStyleOptionGraphicsItem>

#include "detailfooter.h"

inline void initMyResource()
{
	Q_INIT_RESOURCE(detailFooter);
}

DetailFooter::DetailFooter(QGraphicsItem* parent, QObject* parentObject): BandInterface(parent, parentObject)
{
	initMyResource();
//	setBandType(Report::BandInterface::DetailFooter);
	setResizeFlags(FixedPos | ResizeBottom);
}


DetailFooter::~DetailFooter()
{
}

bool DetailFooter::init(Report::PaintInterface * paintInterface)
{
    m_groupValue = "";
    m_agregateCounter = 0;
    needResetAgregateValues = true;
    return ItemInterface::init(paintInterface);
}

bool DetailFooter::prePaint(QPainter * painter, Report::PaintInterface::PrintMode pMode)
{
    ItemInterface::prePaint(painter);
    bool ok = false;
    switch (pMode)
    {
	case Report::PaintInterface::pmNormal:
	    if (needResetAgregateValues)
		resetAgregateValues();

	    accumulateAgregateValues();

	    if (!m_condition.isEmpty())
	    {
		m_groupValue = processString(m_condition);	    
		findQuery(query())->next();		    // query lookahead

		if (m_groupValue != processString(m_condition))
		    ok = true;
		else
		    ok = false;

		findQuery(query())->previous();		    //back
	    }
	    else
		ok = true;

	    if (ok)
	    {
		needResetAgregateValues = true;
		return true;
	    }
	    else
	    {
		needResetAgregateValues = false;
		return false;
	    }
	    break;
	case Report::PaintInterface::pmNewPage:
	    return false;
	    break;
	default:
	    return false;
    }

    return false;
}


bool DetailFooter::canContain(QObject * object)
{
	return (!dynamic_cast<Report::BandInterface*>(object) && dynamic_cast<Report::ItemInterface*>(object));
}

QRectF DetailFooter::boundingRect() const
{
	return QRectF(0, 0, width(), height());
}

void DetailFooter::paint(QPainter * painter, const QStyleOptionGraphicsItem * option, QWidget * /*widget*/)
{
	if (option->type != QStyleOption::SO_GraphicsItem)
		emit beforePrint(this);

	QRectF rect = (option->type == QStyleOption::SO_GraphicsItem) ? boundingRect() : option->exposedRect;

	setupPainter(painter);
	painter->fillRect(rect,painter->brush());

	if (option->type == QStyleOption::SO_GraphicsItem)
	{
		drawSelection(painter, rect);
		drawTitle(tr("Detail footer"), TitleLeft, Qt::AlignCenter);
	}

	adjustRect(rect);

	if (frame()&DrawLeft)
		painter->drawLine(rect.left(), rect.top(), rect.left(), rect.bottom());

	if (frame()&DrawRight)
		painter->drawLine(rect.right(), rect.top(), rect.right(), rect.bottom());

	if (frame()&DrawTop)
		painter->drawLine(rect.left(), rect.top(), rect.right(), rect.top());

	if (frame()&DrawBottom)
		painter->drawLine(rect.left(), rect.bottom(), rect.right(), rect.bottom());

	if (option->type != QStyleOption::SO_GraphicsItem)
		emit afterPrint(this);
}

QIcon DetailFooter::toolBoxIcon()
{
	return QIcon(":/detailFooter.png");
}

QString DetailFooter::toolBoxText()
{
	return tr("Detail footer");
}

QString DetailFooter::toolBoxGroup()
{
	return tr("Bands");
}

QObject * DetailFooter::createInstance(QGraphicsItem* parent, QObject* parentObject)
{
	return new DetailFooter(parent, parentObject);
}

QString DetailFooter::condition()
{
    return m_condition;
}

void DetailFooter::setCondition(const QString & condition)
{
    m_condition = condition;
}

Q_EXPORT_PLUGIN2(detailFooter, DetailFooter)

