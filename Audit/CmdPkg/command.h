#if !defined(_COMMAND_H)
#define _COMMAND_H

// ==================================================================
//		Command
//
//	Self-parsing Command object.  When given a string-based
//	command, it can extract one or more routing sub-strings, 
//	plus any number of tagged parameter values.
//
//	Commands take the form:
//
//		"name1:name2:a=1,b=2.0,c='text'"
//
//	Where:	name1 and name2 are routing instructions (1 or more)
//			a is an integer attribute
//			b is a floating attribute
//			c is a string attribute
//
//	':' is used to terminate routing instructions
//	'=' assigns attributes
//	',' is the attribute separator
//
// ==================================================================

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

// ==================================================================

class CCommand;

// ==================================================================

#include "Return.h"

#include "Var.h"
#include "VarList.h"

class CModel;
// #include "Model.h"

class CViewMgr;


// ==================================================================

class CCmdParser;

class dllExport CCommand
{
	friend CCmdParser;

public:

	CCommand();
	virtual ~CCommand();

	void	Reset( void );

	CReturn	setCommand( const CString& in_cmd, CModel* in_model, CViewMgr* in_view_mgr );
	CReturn	addCommand( const CString& in_cmd, CModel* in_model, CViewMgr* in_view_mgr );

	bool		ModelExists( void ) const;

	void setModel( CModel* model );  // BE VERY CAREFUL WITH THIS ONE!
	CModel&		getModel( void ) const;

	CViewMgr&	getViewMgr( void ) const;

	CReturn	nextRoute( CString* io_route );
	CReturn	Backup( void );

	CReturn	getInt( const CString& in_name, int* io_val ) const;
	CReturn	getReal( const CString& in_name, double* io_val ) const;
	CReturn	getString( const CString& in_name, CString* io_val ) const;

	void setInt( const CString& in_name, int in_val );
	void setReal( const CString& in_name, double in_val );
	void setString( const CString& in_name, const CString& in_val );

	const CVarList&	VarList( void ) const;

	void Dump( void );

	// Made public only for use with CmdParser.
	CReturn add_route( const CString& in_route );

private:

	// Disabled.
	CCommand( const CCommand& );
	const CCommand& operator = ( const CCommand& );
	int operator == ( const CCommand& ) const;
	int operator != ( const CCommand& ) const;

private:

	int			count_route() const							{ return m_route_list.GetSize(); }
	CString		get_route( int in_idx ) const				{ return m_route_list.GetAt( in_idx ); }

	CVarList		m_varlist;
	CStringArray	m_route_list;

	CModel*			m_model;				// Reference to the relevant model
	CViewMgr*		m_view_mgr;			// Reference to the relevant view manager

	int				m_route_idx;		// Current routing index
};


inline bool CCommand::ModelExists( void ) const
	{ return (m_model != NULL); }

inline void CCommand::setModel( CModel* model )
	{ m_model = model; }

inline CModel& CCommand::getModel( void ) const
	{ return *m_model; }

inline CViewMgr& CCommand::getViewMgr( void ) const
	{ return *m_view_mgr; }

inline CReturn CCommand::getInt( const CString& in_name, int* io_val ) const
	{ return m_varlist.getInt( in_name, io_val ); }

inline CReturn CCommand::getReal( const CString& in_name, double* io_val ) const
	{ return m_varlist.getReal( in_name, io_val ); }

inline CReturn CCommand::getString( const CString& in_name, CString* io_val ) const
	{ return m_varlist.getString( in_name, io_val ); }

inline void CCommand::setInt( const CString& in_name, int in_val )
	{ m_varlist.setInt( in_name, in_val ); }

inline void CCommand::setReal( const CString& in_name, double in_val )
	{ m_varlist.setReal( in_name, in_val ); }

inline void CCommand::setString( const CString& in_name, const CString& in_val )
	{ m_varlist.setString( in_name, in_val ); }

inline const CVarList& CCommand::VarList( void ) const
	{ return m_varlist; }

#endif

