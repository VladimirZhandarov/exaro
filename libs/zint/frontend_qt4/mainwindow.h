/***************************************************************************
 *   Copyright (C) 2008 by BogDan Vatra   *
 *   taipan@licentia.eu   *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QtGui>
#include <QGraphicsItem>

#include "ui_mainWindow.h"
#include "barcodeitem.h"

class MainWindow : public QWidget, private Ui::mainWindow
{
	Q_OBJECT

	Q_ENUMS(BarcodeTypes)

public:
	enum BarcodeTypes
	{
		CODE11 = 1,
		C25MATRIX = 2,
		C25INTER = 3,
		C25IATA = 4,
		C25LOGIC = 6,
		C25IND = 7,
		CODE39 = 8,
		EXCODE39 = 9,
		EANX = 13,
		EAN128 = 16,
		CODABAR = 18,
		CODE128 = 20,
		DPLEIT = 21,
		DPIDENT = 22,
		CODE16K = 23,
		CODE93 = 25,
		FLAT = 28,
		RSS14 = 29,
		RSS_LTD = 30,
		RSS_EXP = 31,
		TELEPEN = 32,
		UPCA = 34,
		UPCE = 37,
		POSTNET = 40,
		MSI_PLESSEY = 47,
		FIM = 49,
		LOGMARS = 50,
		PHARMA = 51,
		PZN = 52,
		PHARMA_TWO = 53,
		PDF417 = 55,
		PDF417TRUNC = 56,
		MAXICODE = 57,
		QRCODE = 58,
		CODE128B = 60,
		AUSPOST = 63,
		AUSREPLY = 66,
		AUSROUTE = 67,
		AUSREDIRECT = 68,
		ISBNX = 69,
		RM4SCC = 70,
		DATAMATRIX = 71,
		ITF14 = 72,
		NVE18 = 75,
		KOREAPOST = 77,
		RSS14STACK = 79,
		RSS14STACK_OMNI = 80,
		RSS_EXPSTACK = 81,
		PLANET = 82,
		MICROPDF417 = 84,
		ONECODE = 85,
		PLESSEY = 86,
		TELEPEN_NUM = 100,
		MSI_10 = 101,
		MSI_10_10 = 102,
		MSI_11 = 103,
		MSI_11_10 = 104,
		CODE39_43 = 105,
		EXCODE39_43 = 106,
		AZTEC = 107,
		SUPER = 108,
		ULTRA = 109,
		EANX_CC = 110,
		EAN128_CC = 111,
		RSS14_CC = 112,
		RSS_LTD_CC = 113,
		RSS_EXP_CC = 114,
		UPCA_CC = 115,
		UPCE_CC = 116,
		RSS14STACK_CC = 117,
		RSS14_OMNI_CC = 118,
		RSS_EXPSTACK_CC = 119,
	};

public:
	MainWindow(QWidget* parent = 0, Qt::WFlags fl = 0);
	~MainWindow();

public slots:
	void on_generate_clicked();
	void on_fgcolor_clicked();
	void on_bgcolor_clicked();

private:
	QColor m_fgcolor,m_bgcolor;
	BarcodeItem m_bc;

};

#endif


