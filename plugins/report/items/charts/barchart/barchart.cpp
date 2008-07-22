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
#include <QLinearGradient>

#include "barchart.h"

inline void initMyResource()
{
	Q_INIT_RESOURCE(barchart);
}

BarChart::BarChart(QGraphicsItem* parent, QObject* parentObject) : ChartInterface(parent, parentObject), m_barsIdentation(1/UNIT), m_showLabels(false),m_toColorFactor(1),m_drawBorder(false)
{
	initMyResource();
}

QRectF BarChart::boundingRect() const
{
	return QRectF(0, 0, width(), height());
}

void BarChart::paint(QPainter * painter, const QStyleOptionGraphicsItem * option, QWidget * /*widget*/)
{
	if (option->type != QStyleOption::SO_GraphicsItem)
		emit beforePrint(this);

	painter->save();
	QRectF rect = (option->type == QStyleOption::SO_GraphicsItem) ? boundingRect() : option->exposedRect;
	if (option->type == QStyleOption::SO_GraphicsItem)
		drawSelection(painter, boundingRect());

	setupPainter(painter);
	adjustRect(rect);

	QList<ChartInterface::_chartValue> val=values();
	if (!val.size())
	{
		if (option->type != QStyleOption::SO_GraphicsItem)
			emit afterPrint(this);
		painter->restore();
		return;
	}

	double maxVal=0;

	foreach(ChartInterface::_chartValue cv, val)
		if (cv.value>maxVal)
			maxVal=cv.value;

	painter->fillRect(rect,brush());
	if (m_drawBorder)
		painter->drawRect(rect);
	painter->translate(rect.topLeft());
	int x=m_barsIdentation;
	qreal barWidth=(rect.width()-m_barsIdentation*(val.size()+1))/val.size();
	qreal maxHeight=rect.height()-(m_showLabels?painter->fontMetrics().height():1);
	foreach(ChartInterface::_chartValue cv, val)
	{
		qreal barHeight=maxHeight*(cv.value/maxVal)-4;
		QLinearGradient lg(QPointF(x+barWidth/2,maxHeight-barHeight), QPointF(x+barWidth,maxHeight-barHeight));
		lg.setSpread(QGradient::ReflectSpread);
		lg.setColorAt(0, cv.color);
		lg.setColorAt(1, QColor(cv.color.red()*m_toColorFactor, cv.color.green()*m_toColorFactor, cv.color.blue()*m_toColorFactor, cv.color.alpha()));
		painter->fillRect(QRectF(x,maxHeight-barHeight,barWidth, barHeight),QBrush(lg));

		if (m_showLabels)
			painter->drawText(QRectF(x-m_barsIdentation/2, maxHeight, barWidth+m_barsIdentation, painter->fontMetrics().height()),Qt::AlignCenter,QString("%1").arg(cv.value));

		x+=barWidth+m_barsIdentation;
	}

	painter->restore();
	if (option->type != QStyleOption::SO_GraphicsItem)
		emit afterPrint(this);
}

QIcon BarChart::toolBoxIcon()
{
	return QIcon(":/legend.png");
}

QString BarChart::toolBoxText()
{
	return tr("Bar chart");
}

QString BarChart::toolBoxGroup()
{
	return tr("Charts");
}

QObject * BarChart::createInstance(QGraphicsItem* parent, QObject* parentObject)
{
	return new BarChart(parent, parentObject);
}

int BarChart::barsIdentation()
{
	return m_barsIdentation;
}

void BarChart::setBarsIdentation(int barsIdentation)
{
	if (barsIdentation<1)
		barsIdentation=1;
	m_barsIdentation=barsIdentation;
	update();
}

bool BarChart::showLabels()
{
	return m_showLabels;
}

void BarChart::setShowLabels(bool showLabels)
{
	m_showLabels=showLabels;
	update();
}

qreal BarChart::toColorFactor()
{
	return m_toColorFactor;
}

void BarChart::setToColorFactor(qreal toColorFactor)
{
	if (toColorFactor>10)
		toColorFactor=10;

	if (toColorFactor<0.1)
		toColorFactor=0.1;
	m_toColorFactor=toColorFactor;
	update();
}

bool BarChart::drawBorder()
{
	return m_drawBorder;
}
void BarChart::setDrawBorder(bool drawBorder)
{
	m_drawBorder=drawBorder;
	update();
}

Q_EXPORT_PLUGIN2(legend, BarChart)
