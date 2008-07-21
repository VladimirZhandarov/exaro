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

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QList>
#include <QDomDocument>

#include "reportengine.h"


#include "ui_mainwindow.h"
#include "propertyeditor.h"
#include "designerquerywidget.h"
#include "designeruiwidget.h"
#include "objectmodel.h"

class QTabWidget;

class QTreeView;

class QToolBox;

class mainWindow : public QMainWindow, private Ui::mainWindow
{
	Q_OBJECT

public:
	mainWindow(QWidget* parent = 0, Qt::WFlags fl = 0);
	~mainWindow();

	void closeEvent(QCloseEvent *event);

protected:
	void keyReleaseEvent(QKeyEvent * event);
	void connectItem(QObject * item);
	void pasteItem(QObject * item);
	void setMagnetActions(Report::PageInterface* page);

private:
	void openReport(const QString & report);
	bool selectObject(QObject * object, QModelIndex index);

private:
	ObjectModel m_objectModel;
	QTreeView * m_objectInspector;
	Report::ReportEngine m_reportEngine;
	QString m_saveFile, m_designerTool;
	QTabWidget *m_tw;
	QToolBox * m_tb;
	PropertyEditor::PropertyEditor *m_pe;
	Report::DesignerQueryWidget * m_dquery;
	Report::DesignerUiWidget * m_dui;
	Report::ReportInterface* m_report;
	QObject * m_lastSelectedObject;
	QMenu	m_contextMenu;
	QDockWidget * m_dwToolBox;
	QDockWidget * m_dwPropertyEditor;
	QDockWidget * m_dwQueryEditor;
	QDockWidget * m_dwUiEditor;
	QDockWidget * m_dwObjectInspector;

protected slots:
	void newPage();
	void removePage();
	void currentChanged(int);
	void itemSelected(QObject *, QPointF);
	void zoomIn();
	void zoomOut();
	void zoomOriginal();
	void zoomWYSIWYG();
	void editScript();
	void saveReport();
	void saveReportAs();
	void openReport();
	void newReport();
	void executeReport();
	void setupDatabase();
	void copy();
	void paste();
	void del();
	void cut();
	void setDesignerPath();
	void objectChanged(const QModelIndex & current, const QModelIndex & previous);
	void about();

signals:
	void setCurrentIndex(const QModelIndex & , QItemSelectionModel::SelectionFlags);
};

#endif
