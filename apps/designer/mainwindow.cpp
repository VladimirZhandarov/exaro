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

#include <QDockWidget>
#include <QSettings>
#include <QDir>
#include <QToolBox>
#include <QDate>
#include <QDateTime>
#include <QTime>
#include <QProcess>
#include <QTreeView>
#include <QItemSelectionModel>
#include <QMetaClassInfo>
#include <QUndoView>

#include <QPluginLoader>
#include <QFileDialog>
#include <QMessageBox>
#include <QDesktopWidget>
#include <QtSql>

#include "mainwindow.h"
#include "designerpage.h"
#include "sqldatabasedialog.h"
#include "iteminterface.h"
#include "aboutdialog.h"
#include "optionsdialog.h"
#include "namevalidator.h"
#include "pageview.h"
#include "layoutmanager.h"
#include "bandinterface.h"
#include "selecter.h"

#define ROWS_IN_MENU  10
#define STATIC_TABS	2
#define FILE_FORMAT_VERSION 3.0

#define screen_heightMM (((double)QDesktopWidget().screen()->height() /(double)QDesktopWidget().screen()->physicalDpiY() )*25.4)
#define screen_widthMM (((double)QDesktopWidget().screen()->width() /(double)QDesktopWidget().screen()->physicalDpiX() )*25.4)

mainWindow::mainWindow( QWidget* parent, Qt::WFlags fl )
		: QMainWindow( parent, fl ), m_report( 0 )
{
	setupUi( this );
	setWindowTitle( tr( "eXaro v%1 unsaved report" ).arg( EXARO_VERSION ) );

#ifdef Q_OS_MAC
	toolBar->setFloatable( false );
	setUnifiedTitleAndToolBarOnMac( true );
#endif

	m_tb = new QToolBox( this );
	m_dwToolBox = new QDockWidget( tr( "Tool Box" ) , this );
	m_dwToolBox->setObjectName( "Tool Box" );
	m_dwToolBox->setWidget( m_tb );
	m_dwToolBox->setAllowedAreas( Qt::AllDockWidgetAreas );
	addDockWidget( Qt::LeftDockWidgetArea, m_dwToolBox );

	m_pe = new PropertyEditor::PropertyEditor( this );
	m_dwPropertyEditor = new QDockWidget( tr( "Property Editor" ) , this );
	m_dwPropertyEditor->setObjectName( "Property Editor" );
	m_dwPropertyEditor->setWidget( m_pe );
	m_dwPropertyEditor->setAllowedAreas( Qt::AllDockWidgetAreas );
	addDockWidget( Qt::RightDockWidgetArea, m_dwPropertyEditor );
	m_nameValidator = new NameValidator( this );
	m_pe->setValidator( QVariant::String, m_nameValidator );

	m_dwUiEditor = new QDockWidget( tr( "Ui Editor" ), this );
	m_dui = new Report::DesignerUiWidget( m_dwUiEditor );
	m_dwUiEditor->setObjectName( "Ui Editor" );
	m_dwUiEditor->setWidget( m_dui );
	m_dwUiEditor->setAllowedAreas( Qt::AllDockWidgetAreas );
	addDockWidget( Qt::BottomDockWidgetArea, m_dwUiEditor );

	m_objectInspector = new QTreeView( this );
	m_objectInspector->setModel( &m_objectModel );
	m_objectInspector->setHeaderHidden( true );
//	m_objectInspector->setSelectionMode( QAbstractItemView::SingleSelection );
	m_objectInspector->setSelectionMode( QAbstractItemView::MultiSelection );
	QItemSelectionModel *selectionModel = new QItemSelectionModel( &m_objectModel );
	m_objectInspector->setSelectionModel( selectionModel );

	m_dwObjectInspector = new QDockWidget( tr( "Object Inspector" ), this );
	m_dwObjectInspector->setObjectName( "Object Inspector" );
	m_dwObjectInspector->setWidget( m_objectInspector );
	m_dwObjectInspector->setAllowedAreas( Qt::AllDockWidgetAreas );
	addDockWidget( Qt::BottomDockWidgetArea, m_dwObjectInspector );

	undoStack = new QUndoStack( this );
	undoView = new QUndoView( undoStack );
	undoView->setWindowTitle( tr( "Command List" ) );
	//undoView->show();
	//undoView->setAttribute(Qt::WA_QuitOnClose, false);
	m_dwUndoView = new QDockWidget( tr( "Command List" ), this );
	m_dwUndoView->setObjectName( "Command List" );
	m_dwUndoView->setWidget( undoView );
	m_dwUndoView->setAllowedAreas( Qt::AllDockWidgetAreas );
	addDockWidget( Qt::BottomDockWidgetArea, m_dwUndoView );

	m_tw = new QTabWidget( this );
	setCentralWidget( m_tw );

	actionRemove_page->setEnabled( false );

	connect( actionNew_report, SIGNAL( triggered( bool ) ), SLOT( newReport() ) );

	connect( actionSave_report, SIGNAL( triggered( bool ) ), SLOT( saveReport() ) );
	connect( actionSave_report_as, SIGNAL( triggered( bool ) ), SLOT( saveReportAs() ) );

	connect( actionOpen_report, SIGNAL( triggered( bool ) ), SLOT( openReport() ) );
	connect( menuOpen_last_report, SIGNAL( aboutToShow() ), SLOT( prepareLastReportMenu() ) );

	connect( actionOpen_template, SIGNAL( triggered( bool ) ), SLOT( openTemplate() ) );
	connect( menuOpen_last_temlate, SIGNAL( aboutToShow() ), SLOT( prepareLastTemplateMenu() ) );

	connect( actionNew_page, SIGNAL( triggered( bool ) ), SLOT( newPage() ) );
	connect( actionRemove_page, SIGNAL( triggered( bool ) ), SLOT( removePage() ) );

	connect( actionExit, SIGNAL( triggered( bool ) ), SLOT( close() ) );

	connect( actionZoom_in, SIGNAL( triggered( bool ) ), SLOT( zoomIn() ) );
	connect( actionZoom_out, SIGNAL( triggered( bool ) ), SLOT( zoomOut() ) );
	connect( actionZoom_original, SIGNAL( triggered( bool ) ), SLOT( zoomOriginal() ) );

	connect( actionExecute, SIGNAL( triggered( bool ) ), SLOT( saveReport() ) );
	connect( actionExecute, SIGNAL( triggered( bool ) ), SLOT( executeReport() ) );

	connect( actionSetup_a_database, SIGNAL( triggered( bool ) ), SLOT( setupDatabase() ) );

	connect( actionCopy, SIGNAL( triggered( bool ) ), SLOT( copy() ) );
	connect( actionCut, SIGNAL( triggered( bool ) ), SLOT( cut() ) );
	connect( actionPaste, SIGNAL( triggered( bool ) ), SLOT( paste() ) );
	connect( actionDelete, SIGNAL( triggered( bool ) ), SLOT( del() ) );
	connect( actionSave_item_as, SIGNAL( triggered( bool ) ), SLOT( saveItem() ) );
	connect( actionOpen_item_from, SIGNAL( triggered( bool ) ), SLOT( openItem() ) );

	connect( action_About_eXaro, SIGNAL( triggered( bool ) ), SLOT( about() ) );
	connect( actionOptions, SIGNAL( triggered( bool ) ), SLOT( options() ) );

	//connect( actionUndo, SIGNAL( triggered( bool ) ), undoStack, SLOT( undo() ) );
	//connect( actionRedo, SIGNAL( triggered( bool ) ), undoStack, SLOT( redo() ) );

	connect( m_tw, SIGNAL( currentChanged( int ) ), SLOT( currentChanged( int ) ) );

	connect( this, SIGNAL( setCurrentIndex( const QModelIndex&, QItemSelectionModel::SelectionFlags ) ), selectionModel, SLOT( setCurrentIndex( const QModelIndex&, QItemSelectionModel::SelectionFlags ) ) );
	connect( selectionModel, SIGNAL( currentChanged( const QModelIndex & , const QModelIndex & ) ), SLOT( objectChanged( const QModelIndex & , const QModelIndex & ) ) );

	connect( m_pe, SIGNAL( propertyChanged( QObject *, const QString & , const QVariant & , const QVariant & ) ),
	                this, SLOT( propertyChanged( QObject *, const QString & , const QVariant & , const QVariant & ) ) );

	m_contextMenu.addAction( actionCopy );
	m_contextMenu.addAction( actionCut );
	m_contextMenu.addAction( actionPaste );
	m_contextMenu.addAction( actionDelete );
	m_contextMenu.addSeparator();
	m_contextMenu.addAction( actionZoom_in );
	m_contextMenu.addAction( actionZoom_out );
	m_contextMenu.addAction( actionZoom_original );
	m_contextMenu.addSeparator();
	m_contextMenu.addAction( actionSave_item_as );
	m_contextMenu.addAction( actionOpen_item_from );

	m_report = m_reportEngine.reports()[0]->createInstance( 0 );
	m_report->setObjectName( "report" );
	m_report->setName( tr( "report name" ) );
	m_report->setAuthor( "(c) 2008 BogDan" );
	m_pe->setObject( m_report );
	m_nameValidator->setRootObject( m_report );
	m_saveFile = "";

	for ( int i = 0;i < m_reportEngine.items().uniqueKeys().size();i++ )
	{
		m_tb->addItem( new QListWidget( this ), m_reportEngine.items().uniqueKeys()[i] );
		m_tb->setCurrentIndex( m_tb->count() - 1 );

		for ( int j = 0;j < m_reportEngine.items().values( m_reportEngine.items().uniqueKeys()[i] ).count();j++ )
			new QListWidgetItem( m_reportEngine.items().values( m_reportEngine.items().uniqueKeys()[i] )[j]->toolBoxIcon(), m_reportEngine.items().values( m_reportEngine.items().uniqueKeys()[i] )[j]->toolBoxText(), dynamic_cast<QListWidget*>( m_tb->currentWidget() ) );

		dynamic_cast<QListWidget*>( m_tb->currentWidget() )->setCurrentRow( -1 );
	}

	m_tb->setCurrentIndex( 0 );
	m_lastSelectedObject = 0;
	if ( 2 == qApp->arguments().size() )
		openReport( qApp->arguments()[1] );

	m_dwToolBox->toggleViewAction()->setIcon( QIcon( ":/images/button_tool.png" ) );
	m_dwPropertyEditor->toggleViewAction()->setIcon( QIcon( ":/images/button_property.png" ) );
	m_dwUiEditor->toggleViewAction()->setIcon( QIcon( ":/images/button_uieditor.png" ) );
	m_dwObjectInspector->toggleViewAction()->setIcon( QIcon( ":/images/button_objects.png" ) );
	m_dwUndoView->toggleViewAction()->setIcon( QIcon( ":/images/button_commands.png" ) );

	menuTools->addSeparator();
	menuTools->addAction( m_dwToolBox->toggleViewAction() );
	menuTools->addAction( m_dwPropertyEditor->toggleViewAction() );
	menuTools->addAction( m_dwUiEditor->toggleViewAction() );
	menuTools->addAction( m_dwObjectInspector->toggleViewAction() );
	menuTools->addAction( m_dwUndoView->toggleViewAction() );

	toolBarTools->addAction( m_dwToolBox->toggleViewAction() );
	toolBarTools->addAction( m_dwPropertyEditor->toggleViewAction() );
	toolBarTools->addAction( m_dwUiEditor->toggleViewAction() );
	toolBarTools->addAction( m_dwObjectInspector->toggleViewAction() );
	toolBarTools->addAction( m_dwUndoView->toggleViewAction() );

	m_objectModel.setRootObject( m_report );

	m_smTemplate = 0;
	m_smReport = 0;
	setupActions();

	restoreSettings();

	m_wdataset = new Report::DesignerDatasetEditor( &m_reportEngine,  m_tw);
	m_tw->addTab(m_wdataset , tr("Data"));

	m_dscript = new Report::DesignerScriptWidget( m_tw );
	m_tw->addTab(m_dscript, tr("Script"));

	newReport();
}

void mainWindow::restoreSettings()
{   
	QSettings s;

	QString iSize = s.value( "Options/iconSize" ).toString();
	int h, w;
	h = iSize.section( "x", 0, 0 ).toInt();
	w = iSize.section( "x", 1, 1 ).toInt();
	if ( h == 0 or w == 0 )
		toolBar->setIconSize( QSize( 16, 16 ) );
	else
		toolBar->setIconSize( QSize( w, h ) );

	s.beginGroup( "Main_window" );
	restoreGeometry( s.value( "Geometry", saveGeometry() ).toByteArray() );
	restoreState( s.value( "State", saveState() ).toByteArray() );
	s.endGroup();

	Report::LayoutManager::setMargin( s.value( "Options/margin", 0 ).toInt() * 10 );
	Report::BandInterface::setTitleEnabled( s.value( "Options/drawTitles", true ).toBool() );

//	if (Report::BandInterface::hasTitle() != s.value( "Options/drawTitles", true ).toBool() )
//	{
	    for (int i=STATIC_TABS; i<m_tw->count(); i++)
	    {
		foreach (QObject * obj, dynamic_cast<PageView*>(m_tw->widget( i ))->scene()->findChildren<Report::BandInterface*>() )
		    if (dynamic_cast<Report::BandInterface*> (obj))
			dynamic_cast<Report::BandInterface*> (obj)->showTitle( ( s.value( "Options/drawTitles", true ).toBool()));
		Report::LayoutManager::updatePositions(dynamic_cast<PageView*>(m_tw->widget( i ))->scene() );
	    }
//	}

}


void mainWindow::setupActions()
{
	actionOpen_report->setShortcuts( QKeySequence::Open );
	actionSave_report->setShortcuts( QKeySequence::Save );
	actionNew_page->setShortcuts( QKeySequence::New );
	actionRemove_page->setShortcuts( QKeySequence::Close );
	actionDelete->setShortcuts( QKeySequence::Delete );

	actionCut->setShortcuts( QKeySequence::Cut );
	actionCopy->setShortcuts( QKeySequence::Copy );
	actionPaste->setShortcuts( QKeySequence::Paste );
	actionUndo->setShortcuts( QKeySequence::Undo );
	actionRedo->setShortcuts( QKeySequence::Redo );
	actionZoom_in->setShortcuts( QKeySequence::ZoomIn );
	actionZoom_out->setShortcuts( QKeySequence::ZoomOut );
	connect( undoStack, SIGNAL( canUndoChanged( bool ) ), actionUndo, SLOT( setEnabled( bool ) ) );
	connect( undoStack, SIGNAL( canRedoChanged( bool ) ), actionRedo, SLOT( setEnabled( bool ) ) );
	actionUndo->setEnabled( undoStack->canUndo() );
	actionRedo->setEnabled( undoStack->canRedo() );
}

void mainWindow::saveItem()
{
	if ( !m_lastSelectedObject )
		return;
	QString fileName = QFileDialog::getSaveFileName( this, tr( "Save item" ),
	                QDir::homePath() + "/item.bdrti", tr( "Item (*.bdrti)" ) );
	if ( !fileName.length() )
		return;

	QFile file( fileName );

	if ( file.open( QIODevice::WriteOnly | QIODevice::Text ) )
	{
		QDomDocument doc( "report" );
		doc.appendChild( Report::ReportEngine::objectProperties( m_lastSelectedObject, &doc ) );
		file.write( doc.toByteArray( 4 ) );
		file.close();
	}
}

void mainWindow::openItem()
{
	m_lastSelectedObject = m_pe->object();
	if ( !dynamic_cast<Report::ItemInterface*>( m_lastSelectedObject ) && !dynamic_cast<Report::PageInterface *>( m_lastSelectedObject ) )
		return;

	QString reportName = QFileDialog::getOpenFileName( this, tr( "Open report" ),
	                QDir::homePath() + "", tr( "Item (*.bdrti)" ) );

	if ( !reportName.size() )
		return;

	QFile file( reportName );
	if ( file.open( QIODevice::ReadOnly ) )
	{
		if ( dynamic_cast<Report::ItemInterface*>( m_lastSelectedObject ) )
			undoStack->push( new AddDomObject( dynamic_cast<Report::PageInterface *>( dynamic_cast<Report::ItemInterface*>( m_lastSelectedObject )->scene() ), m_lastSelectedObject->objectName(), file.readAll(),  m_lastSelectedObjectPos, this ) );
		else
			undoStack->push( new AddDomObject( dynamic_cast<Report::PageInterface *>( m_lastSelectedObject ), m_lastSelectedObject->objectName(), file.readAll(),  m_lastSelectedObjectPos, this ) );
	}
}

void mainWindow::about()
{
	AboutDialog d;
	d.exec();
}

void mainWindow::objectChanged( const QModelIndex & current, const QModelIndex & /*previous */ )
{
	m_pe->setObject( reinterpret_cast<ObjectModel::ObjectStruct*>( current.internalPointer() )->object );
//	foreach( QObject * obj, m_report->children() )
//		if ( dynamic_cast<Report::PageInterface *>( obj ) )
//			dynamic_cast<Report::PageInterface *>( obj )->clearSelection();

//	if ( dynamic_cast<Report::ItemInterface *>( reinterpret_cast<ObjectModel::ObjectStruct*>( current.internalPointer() )->object ) )
//		dynamic_cast<Report::ItemInterface *>( reinterpret_cast<ObjectModel::ObjectStruct*>( current.internalPointer() )->object )->setSelected( true );
}


bool mainWindow::selectObject( QObject * object, QModelIndex index )
{
	if ( index.isValid() && reinterpret_cast<ObjectModel::ObjectStruct *>( index.internalPointer() )->object == object )
	{
		emit( setCurrentIndex( index, QItemSelectionModel::SelectCurrent ) );
		return true;
	}

	for ( int i = 0;i < m_objectModel.rowCount( index );i++ )
		if ( selectObject( object, m_objectModel.index( i, 0, index ) ) )
			return true;
	return false;
}

bool mainWindow::askToSaveReport()
{
    Q_ASSERT(undoStack);
	if ( undoStack->index() )
		switch ( QMessageBox::question( this, tr( "eXaro" ), tr( "Save changes ?" ), QMessageBox::Yes | QMessageBox::No | QMessageBox::Cancel ) )
		{
			case QMessageBox::Yes:
				saveReport();
				if ( !m_saveFile.length() )
					return false;
				break;
			case QMessageBox::Cancel:
				return false;
		}
	return true;
}

void mainWindow::setMagnetActions( Report::PageInterface* page )
{
	if ( !page )
		return;

	connect( actionLeft_magnet, SIGNAL( toggled( bool ) ), page, SLOT( setLeftMagnet( bool ) ) );
	connect( actionRight_magnet, SIGNAL( toggled( bool ) ), page, SLOT( setRightMagnet( bool ) ) );
	connect( actionTop_magnet, SIGNAL( toggled( bool ) ), page, SLOT( setTopMagnet( bool ) ) );
	connect( actionBottom_magnet, SIGNAL( toggled( bool ) ), page, SLOT( setBottomMagnet( bool ) ) );
	connect( actionVertical_magnet, SIGNAL( toggled( bool ) ), page, SLOT( setVerticalMagnet( bool ) ) );
	connect( actionHorizontal_magnet, SIGNAL( toggled( bool ) ), page, SLOT( setHorizontalMagnet( bool ) ) );

	page->setLeftMagnet( actionLeft_magnet->isChecked() );
	page->setRightMagnet( actionRight_magnet->isChecked() );
	page->setTopMagnet( actionTop_magnet->isChecked() );
	page->setBottomMagnet( actionBottom_magnet->isChecked() );
	page->setVerticalMagnet( actionVertical_magnet->isChecked() );
	page->setHorizontalMagnet( actionHorizontal_magnet->isChecked() );
}

void mainWindow::copy()
{
	if ( m_lastSelectedObject )
		m_reportEngine.copy( m_lastSelectedObject );
}

void mainWindow::pasteItem( QObject * item )
{
	connect( item, SIGNAL( itemSelected( QObject *, QPointF, Qt::KeyboardModifiers  ) ), SLOT( itemSelected( QObject *, QPointF, Qt::KeyboardModifiers  ) ) );
	connect( item, SIGNAL( geometryChanged( QObject*, QRectF, QRectF ) ), SLOT( itemGeometryChanged( QObject*, QRectF, QRectF ) ) );
	item->setObjectName( Report::ReportEngine::uniqueName( item->metaObject()->className(), m_report ) );
	foreach( QObject * obj, item->children() )
		pasteItem( obj );
}

void mainWindow::paste()
{
	if ( !m_lastSelectedObject )
		return;

	QObject * obj = m_reportEngine.paste( m_lastSelectedObject );
	Report::ItemInterface* item = dynamic_cast<Report::ItemInterface*>( obj );
	if ( dynamic_cast<Report::ItemInterface*>( m_lastSelectedObject ) )
	{
		if ( !dynamic_cast<Report::ItemInterface*>( m_lastSelectedObject )->canContain( item ) )
		{
			delete item;
			item = 0;
		}
	}
	else
		if ( dynamic_cast<Report::PageInterface*>( m_lastSelectedObject )->canContain( item ) )
			dynamic_cast<Report::PageInterface*>( m_lastSelectedObject )->addItem( item );
		else
		{
			delete item;
			item = 0;
		}

	if ( item )
	{
		pasteItem( item );
		if ( dynamic_cast<Report::BandInterface*>( item ) )
			dynamic_cast<Report::BandInterface*>( item )->setOrder( INT_MAX );

		m_pe->setObject( item );
		m_objectModel.setRootObject( m_report );
		selectObject( item, m_objectModel.index( 0, 0 ) );
	}
}

void mainWindow::del()
{
	QGraphicsItem* item;
	if ( !( item = dynamic_cast<QGraphicsItem*>( m_lastSelectedObject ) ) || !dynamic_cast<Report::ItemInterface*>( m_lastSelectedObject ) )
		return;
	QUndoCommand *delCommand = new DelCommand( dynamic_cast<Report::ItemInterface*>( item ), this );
	undoStack->push( delCommand );
}

void mainWindow::cut()
{
	copy();
	del();
}

mainWindow::~mainWindow()
{
}


void mainWindow::setupDatabase()
{
	SqlDatabaseDialog d;
	d.exec();
	m_wdataset ->resetConnection();
}

void mainWindow::executeReport()
{
    	m_wdataset ->sync();
	m_report->exec();
}

void mainWindow::closeEvent( QCloseEvent *event )
{
	QSettings s;
	s.beginGroup( "Main_window" );
	s.setValue( "Geometry", saveGeometry() );
	s.setValue( "State", saveState() );
	s.endGroup();
	if ( askToSaveReport() )
		event->accept();
	else
		event->ignore();
}

void mainWindow::newReport( bool notAsk )
{
	if ( !notAsk && !askToSaveReport() )
		return;

	while ( m_tw->count() > STATIC_TABS )
	{
		m_tw->setCurrentIndex( STATIC_TABS );
		removePage();
	}
	delete m_report;
	m_report = m_reportEngine.reports()[0]->createInstance( 0 );
	m_report->setObjectName( "report" );
	m_report->setName( tr( "Report name" ) );
	m_report->setAuthor( "(c) 2008 BogDan" );
	m_report->setVersion( FILE_FORMAT_VERSION );
	refreshReportBeholders(m_report);
	m_saveFile = "";
	undoStack->clear();
	setWindowTitle( tr( "eXaro v%1 unsaved report" ).arg( EXARO_VERSION ) );
	newPage();
}

void mainWindow::connectItem( QObject * obj )
{
	connect( obj, SIGNAL( itemSelected( QObject *, QPointF, Qt::KeyboardModifiers ) ), this, SLOT( itemSelected( QObject *, QPointF, Qt::KeyboardModifiers ) ) );
	connect( obj, SIGNAL( geometryChanged( QObject*, QRectF, QRectF ) ), this, SLOT( itemGeometryChanged( QObject*, QRectF, QRectF ) ) );
	foreach( QObject * child, obj->children() )
		connectItem( child );
}

void mainWindow::openReport( const QString & report, bool notAsk)
{
	if ( !notAsk && !askToSaveReport() )
		return;

	QSettings s;
	QStringList list = s.value( "Designer/lastReports" ).toString().split( ";;", QString::SkipEmptyParts );
	if ( list.contains( report ) )
	    list.removeAll( report );
	list.prepend( report );
	if ( list.count() > ROWS_IN_MENU )
		list.removeLast();

	QFileInfo f( report );
	s.setValue( "Designer/reportDir", f.absolutePath() );
	s.setValue( "Designer/lastReports", list.join( ";;" ) );

	QFile file( report );
	if ( file.open( QIODevice::ReadOnly ) )
	{
		while ( m_tw->count() > STATIC_TABS )
		{
			m_tw->setCurrentIndex( STATIC_TABS );
			removePage();
		}
		delete m_report;
		m_report = dynamic_cast<Report::ReportInterface*>( m_reportEngine.loadReport( &file ) );
		refreshReportBeholders(m_report);
		file.close();
	}

	if ( !m_report )
	{
		newReport(true);
		return;
	}

	if ((int)m_report->version() < (int)FILE_FORMAT_VERSION )
	{
	    QMessageBox::critical(this,"eXaro", tr("File has too old format version and can't be opened"), QMessageBox::Ok);
	    newReport(true);
	    return;
	}

	PageView * pageView = 0;

	for ( int p = 0; p < m_report->children().size();p++ )
	{
		if ( !dynamic_cast<QGraphicsScene*>( m_report->children()[p] ) )
			continue;
		dynamic_cast<Report::PageInterface*>( m_report->children()[p] )->setContextMenu( &m_contextMenu );
		pageView = new PageView ( dynamic_cast<QGraphicsScene*>( m_report->children()[p] ) , this);
		int lastTab = m_tw->addTab(( QWidget* ) pageView, dynamic_cast<Report::PageInterface*>( pageView->scene() )->objectName() );
		dynamic_cast<QGraphicsScene*>( m_report->children()[p] )->update();
		connect( m_report->children()[p], SIGNAL( itemSelected( QObject *, QPointF, Qt::KeyboardModifiers ) ), this, SLOT( itemSelected( QObject *, QPointF, Qt::KeyboardModifiers ) ) );
//	        connect(m_report->children()[p], SIGNAL(itemMoved(QObject*, QPointF)), this, SLOT (itemMoved(QObject*, QPointF)) );
		foreach (QObject * obj, m_report->children()[p]->children())
			connectItem( obj );
		setMagnetActions( dynamic_cast<Report::PageInterface*>( pageView->scene() ) );
		pageView->view()->centerOn( 0, 0 );
		m_tw->setCurrentIndex(lastTab);
	}

	actionRemove_page->setEnabled( m_tw->count() > STATIC_TABS + 1);
	m_tw->setCurrentIndex( STATIC_TABS );

	m_saveFile = report;
	undoStack->clear();
	setWindowTitle( tr( "eXaro v%1 (%2)" ).arg( EXARO_VERSION ).arg(report) );
}

void mainWindow::openReport( bool notAsk)
{
	QSettings s;
	QString reportDir = s.value( "Designer/reportDir" ).toString();
	if ( reportDir.isEmpty() )
		reportDir = QDir::homePath();

	QString fileName = QFileDialog::getOpenFileName( this, tr( "Open report" ),
	                reportDir , tr( "Report (*.bdrt)" ) );
	if ( !fileName.size() )
		return;
	openReport( fileName , notAsk);
}

void mainWindow::openTemplate()
{
	QSettings s;
	QString templateDir = s.value( "Designer/templeteDir" ).toString();
	if ( templateDir.isEmpty() )
		templateDir = QDir::homePath();

	QString report = QFileDialog::getOpenFileName( this, tr( "Open template" ),
	                templateDir , tr( "Exaro Template (*.extt)" ) );

	if ( report.isEmpty() )
		return;

	openTemplate( report );
}

void mainWindow::prepareLastReportMenu()
{
	menuOpen_last_report->clear();
	if ( m_smReport )
		delete m_smReport;
	m_smReport = new QSignalMapper( this );

	QSettings s;
	QStringList list = s.value( "Designer/lastReports" ).toString().split( ";;", QString::SkipEmptyParts );

	for ( int i = 0; i < list.count(); ++i )
	{
		QAction *action = new QAction( list.at( i ), this );
		action->setData( list.at( i ) );
		menuOpen_last_report->addAction( action );
		connect( action, SIGNAL( triggered() ), m_smReport , SLOT( map() ) );
		m_smReport ->setMapping( action, list.at( i ) );
	}

	connect( m_smReport, SIGNAL( mapped( const QString & ) ), this, SLOT( openReport( const QString & ) ) );
}

void mainWindow::prepareLastTemplateMenu()
{
	menuOpen_last_temlate->clear();
	if ( m_smTemplate )
		delete m_smTemplate;
	m_smTemplate = new QSignalMapper( this );

	QSettings s;
	QStringList list = s.value( "Designer/lastTemplates" ).toString().split( ";;", QString::SkipEmptyParts );

	for ( int i = 0; i < list.count(); ++i )
	{
		QAction *action = new QAction( list.at( i ), this );
		action->setData( list.at( i ) );
		menuOpen_last_temlate->addAction( action );
		connect( action, SIGNAL( triggered() ), m_smTemplate, SLOT( map() ) );
		m_smTemplate->setMapping( action, list.at( i ) );
	}

	connect( m_smTemplate, SIGNAL( mapped( const QString & ) ), SLOT( openTemplate( const QString & ) ) );

}

void mainWindow::openTemplate( const QString & fileName )
{
	if ( !askToSaveReport() )
		return;

	QSettings s;
	QStringList list = s.value( "Designer/lastTemplates" ).toString().split( ";;", QString::SkipEmptyParts );
	if ( !list.contains( fileName ) )
		list.append( fileName );
	if ( list.count() > ROWS_IN_MENU )
		list.removeFirst();
	QFileInfo f( fileName );
	s.setValue( "Designer/templeteDir", f.absolutePath() );
	s.setValue( "Designer/lastTemplates", list.join( ";;" ) );
	openReport( fileName );
	m_saveFile = "";
}

void mainWindow::options()
{
	OptionsDialog d( this );
	d.exec();
	restoreSettings();
}


void mainWindow::saveReport()
{
	if ( !m_saveFile.length() )
		m_saveFile = QFileDialog::getSaveFileName( this, tr( "Save report" ),
		                QDir::homePath() + "/newReport.bdrt", tr( "Report (*.bdrt);;Template (*.extt)" ) );

	if ( !m_saveFile.length() )
		return;

	m_wdataset ->sync();
	m_report->setUis( m_dui->uis() );

	if ( !m_reportEngine.saveReport( m_saveFile, m_report ) )
		throw( QString( "Can't save the report" ) );
	setWindowTitle( tr( "eXaro v%1 (%2)" ).arg( EXARO_VERSION ).arg(m_saveFile) );
}

void mainWindow::saveReportAs()
{
	m_saveFile = "";
	saveReport();
}

void mainWindow::zoomIn()
{
	if ( dynamic_cast<PageView*>( m_tw->currentWidget() ) )
	    dynamic_cast<PageView*>( m_tw->currentWidget() )->setZoom( dynamic_cast<PageView*>( m_tw->currentWidget() )->zoom() * 1.1);
}

void mainWindow::zoomOut()
{
	if ( dynamic_cast<PageView*>( m_tw->currentWidget() ) )
		dynamic_cast<PageView*>( m_tw->currentWidget() )->setZoom( dynamic_cast<PageView*>( m_tw->currentWidget() )->zoom() * 0.9);
}

/*
void mainWindow::zoomWYSIWYG()
{
	if ( dynamic_cast<PageView*>( m_tw->currentWidget() ) )
	{
		//dynamic_cast<PageView*>( m_tw->currentWidget() )->view()->scale(( double )QDesktopWidget().screen()->width() / ( screen_widthMM*10 ), ( double )QDesktopWidget().screen()->height() / ( screen_heightMM*10 ) );
//	    dynamic_cast<PageView*>( m_tw->currentWidget() )->setZoom(( double )QDesktopWidget().screen()->width() / ( screen_widthMM*10 ) );
		dynamic_cast<PageView*>( m_tw->currentWidget() )->view()->centerOn( 0, 0 );
	}
}
*/

void mainWindow::zoomOriginal()
{
	if ( dynamic_cast<PageView*>( m_tw->currentWidget() ) )
	{
	    //		dynamic_cast<PageView*>( m_tw->currentWidget() )->view()->resetMatrix();
	    dynamic_cast<PageView*>( m_tw->currentWidget() )->setZoom(1.0);
	    dynamic_cast<PageView*>( m_tw->currentWidget() )->view()->centerOn( 0, 0 );
	}
}

void mainWindow::keyReleaseEvent( QKeyEvent * event )
{
	QListWidget * lw = dynamic_cast<QListWidget*>( m_tb->currentWidget() );

	if ( lw )
		lw-> setCurrentRow( -1 );

	QMainWindow::keyReleaseEvent( event );
}

void mainWindow::newPage()
{
	if ( !m_reportEngine.pages().count() )
		return;

	QUndoCommand *newPageCommand = new NewPageCommand( this );
	undoStack->push( newPageCommand );
}

void mainWindow::selectLastObject()
{
	itemSelected(m_lastSelectedObject ,QPointF(0,0) , Qt::NoModifier);
}

void mainWindow::itemSelected( QObject *object, QPointF pos, Qt::KeyboardModifiers  key )
{
    qDebug("mainwindow::item selected");
	m_lastSelectedObject = object;
	m_lastSelectedObjectPos = pos;
	QListWidget* lw = dynamic_cast<QListWidget*>( m_tb->currentWidget() );

	if ( lw && lw->currentRow() > -1 )
	{
		const char* needClassName = ( m_reportEngine.items().values( m_reportEngine.items().uniqueKeys()[m_tb->currentIndex()] )[lw->currentRow()] )->metaObject()->className();
		QPointF absPos = dynamic_cast<Report::PageInterface*>( object ) ? pos : dynamic_cast<Report::ItemInterface*>( object )->mapToScene( pos );
		Report::PageInterface* page = dynamic_cast<Report::PageInterface*>( object ) ? dynamic_cast<Report::PageInterface*>( object )
		                : dynamic_cast<Report::PageInterface*>( dynamic_cast<Report::ItemInterface*>( object )->scene() );

		QUndoCommand *addCommand = new AddCommand( page, needClassName, absPos, this );
		undoStack->push( addCommand );
		lw->setCurrentRow( -1 );
	}
	else
	{
		m_pe->setObject( object );
		selectObject( object, m_objectModel.index( 0, 0 ) );

		Report::ItemInterface* item = dynamic_cast<Report::ItemInterface*>( object );
		PageView* page = dynamic_cast<PageView*>( m_tw->widget( m_tw->currentIndex() ) );

		if (item && page)
		    page->selecter()->itemSelected( item, key );
	}
}

void mainWindow::removePage()
{
    if (m_tw->currentIndex() < STATIC_TABS)
	return;
    QUndoCommand *removePageCommand = new RemovePageCommand( this, m_tw->currentIndex() );
    undoStack->push( removePageCommand );
}

void mainWindow::currentChanged( int index )
{
    if ( index <  STATIC_TABS)
    {
	m_pe->setObject( 0 );
	actionRemove_page->setEnabled( false);
	return;
    }
    actionRemove_page->setEnabled( m_tw->count() > STATIC_TABS + 1);
    if (dynamic_cast<PageView*>( m_tw->widget( index )))
	m_pe->setObject( dynamic_cast<Report::PageInterface*>( dynamic_cast<PageView*>( m_tw->widget( index ) )->scene() ) );
}

void mainWindow::itemMoved( QObject *movedItem, QPointF movedFromPosition )
{
	QUndoCommand *moveCommand = new MoveCommand( dynamic_cast<Report::ItemInterface*>( movedItem ), movedFromPosition, this );
	undoStack->push( moveCommand );
}

void mainWindow::propertyChanged( QObject * obj, const QString & propertyName, const QVariant & old_value, const QVariant & new_value )
{
	QUndoCommand *propertyChangeCommand = new PropertyChangeCommand( obj, propertyName, old_value, new_value, this );
	undoStack->push( propertyChangeCommand );
}

void mainWindow::itemGeometryChanged( QObject* object, QRectF newGeometry, QRectF oldGeometry )
{
	QUndoCommand *geometryChangeCommand = new GeometryChangeCommand( object, newGeometry, oldGeometry, this );
	undoStack->push( geometryChangeCommand );
}


int mainWindow::_createNewPage_(Report::PageInterface* page,int afterIndex, QString pageName )
{
	PageView * pageView = 0;

	if ( 1 == m_reportEngine.pages().count() ) {
	    if (!page)
		pageView = new PageView( static_cast<QGraphicsScene*>( m_reportEngine.pages()[0]->createInstance( m_report ) ) , this );
	    else
		pageView = new PageView( static_cast<QGraphicsScene*>(page), this );
	    }
	else
	{
#ifndef WIN32
#warning implement me !!!!
#endif
		//ehe tata ici e de lucru
	}

	/*
			gw->setAutoFillBackground(true);
			gw->setBackgroundRole(QPalette::Base);
			QPalette pal = gw->palette();
			pal.setBrush(QPalette::Base, Qt::blue);
			pal.setColor(QPalette::HighlightedText, Qt::red);
			gw->setPalette(pal);
			gw->scene()->setBackgroundBrush(QPixmap(":/images/background.png"));
	*/
	QString nameOfPage = pageName.isNull() ? Report::ReportEngine::uniqueName( dynamic_cast<Report::PageInterface*>( pageView->scene() )->metaObject()->className() , m_report ) : pageName;

	dynamic_cast<Report::PageInterface*>( pageView->scene() )->setObjectName( nameOfPage );
	dynamic_cast<Report::PageInterface*>( pageView->scene() )->setContextMenu( &m_contextMenu );

	int m_index = m_tw->insertTab( afterIndex, ( QWidget* ) pageView, dynamic_cast<Report::PageInterface*>( pageView->scene() )->objectName() );
	m_tw->setCurrentWidget(( QWidget* ) pageView );

	actionRemove_page->setEnabled( m_tw->count() > STATIC_TABS + 1);

	connect( dynamic_cast<Report::PageInterface*>( pageView->scene() ), SIGNAL( itemSelected( QObject *, QPointF, Qt::KeyboardModifiers ) ), this, SLOT( itemSelected( QObject *, QPointF, Qt::KeyboardModifiers ) ) );
//	connect( dynamic_cast<Report::PageInterface*>( pageView->scene() ), SIGNAL( itemMoved( QObject*, QPointF ) ), this, SLOT( itemMoved( QObject*, QPointF ) ) );

	setMagnetActions( dynamic_cast<Report::PageInterface*>( pageView->scene() ) );

	if ( (STATIC_TABS + 1) == m_tw->count() )
	    m_pe->setObject( dynamic_cast<Report::PageInterface*>( pageView->scene() ) );

	m_objectModel.setRootObject( m_report );

	return m_index;
}


void mainWindow::_deletePage_( int index )
{
    if (index < STATIC_TABS)
	return;

    if ( dynamic_cast<PageView*>( m_tw->widget( index ) ) )
	delete dynamic_cast<PageView*>( m_tw->widget( index ) )->scene();
    m_tw->removeTab( index );
    actionRemove_page->setEnabled( m_tw->count() > STATIC_TABS + 1);
    
    if ( m_tw->count() >= STATIC_TABS )
	m_pe->setObject( m_report );
    m_objectModel.setRootObject( m_report );
}

void mainWindow::on_actionBandUp_triggered()
{
    if (dynamic_cast<Report::BandInterface*>(m_lastSelectedObject))
	dynamic_cast<Report::BandInterface*>(m_lastSelectedObject)->setOrder(dynamic_cast<Report::BandInterface*>(m_lastSelectedObject)->order() - 1);
}

void mainWindow::on_actionBandDown_triggered()
{
   if (dynamic_cast<Report::BandInterface*>(m_lastSelectedObject))
	dynamic_cast<Report::BandInterface*>(m_lastSelectedObject)->setOrder(dynamic_cast<Report::BandInterface*>(m_lastSelectedObject)->order() + 1);
}

void mainWindow::on_actionLastConnect_triggered()
{
    QSettings s;
    QMap<QString, QVariant> connList = s.value("Designer/connections").toMap();

    QMap <QString, QVariant> map = connList[ s.value("Designer/lastConnect").toString()].toMap();
    QSqlDatabase db = QSqlDatabase::addDatabase(map.value("driver").toString());
    db.setDatabaseName(map.value("databaseName").toString());
    db.setHostName(map.value("host").toString());
    db.setPort(map.value("port").toInt());
    db.setUserName(map.value("user").toString());
    db.setPassword(map.value("password").toString());
    if (!db.open())
	QMessageBox::critical(this, tr("Connection error"), db.lastError().text(), QMessageBox::Ok);
    m_wdataset ->resetConnection();
}

void mainWindow::refreshReportBeholders(Report::ReportInterface* report)
{
    	m_wdataset ->setReport( report );
	m_dui->setUis( report->uis() );
	m_pe->setObject( report );
	m_nameValidator->setRootObject( report );
	m_objectModel.setRootObject( report );
}
