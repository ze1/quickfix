/* -*- C++ -*- */

/****************************************************************************
** Copyright (c) 2001-2005 quickfixengine.org  All rights reserved.
**
** This file is part of the QuickFIX FIX Engine
**
** This file may be distributed under the terms of the quickfixengine.org
** license as defined by quickfixengine.org and appearing in the file
** LICENSE included in the packaging of this file.
**
** This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
** WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
**
** See http://www.quickfixengine.org/LICENSE for licensing information.
**
** Contact ask@quickfixengine.org if any conditions of this licensing are
** not clear to you.
**
****************************************************************************/

#ifndef HAVE_MYSQL
#error MySQLConnection.h included, but HAVE_MYSQL not defined
#endif

#ifdef HAVE_MYSQL
#ifndef FIX_MYSQLCONNECTION_H
#define FIX_MYSQLCONNECTION_H

#ifdef _MSC_VER
#pragma warning( disable : 4503 4355 4786 4290 )
#pragma comment( lib, "libMySQL" )
#endif

#include <mysql.h>
#include <errmsg.h>

#undef MYSQL_PORT

namespace FIX
{
  class MySQLQuery
  {
  public:
    MySQLQuery( const std::string& query ) 
    : m_result( 0 ), m_query( query ) 
    {}

    ~MySQLQuery()
    {
      if( m_result )
        mysql_free_result( m_result );
    }

    bool execute( MYSQL* pConnection )
    {
      int retry = 0;
      
      do
      {
        if( m_result ) mysql_free_result( m_result );
        int errcode = mysql_query( pConnection, m_query.c_str() );
        m_result = mysql_store_result( pConnection );
        if( errcode == 0 )
          return true;
        m_status = mysql_errno( pConnection );
        m_reason = mysql_error( pConnection );
        if( success() ) return true;
        mysql_ping( pConnection );
        retry++;
      } while( retry <= 1 );
      return success();
    }

    bool success()
    {
      return m_status == 0;
    }

    my_ulonglong rows()
    {
      return mysql_num_rows( m_result );
    }

    const std::string& reason()
    {
      return m_reason;
    }

    MYSQL_ROW getNextRow()
    {
      return mysql_fetch_row( m_result );
    }

  private:
    MYSQL_RES* m_result;
    int m_status;
    std::string m_query; 
    std::string m_reason;
  };

  class MySQLConnection
  {
  public:
    MySQLConnection
    ( const std::string& database, const std::string& user,
      const std::string& password, const std::string& host, short port )
    {
      m_pConnection = mysql_init( NULL );
      if( !mysql_real_connect( m_pConnection, host.c_str(), user.c_str(), password.c_str(),
                               database.c_str(), port, 0, 0 ) )
      {
          if( !connected() )
            throw ConfigError( "Unable to connect to database" );
      }
    }

    ~MySQLConnection()
    {
      if( m_pConnection )
        mysql_close( m_pConnection );
    }

    bool connected()
    {
      return mysql_ping( m_pConnection ) == 0;
    }

    bool reconnect()
    {
      return mysql_ping( m_pConnection ) == 0;
    }

    bool execute( MySQLQuery& pQuery )
    {
      return pQuery.execute( m_pConnection );
    }

  private:
    MYSQL* m_pConnection;
  };
}

#endif //FIX_MYSQLCONNECTION_H
#endif //HAVE_MYSQL