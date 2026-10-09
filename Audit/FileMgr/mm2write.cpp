
#include "stdafx.h"
#include "cmn_resource.h"
#include "3x4Matrix.h"
#include "GeoPoint.h"
#include "GeoLine.h"
#include "GeoArc.h"
#include "DbAllEntities.h"
#include "Model.h"
#include "Mm2.h"



////////////////////////////////////////////////////////////////////////

// ==================================================================

CReturn
CMM2::Write( CBinaryFile& file, const CVarList& varlist )
{
	int		num;
	int		idx;
	eVarType	type;
	int		len;
	CVar*		var;
	CString	str;

	num = varlist.countVar();
	file.Write( (BYTE*) &num, sizeof(num) );

	for (idx=0; idx<num; idx++)
	{
		var = varlist.getVar( idx );

		type = var->getType();
		file.Write( (BYTE*) &type, sizeof(type) );
		
		str = var->getName();
		len = str.GetLength()+1;
		file.Write( (BYTE*) &len, sizeof(len) );
		file.Write( (BYTE*) (LPCTSTR)str, len );

		switch (type)
		{
			case VAR_INT:
				{
					int	ival;
					ival = var->getInt();
					file.Write( (BYTE*)&ival, sizeof(ival) );
				}
				break;
			case VAR_REAL:
				{
					double	rval;
					rval = var->getReal();
					file.Write( (BYTE*)&rval, sizeof(rval) );
				}
				break;
			case VAR_STRING:
				{
					str = var->getString();
					len = str.GetLength()+1;
					file.Write( (BYTE*) &len, sizeof(len) );
					file.Write( (BYTE*) (LPCTSTR)str, len );
				}
				break;
		}
	}

	// TODO:  real error checking
	return CReturn( STATUS_OKAY );
}


CReturn
CMM2::Write( CBinaryFile& file, const CDbEntity& dbEntity )
{
	CReturn status = WriteCommon( file, dbEntity );

	if ( !status.IsOk() )
		return status;


	switch ( dbEntity.Type() )
	{
	case DBWORKPLANE:
		status = WriteWorkplane( file, dbEntity );
		break;

	case DBTOOL:
		status = WriteTool( file, dbEntity );
		break;

	case DBPOINT:
		status = WritePoint( file, dbEntity );
		break;

	case DBLINE:
		status = WriteLine( file, dbEntity );
		break;

	case DBARC:
		status = WriteArc( file, dbEntity );
		break;

	case DBHOLE:
		status = WriteHole( file, dbEntity );
		break;

	case DBPROFILE:
		status = WriteProfile( file, dbEntity );
		break;

	case DBCOMMAND:
		status = WriteCommand( file, dbEntity );
		break;

	case DBFEATURE:
		status = WriteFeature( file, dbEntity );
		break;

	case DBSEQUENCE:
		status = WriteSequence( file, dbEntity );
		break;

	case DBPATTERN:
		status = WritePattern( file, dbEntity );
		break;
	}

	return status;
}

CReturn
CMM2::WriteCommon( CBinaryFile& file, const CDbEntity& dbEntity )
{
	CReturn status;

	EDbEntityType type;
	FLAGS flags;
	ID id;
	CString name;
	int len;

	type = dbEntity.Type();

	id = dbEntity.Id();
	file.Write( (BYTE*) &id, sizeof(id) );

	// NOTE: Derived names start with the '_' character.
	// NOTE:  SystemName never returns derived names...
	name = dbEntity.SystemName();
//	len = ((name[0] == '_') ? 0 : name.GetLength()+1);

	len = name.GetLength();
	if (len) len++;

	file.Write( (BYTE*) &len, sizeof(len) );
	if (len > 1)
		file.Write( (BYTE*) (LPCTSTR)name, len );

	flags = dbEntity.Flags();
	// Mask out the selection and hidden bits, otherwise bad 
	// things happen.
	flags &= ~(DBHIDDEN|DBSELECTED);

/*	if ( dbEntity.IsSelected() )
	{
		// Mask out the selection bit.  Otherwise, the entity will
		// appear highlighted when it is read back into the model.
		flags = (flags & ~DBSELECTED);
	}
*/
	file.Write( (BYTE*) &flags, sizeof(flags) );

	id = dbEntity.WorkplaneId();
	file.Write( (BYTE*) &id, sizeof(id) );

	id = dbEntity.ToolId();
	file.Write( (BYTE*) &id, sizeof(id) );

	Write( file, dbEntity.Attrib() );

	return status;
}

CReturn
CMM2::WriteLayer( CBinaryFile& file, const CDbEntity& dbEntity )
{
	CReturn status;
	// A layer doesn't yet contain any data.
	return status;
}

CReturn
CMM2::WriteWorkplane( CBinaryFile& file, const CDbEntity& dbEntity )
{
	CReturn status;

	const CDbWorkplane& dbWorkplane = dynamic_cast<const CDbWorkplane&>( dbEntity );

	if (&dbWorkplane == NULL)
	{
		status.Internal( IDS_FILE_WRITE_ERR, "CMM2::WriteWorkplane" );
	}

	const C3x4Matrix& xform = dbWorkplane.Transform();

	const C3dVec& ivec = xform.getI();
	const C3dVec& jvec = xform.getJ();
	const C3dVec& kvec = xform.getK();
	const C3dCoord& origin = xform.getT();

	WriteTriple( file, ivec.X(), ivec.Y(), ivec.Z() );
	WriteTriple( file, jvec.X(), jvec.Y(), jvec.Z() );
	WriteTriple( file, kvec.X(), kvec.Y(), kvec.Z() );
	WriteTriple( file, origin.X(), origin.Y(), origin.Z() );

	int	up = dbWorkplane.ToolUp();
	file.Write( (BYTE*) &up, sizeof(up) );

	return status;
}

CReturn
CMM2::WriteTool( CBinaryFile& file, const CDbEntity& dbEntity )
{
	CReturn status;
	// Nothing to do.  At first implementation, all tooling
	// information is stored as attributes on the tool.
	return status;
}

CReturn
CMM2::WritePoint( CBinaryFile& file, const CDbEntity& dbEntity )
{
	CReturn status;

	const CDbPoint& dbPoint = dynamic_cast<const CDbPoint&>( dbEntity );

	if (&dbPoint == NULL)
	{
		status.Internal( IDS_FILE_WRITE_ERR, "CMM2::WritePoint" );
	}

	if ( status.IsOk() )
	{
		C3dCoord pt = dbPoint.Coord();
		WriteCoord( file, pt );
	}

	return status;
}

CReturn
CMM2::WriteLine( CBinaryFile& file, const CDbEntity& dbEntity )
{
	CDbEntityList dbEntities;
	dbEntity.RefsTo( &dbEntities );

	return WriteEntityList( file, dbEntities );
}

CReturn
CMM2::WriteArc( CBinaryFile& file, const CDbEntity& dbEntity )
{
	CDbEntityList dbEntities;
	dbEntity.RefsTo( &dbEntities );

	CReturn status = WriteEntityList( file, dbEntities );

	const CDbArc& dbArc = dynamic_cast<const CDbArc&>( dbEntity );

	if (&dbArc == NULL)
	{
		status.Internal( IDS_FILE_WRITE_ERR, "CMM2::WriteArc" );
	}

	if ( status.IsOk() )
	{
		int dir = dbArc.Dir();

		file.Write( (BYTE*) &dir, sizeof(dir) );
	}

	return status;
}

CReturn
CMM2::WriteHole( CBinaryFile& file, const CDbEntity& dbEntity )
{
	CReturn status;

	const CDbHole& dbHole = dynamic_cast<const CDbHole&>( dbEntity );

	if (&dbHole == NULL)
	{
		status.Internal( IDS_FILE_WRITE_ERR, "CMM2::WriteHole" );
	}

	if ( status.IsOk() )
	{
		double diam = dbHole.Diam();
		double depth = dbHole.Depth();

		file.Write( (BYTE*) &diam, sizeof(diam) );
		file.Write( (BYTE*) &depth, sizeof(depth) );

		status = WriteCoord( file, dbHole.Center() );
	}

	return status;
}

CReturn
CMM2::WriteProfile( CBinaryFile& file, const CDbEntity& dbEntity )
{
	const CDbProfile& dbProfile = dynamic_cast<const CDbProfile&>( dbEntity );

	int isAssociated = (dbProfile.IsAssociated() ? 1 : 0);
	file.Write( (BYTE*) &isAssociated, sizeof(isAssociated) );

	CDbEntityList dbEntities;
	dbEntity.RefsTo( &dbEntities );

	return WriteEntityList( file, dbEntities );
}

CReturn
CMM2::WriteFeature( CBinaryFile& file, const CDbEntity& dbEntity )
{
	CReturn status;

	CDbEntityList dbEntities;
	dbEntity.RefsTo( &dbEntities );
	status = WriteEntityList( file, dbEntities );

	dbEntities.BenignFlush();
	dbEntity.Subordinates( &dbEntities );
	status = WriteEntityList( file, dbEntities );

	return status;
}

CReturn
CMM2::WritePattern( CBinaryFile& file, const CDbEntity& dbEntity )
{
	CReturn			status;
	CDbEntityList	dbEntities;

	dbEntities.BenignFlush();
	dbEntity.Subordinates( &dbEntities );
	status = WriteEntityList( file, dbEntities );

	return status;
}

CReturn
CMM2::WriteSequence( CBinaryFile& file, const CDbEntity& dbEntity )
{
	CReturn status;

	CDbEntityList dbEntities;
	dbEntity.Subordinates( &dbEntities );
	status = WriteEntityList( file, dbEntities );

	return status;
}

CReturn
CMM2::WriteCommand( CBinaryFile& file, const CDbEntity& dbEntity )
{
	CReturn status;

	const CDbCommand& dbCommand = dynamic_cast<const CDbCommand&>( dbEntity );

	if (&dbCommand == NULL)
	{
		status.Internal( IDS_FILE_WRITE_ERR, "CMM2::WriteCommand" );
	}

	if ( status.IsOk() )
	{
		C3dCoord pt = dbCommand.Coord();
		WriteCoord( file, pt );

		CString text = dbCommand.Text();
		int len = text.GetLength();
		if (len) len++;

		file.Write( (BYTE*) &len, sizeof(len) );
		if (len > 1)
			file.Write( (BYTE*) (LPCTSTR)text, len );
	}

	return status;
}

CReturn
CMM2::WriteCoord( CBinaryFile& file, const C3dCoord& pt )
{
	return WriteTriple( file, pt.X(), pt.Y(), pt.Z() );
}

CReturn
CMM2::WriteEntityList( CBinaryFile& file, const CDbEntityList& dbEntities )
{
	CReturn status;

	int count = dbEntities.Count();
	file.Write( (BYTE*) &count, sizeof(count) );

	for (int indx = 0; indx < count; ++indx)
	{
		ID id = dbEntities[indx]->Id();
		file.Write( (BYTE*) &id, sizeof(id) );
	}

	return status;
}

CReturn
CMM2::WriteTriple( CBinaryFile& file, double x, double y, double z )
{
	CReturn status;

	file.Write( (BYTE*) &x, sizeof(x) );
	file.Write( (BYTE*) &y, sizeof(y) );
	file.Write( (BYTE*) &z, sizeof(z) );

	return status;
}
