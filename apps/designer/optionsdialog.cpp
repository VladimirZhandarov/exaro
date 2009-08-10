/***************************************************************************
 *   Copyright (C) 2009 by Alexander Mikhalov                              *
 *   alexmi3@rambler.ru                                                    *
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

#include <QtGui>
#include "optionsdialog.h"

OptionsDialog::OptionsDialog( QWidget *parent ) :
		QDialog( parent )
{
	setupUi( this );
	connect( buttonBox, SIGNAL( accepted() ), SLOT( needAccept() ) );
	QTimer::singleShot( 0, this, SLOT( restoreAll() ) );
}

void OptionsDialog::on_bDesignerPath_clicked()
{
#if defined(Q_OS_WIN)
	QString fileName = QFileDialog::getOpenFileName( this, "designer.exe", ".", "designer.exe" );
#else
	QString fileName = QFileDialog::getOpenFileName( this, "designer", "/usr/bin", "designer" );
#endif

	leDesignerPath->setText( fileName );

}

void OptionsDialog::needAccept()
{
	storeAll();
	emit accept();
}

void OptionsDialog::storeAll()
{
	QSettings s;
	s.setValue( "Options/designerPath", leDesignerPath->text() );
	s.setValue( "Options/iconSize", cbIconSize->currentText() );
	s.setValue( "Items/drawSelectionBorder", drawSelection->isChecked());
	s.setValue( "Options/drawTitles", cbBandTitle->isChecked());
	s.setValue( "Options/margin", sbBandsMargin->value());
	s.setValue( "Options/grid" , cbGrid->isChecked() );
	s.setValue( "Options/gridStep", sbGridStep->value());
}

void OptionsDialog::restoreAll()
{
	QSettings s;
	leDesignerPath->setText( s.value( "Options/designerPath" ).toString() );
	cbIconSize->setCurrentIndex( cbIconSize->findText( s.value( "Options/iconSize" ).toString() ) );
	drawSelection->setChecked( s.value( "Items/drawSelectionBorder", true ).toBool());
	cbBandTitle->setChecked( s.value( "Options/drawTitles", true ).toBool());
	sbBandsMargin->setValue( s.value( "Options/margin", 0 ).toInt() );
	cbGrid->setChecked ( s.value( "Options/grid", true ).toBool());
	sbGridStep->setValue( s.value( "Options/gridStep", 1).toInt() );
}

