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

#ifndef REPORTINTERFACE_H
#define REPORTINTERFACE_H

#include <QObject>
#include <QMap>
#include <QList>
#include <QVariant>
#include <QDomDocument>
#include <QPrinter>
#include <QPainter>
#include <QFile>
#include <QScriptEngine>
#include <QUiLoader>
#include <QSqlRecord>
#include <QSplashScreen>
#include <QSqlDatabase>

#include "pageinterface.h"
#include "bandinterface.h"
#include "globals.h"


/** \namespace Report */
namespace Report
{

/** \class ReportInterface
* \brief Interface for reports
*
* This is the base class for all report(s) object(s).
*/

class PaintDevice;
class KONTAMABIL_EXPORTS ReportInterface : public QObject
{

	Q_OBJECT
	Q_PROPERTY(QString name READ name WRITE setName)
	Q_PROPERTY(QString author READ author WRITE setAuthor)
	Q_PROPERTY(QString script READ script WRITE setScript DESIGNABLE false)
	Q_PROPERTY(QVariantMap queries READ queries WRITE setQueries DESIGNABLE false)
	Q_PROPERTY(QVariantMap uis READ uis WRITE setUis DESIGNABLE false)

public:
	/**
	 * ReportInterface constructor
	 * @param parent parent object
	 */
	ReportInterface(QObject *parent = 0);

	virtual ~ReportInterface();

	/**
	 * Exec the report. I the report is not canceled the preview dialog is shown.
	 * @return true if the report is not canceled
	 */
	virtual bool exec();


	/**
	 * Return report name
	 * @return report name
	 * @see setName()
	 */
	QString name();
	/**
	 * Set the report name.
	 * @param name report name
	 * @see name()
	 */
	void setName(const QString & name);

	/**
	 * The report author.
	 * @return report author
	 * @see setAuthor()
	 */
	QString author();
	/**
	 * Set report author
	 * @param author report author
	 * @see author()
	 */
	void setAuthor(const QString & author);

	/**
	 * Before you add a object to report call this method to see if the report can containt the object.
	 * @param object object you want to add
	 * @return true if the report can contain the object
	 */
	virtual bool canContain(QObject * object);

	/**
	 * Create the report instance.
	 * @param parent parent object
	 * @return report instance
	 */
	virtual Report::ReportInterface * createInstance(QObject * parent) = 0;

	/**
	 * Return report script
	 * @return report script
	 */
	QString script();
	/**
	 * Set the report script
	 * @param script report script
	 */
	virtual void setScript(const QString & script);

	/**
	 * Return the report queries
	 * @return queries
	 */
	QVariantMap queries();
	/**
	 * Sets the report queries
	 * @param queries report queries
	 */
	void setQueries(QVariantMap queries);

	/**
	 * Return the report uis
	 * @return
	 */
	QVariantMap uis();
	/**
	 * Sets the report uis. The uis can be used in script to interact with the user.
	 * @param uis
	 */
	void setUis(QVariantMap uis);

	/**
	 * Return the script engine
	 * @return script engine
	 */
	QScriptEngine * scriptEngine();

	/**
	 * Retrun global objects
	 * @return global objects
	 */
	const QObjectList & globalObjects();

	/**
	 * Sets the globalObjects.
	 * @param objectList
	 */
	void setGlobalObjects(const QObjectList & objectList);

	/**
	 * Sets the report database
	 * @param db
	 */
	void setDatabase(const QSqlDatabase & db);

public slots:
	/**
	 * Call this slot to cancel the report execution
	 */
	void cancelReport();

signals:
	/**
	 * This signal is triggered before report execution
	 */
	void beforeExec();

	/**
	 * This signal is triggered after report execution
	 */
	void afterExec();


protected:
	virtual void paintPage(PageInterface * page);
	virtual void paintBand(BandInterface * band);
	virtual void setScriptEngineGlobalVariables();
	virtual	void paintOverlays();

protected:
	PaintDevice * m_printer;
	QPainter m_painter;
	QDomDocument m_doc;
	QDomNode m_exportNode;
	QScriptEngine * m_scriptEngine;

private:
	void setReportObjectsToScriptEngineGlobalVariables(QObject * object);
	void addOrderedBand(QList<BandInterface *> & m_bands, BandInterface * band);
	void newPage();
	void prepareCurrentPage();
	bool canPaint(BandInterface * band);
	void exportRecord(const QSqlRecord & record, QDomElement & el);
	void paintObjects(ItemInterface * item, QPointF translate, const QRectF & clipRect);
	void cleanUpObjects();
	bool isNumber(QVariant::Type type);

private:
	bool m_reportCanceled;
	QString m_name;
	QString m_author;
	QString m_script;
	QVariantMap m_queries;
	QVariantMap m_uis;
	QList<BandInterface *> m_pageHeaderBands;
	QList<BandInterface *> m_titleBands;
	QList<BandInterface *> m_detailContainerBands;
	QList<BandInterface *> m_summaryBands;
	QList<BandInterface *> m_pageFooterBands;
	QList<BandInterface *> m_pageOverlayBands;
	QObjectList m_objectList;
	qreal m_currentHeight, m_currentTop, m_currentBottom;
	PageInterface * m_currentPage;
	QSplashScreen m_splashScreen;
	QSqlDatabase m_sqlDatabase;
};
}
Q_DECLARE_INTERFACE(Report::ReportInterface, "ro.bigendian.ReportDesigner.ReportInterface/1.0");
#endif