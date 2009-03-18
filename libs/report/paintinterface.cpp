#include "paintinterface.h"
#include <QtSql>
#include <paintdevice.h>
#include <sqlquery.h>
#include "reportinterface.h"
#include "pageinterface.h"


using namespace Report;

PaintInterface::PaintInterface(ReportInterface * report, QObject * parent)
	:QThread(parent)
{
    m_report = report;
}


void PaintInterface::run()
{

    m_report->m_doc.appendChild(m_report->m_doc.createComment("Author '" + m_report->author() + "'"));

    m_report->pdf_file = new QTemporaryFile(QDir::tempPath()+"/XXXXXXXXXXXXX.bdrtpf", this);
    if (!m_report->pdf_file->open())
	throw QString(tr("Can't create temporary files"));


    m_printer = new PaintDevice(m_report->pdf_file);
    m_report->m_exportNode = m_report->m_doc.createElement("export");
    m_painter.begin(m_printer);
    m_painter.setRenderHints(QPainter::Antialiasing | QPainter::TextAntialiasing | QPainter::SmoothPixmapTransform);
    bool first=true;
    foreach(QObject * obj, m_report->children())
    {
	if (!first)
	    m_printer->newPage();
	if (dynamic_cast<PageInterface*>(obj))
	{
	    m_printer->setPaperSize(dynamic_cast<PageInterface*>(obj)->paperRect().size());
	    m_printer->setPaperOrientation((QPrinter::Orientation)dynamic_cast<PageInterface*>(obj)->orientation());

	    m_currentPage = dynamic_cast<PageInterface*>(obj);
	    processPage();
	}
    }
    m_painter.end();
    delete m_printer;
    m_printer = 0;

}

void PaintInterface::processPage()
{
    listTop.clear();
    listBottom.clear();
    listFree.clear();

    LayoutManager::splitOnLayoutTypesSorted(m_currentPage, &listTop, &listBottom, &listFree);

    initBands();

    freeSpace = m_currentPage->geometry();
    this->m_currentPageNumber = 1;
    m_report->m_scriptEngine->globalObject().setProperty("_page_", QScriptValue(m_report->m_scriptEngine, 1), QScriptValue::ReadOnly);

    prepareCurrentPage();

    bandDone.clear();

    foreach(BandInterface * band, listTop)
	if (band->accommodationType() == band->AccomodationOnce && !bandDone.contains(band))
	    if (band->query().isEmpty())
		processBand(band, pmNormal);
    else
	processQuery(band->query(), band);

}

void PaintInterface::finish(QString error)
{
    showError(error);
    deleteLater () ;
}

void PaintInterface::initBands()
{
    qDebug("PaintInterface::initBands()");

    foreach(BandInterface * band, listTop)
	if (!band->init(this))
	    showError(QString("Error in item \'%1\' \n%2").arg(band->objectName()).arg(band->lastError()));
	else
	    foreach(QObject * obj, band->QObject::children())
		if (dynamic_cast<Report::ItemInterface*>(obj))
		    dynamic_cast<Report::ItemInterface*>(obj)->init(this);
    foreach(BandInterface * band, listBottom)
	if (!band->init(this))
	    showError(QString("Error in item \'%1\' \n%2").arg(band->objectName()).arg(band->lastError()));
    	else
	    foreach(QObject * obj, band->QObject::children())
		if (dynamic_cast<Report::ItemInterface*>(obj))
		    dynamic_cast<Report::ItemInterface*>(obj)->init(this);
    foreach(BandInterface * band, listFree)
	if (!band->init(this))
	    showError(QString("Error in item \'%1\' \n%2").arg(band->objectName()).arg(band->lastError()));
    	else
	    foreach(QObject * obj, band->QObject::children())
		if (dynamic_cast<Report::ItemInterface*>(obj))
		    dynamic_cast<Report::ItemInterface*>(obj)->init(this);
}


void PaintInterface::showError(QString err)
{
    qDebug("Error:\n %s", qPrintable(err));
}

void PaintInterface::processBand(BandInterface * band, PrintMode pMode)
{
    qDebug("PaintInterface::processBand");
	if (!band /*|| m_reportCanceled*/)
		return;
	if (!band->isEnabled())
	    return;

	if (!canPaint(band) )
	    newPage();

	m_painter.save();

	#warning    "FIXME - need optimization and proceed child item only if band be paint"
	foreach(QObject * obj, ((QObject*)band)->children())		//preProcess all child items
	{
	    qDebug("process child name=%s",qPrintable(obj->objectName()));
	    ItemInterface * item = dynamic_cast<ItemInterface *>(obj);
	    if (item)
		if (!item->prePaint(&m_painter, pMode))
		    finish(item->lastError());
	}

	if (!band->prePaint(&m_painter, pMode))				//preProcess band
	{
	    m_painter.restore();
	    return;
	}

	if (band->layoutType() == BandInterface::LayoutBottom)
		m_painter.translate(/*freeSpace.x() +*/ band->x(), freeSpace.bottom() - band->geometry().height());
	else
	    if (band->layoutType() == BandInterface::LayoutTop)
		m_painter.translate(/*freeSpace.x() +*/ band->x(), freeSpace.top());
	    else
		if (band->layoutType() == BandInterface::LayoutFree)
		    m_painter.translate(band->geometry().x(), band->geometry().y());

//	m_report->m_currentHeight = 0;
//	m_currentSize = QSize(0,0);

	    ///NEED CHECK FOR GROUP

	paintObjects(band, QPointF(0, 0), QRectF(0, 0, m_currentPage->geometry().width(), m_currentPage->geometry().height()));

	foreach(QObject * obj, ((QObject*)band)->children())		//postProcess all child items
	{
	    ItemInterface * item = dynamic_cast<ItemInterface *>(obj);
	    if (item)
		item->postPaint();
	}

	band->postPaint();				//postProcess band

//	m_report->m_currentHeight += band->indentation();

	if (band->layoutType()== BandInterface::LayoutBottom)
		freeSpace.setBottom( freeSpace.bottom() - band->geometry().height() -  band->indentation());
	else
		freeSpace.setTop( freeSpace.top() + band->geometry().height() + band->indentation());
//	band->unstretch();
	m_painter.restore();
}



void PaintInterface::newPage()
{
    postprocessCurrentPage();
    m_currentPageNumber++;
//    m_report->m_splashScreen.showMessage(tr("Prepare page: %1").arg(m_currentPageNumber));
    m_report->m_scriptEngine->globalObject().setProperty("_page_", QScriptValue(m_report->m_scriptEngine, m_currentPageNumber), QScriptValue::ReadOnly);
    m_report->m_scriptEngine->globalObject().setProperty("_rpage_", QScriptValue(m_report->m_scriptEngine, m_report->m_scriptEngine->globalObject().property("_rpage_").toInteger() + 1), QScriptValue::ReadOnly);
    m_printer->newPage();
    freeSpace = m_currentPage->geometry();
    prepareCurrentPage();
    emit showProcess(tr("Prepare page: %1").arg(m_currentPageNumber));
}


bool PaintInterface::canPaint(BandInterface * band)
{
	return (freeSpace.top() + band->geometry().height() <= freeSpace.bottom());
}


void PaintInterface::prepareCurrentPage()
{
    m_report->m_currentTop = m_currentPage->geometry().y();
    m_report->m_currentBottom = m_currentPage->geometry().bottom();

    foreach(BandInterface * band, listTop)
	if (band->accommodationType() == band->AccomodationEveryPage || (band->accommodationType() == band->AccomodationFirstPage && m_currentPageNumber == 1))
	    processBand(band);

    for (int i = listBottom.count()-1; i>=0 ;i--)
    {
	BandInterface * band = listBottom.at(i);
	if (band->accommodationType() == band->AccomodationEveryPage || (band->accommodationType() == band->AccomodationFirstPage && m_currentPageNumber == 1))
	    processBand(band);
    }

    qDebug("size of  current group is %i", currentGroup.size());

    if (!currentGroup.isEmpty())
	foreach (BandInterface * band, currentGroup)
	   // if (band->reprintOnNewPage())
		processBand(band, pmNewPage);
}

void PaintInterface::postprocessCurrentPage()
{
    foreach(BandInterface * band, listFree)
	if (band->accommodationType() == band->AccomodationEveryPage || (band->accommodationType() == band->AccomodationFirstPage && m_currentPageNumber == 1))
	    processBand(band);
}


void PaintInterface::paintObjects(ItemInterface * item, QPointF translate, const QRectF & clipRect)
{
	if (!item || !item->isEnabled())
		return;

	QStyleOptionGraphicsItem option;
	option.type = 31;
	option.exposedRect = dynamic_cast<BandInterface *>(item) ? QRectF(0, 0, dynamic_cast<BandInterface *>(item)->geometry().width(), dynamic_cast<BandInterface *>(item)->geometry().height()) : item->geometry();
	m_painter.save();
	option.exposedRect.translate(translate);
	m_painter.setClipRect(clipRect);
	item->paint(&m_painter, &option);
	m_painter.restore();
	translate += option.exposedRect.topLeft();
	foreach(QObject * obj, ((QObject*)item)->children())
		paintObjects(dynamic_cast<ItemInterface *>(obj), translate, option.exposedRect);
}


void PaintInterface::processQuery(QString queryName, BandInterface * band )
{
    SqlQuery * query = m_report->findChild<SqlQuery *>(queryName);
    if (!query)
    {
	QString bandName = band ? band->objectName(): "Unknown";
	finish(tr("Query named \'%1\' not found for band \'%2\'").arg(queryName).arg(bandName) );
    }

    //already exec()'d ?
    if (!query->first())
	if (!(query->exec() && query->first()))
	    finish(tr("query \'%1\' exacution error:\n").arg(queryName).arg(query->lastError().text()));

    QDomElement qryElement = m_report->m_doc.createElement("query");
    qryElement.setAttribute("name", query->objectName());
    QDomElement rowElement = m_report->m_doc.createElement("row");
    for (int i = 0;i < query->record().count();i++)
    {
	QDomElement field = m_report->m_doc.createElement("field");
	field.setAttribute("type", "columnName");
	field.appendChild(m_report->m_doc.createTextNode(query->record().fieldName(i)));
	rowElement.appendChild(field);
    }
    qryElement.appendChild(rowElement);

    m_report->m_scriptEngine->globalObject().setProperty("_line_", QScriptValue(m_report->m_scriptEngine, 0), QScriptValue::ReadOnly);

    m_currentQuery = query;
    m_currentQueryRow = 0;
    m_currentLineNumber = 0;

    currentGroup.clear();

    foreach(BandInterface * band, listTop)
	if (band->query() == queryName)
	    currentGroup.append(band);

    do
    {
	m_currentQueryRow++;
	m_currentLineNumber++;
	m_report->m_scriptEngine->globalObject().setProperty("_line_", QScriptValue(m_report->m_scriptEngine, m_currentLineNumber), QScriptValue::ReadOnly);

	foreach(BandInterface * band, currentGroup)
	    processBand(band);

	m_report->exportRecord(query->record(), qryElement);
    }
    while (query->next());

    m_report->m_exportNode.appendChild(qryElement);

    foreach (BandInterface * band, currentGroup)
	bandDone.append(band);
    currentGroup.clear();
}

int PaintInterface::currentPageNumber()
{
    return m_currentPageNumber;
}

int PaintInterface::currentQueryRow()
{
    return m_currentQueryRow;
}

void PaintInterface::setDetailNumber(int num)
{
    m_currentLineNumber = num;
    m_report->m_scriptEngine->globalObject().setProperty("_line_", QScriptValue(m_report->m_scriptEngine, m_currentLineNumber), QScriptValue::ReadOnly);
}

/*
void PaintInterface::exportRecord(const QSqlRecord & record, QDomElement & el)
{
	QDomElement rowElement = m_doc.createElement("row");
	for (int i = 0;i < record.count();i++)
	{
		QDomElement field = m_doc.createElement("field");
		field.setAttribute("type", typeIsNumber(record.field(i).value().type()) ? QString("float") : QString("string"));
		field.appendChild(m_doc.createTextNode(record.field(i).value().toString()));
		rowElement.appendChild(field);
	}
	el.appendChild(rowElement);
}
*/

/*
	foreach(BandInterface * detailContainerBand, m_detailContainerBands)
	{
		QList<BandInterface *> detailHeaders;
		QList<BandInterface *> details;
		QList<BandInterface *> detailFooters;
		prepareCurrentPage();

		m_scriptEngine->globalObject().setProperty("_detailNumber_", QScriptValue(m_scriptEngine, 0), QScriptValue::ReadOnly);

		foreach(QObject * obj, ((QObject*)detailContainerBand)->children())
		{
			BandInterface * bd = dynamic_cast<BandInterface *>(obj);
			if (!bd)
				continue;
			switch (bd->bandType())
			{
				case BandInterface::DetailHeader:
					addOrderedBand(detailHeaders, bd);
					break;
				case BandInterface::Detail:
					addOrderedBand(details, bd);
					break;
				case BandInterface::DetailFooter:
					addOrderedBand(detailFooters, bd);
					break;
				default:
					break;
			}
		}
		SqlQuery * detailQuery = detailContainerBand->findQuery(detailContainerBand->query());
		if (!detailQuery || !detailQuery->isActive())
		{
			// no query ?!?!?!?
			foreach(BandInterface * band, detailHeaders)
			{
				if (!canPaint(band))
					newPage();
				processBand(band);
			}
			foreach(BandInterface * band, details)
			{
				if (!canPaint(band))
					newPage();
				processBand(band);
			}

			foreach(BandInterface * band, detailFooters)
			{
				if (!canPaint(band))
					newPage();
				processBand(band);
			}
			continue;
		}
//==
		if (!detailQuery->isValid())
			detailQuery->first();

		QDomElement qryElement = m_doc.createElement("query");
		qryElement.setAttribute("name", detailQuery->objectName());
		QDomElement rowElement = m_doc.createElement("row");
		for (int i = 0;i < detailQuery->record().count();i++)
		{
			QDomElement field = m_doc.createElement("field");
			field.setAttribute("type", "columnName");
			field.appendChild(m_doc.createTextNode(detailQuery->record().fieldName(i)));
			rowElement.appendChild(field);
		}
		qryElement.appendChild(rowElement);

		bool first=true;
		do
		{
			foreach(BandInterface * band, detailFooters)
			{
				if (!band->isEnabled())
				    continue;
				if (!band->groupFieldValue().isValid())
					band->setGroupFieldValue(detailContainerBand->queryField(detailContainerBand->query(), band->groupField()));

				if (band->groupFieldValue() != detailContainerBand->queryField(detailContainerBand->query(), band->groupField()))
				{
					if (!canPaint(band))
						newPage();
					processBand(band);
					band->setGroupFieldValue(detailContainerBand->queryField(detailContainerBand->query(), band->groupField()));
				}
			}

			foreach(BandInterface * band, detailHeaders)
				if (band->groupFieldValue() != detailContainerBand->queryField(detailContainerBand->query(), band->groupField()))
				{
					if (!band->isEnabled())
					    continue;
					if (!canPaint(band) || (!first && band->forceNewPage()))
						newPage();
					processBand(band);
					band->setGroupFieldValue(detailContainerBand->queryField(detailContainerBand->query(), band->groupField()));
					if (band->resetDetailNumber())
						m_scriptEngine->globalObject().setProperty("_detailNumber_", QScriptValue(m_scriptEngine, 0), QScriptValue::ReadOnly);
				}

			m_scriptEngine->globalObject().setProperty("_detailNumber_", QScriptValue(m_scriptEngine, m_scriptEngine->globalObject().property("_detailNumber_").toInteger() + 1), QScriptValue::ReadOnly);

			foreach(BandInterface * band, details)
			{
				if (!band->isEnabled())
				    continue;
				if (!canPaint(band))
				{
					newPage();
					foreach(BandInterface * band, detailHeaders)
						if (band->reprintOnNewPage())
							processBand(band);
				}
				processBand(band);
			}
			exportRecord(detailQuery->record(), qryElement);
			first=false;
		}
		while (detailQuery->next());

		m_exportNode.appendChild(qryElement);

		foreach(BandInterface * band, detailFooters)
		{
			if (!band->isEnabled())
			    continue;
			if (!canPaint(band))
				newPage();
			processBand(band);
		}

		foreach(BandInterface * band, detailFooters)
			band->setGroupFieldValue(QVariant());

		foreach(BandInterface * band, detailHeaders)
			band->setGroupFieldValue(QVariant());
	}
	*/
