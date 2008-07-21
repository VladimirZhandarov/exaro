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
#include <QSqlRecord>

#include "sqlquery.h"
namespace Report
{
SqlQuery::SqlQuery(QObject *parent, QSqlDatabase db)
		: QObject(parent), QSqlQuery(db)
{
}


SqlQuery::~SqlQuery()
{
}


bool SqlQuery::exec()
{
	return QSqlQuery::exec();
}

bool SqlQuery::exec(const QString & query)
{
	return QSqlQuery::exec(query);
}

void SqlQuery::bindValue(const QString & placeholder, const QVariant & val, QSql::ParamType paramType)
{
	QSqlQuery::bindValue(placeholder, val, paramType);
}

void SqlQuery::bindValue(int pos, const QVariant & val, QSql::ParamType paramType)
{
	QSqlQuery::bindValue(pos, val, paramType);
}

bool SqlQuery::first()
{
	emit(beforeFirst());
	bool ret=QSqlQuery::first();
	emit(afterFirst());
	return ret;
}

bool SqlQuery::last()
{
	emit(beforeLast());
	bool ret=QSqlQuery::last();
	emit(afterLast());
	return ret;
}

bool SqlQuery::next()
{
	emit(beforeNext());
	bool ret=QSqlQuery::next();
	emit(afterNext());
	return ret;
}

bool SqlQuery::previous()
{
	emit(beforePrevious());
	bool ret=QSqlQuery::previous();
	emit(afterPrevious());
	return ret;
}

bool SqlQuery::prepare(const QString & query)
{
	return QSqlQuery::prepare(query);
}

bool SqlQuery::seek(int index, bool relative)
{
	emit(beforeSeek(index));
	bool ret=QSqlQuery::seek(index, relative);
	emit(afterSeek(index));
	return ret;
}

int SqlQuery::size()
{
	return QSqlQuery::size();
}

QVariant SqlQuery::value(int index) const
{
	return QSqlQuery::value(index);
}

QVariant SqlQuery::value(const QString & field) const
{
	return record().value(field);
}
}