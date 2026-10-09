#if !defined(_REPO_H)
#define _REPO_H

// ==================================================================
//		Repo
//
// ==================================================================

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000


#include "Common.h"
#include "Model.h"
#include "GeoCurve.h"

// ==================================================================

#define LARGE_PART_ZONE 99
#define ZONE_NONE -1

// ==================================================================

class dllExport CRepo
{
public:

	CRepo( CModel* model, const CString& config_db, int autolead_id );

	virtual ~CRepo();

	int ZoneCount( void )			{ return m_zone_array.GetSize(); }
	CReturn	ExtractZone( void );

	bool HasClamps() const;
	CReturn	ExtractClamp( void );

	CReturn	EnableCommands( void );

	CReturn	PackageInstances(void);
	CReturn	LargeInstances( void );
	CReturn	ClampInstances( double clamp_buffer,
							double machine_left,
							double torch_offset,
							double sheet_right,
							bool small_repo,
							int zone,
							CDbFeature** db_czone );

	CReturn Progressive( double burn_offset, double step, double length, double width );

	bool MustRebuild()		{ return m_must_rebuild; }

	CReturn	Rebuild( double clamp_buffer );
	CReturn WrapProfiles( void );

	CReturn	ShiftPierce( double torch_offset );

	CReturn Cleanup( void );

private: // Disabled.

	CRepo();
	CRepo( const CRepo& );
	const CRepo& operator = ( const CRepo& );
	int operator == ( const CRepo& ) const;
	int operator != ( const CRepo& ) const;

private:

	CDbFeature* explode_instance( CDbCommand* db_cmd );
	CReturn		explode_profiles( CDbFeature* db_feat,
								CDbFeature* skip_feat,
								C2dBox*		skipbox,
								C2dBox*		clampbox );
	CReturn split_profiles(	
		CDbFeature*	db_feat, 
		CGeoCurveArray& split_array,
		C2dBox*			inhibit,
		bool*			did_split );

	CReturn		rebuild_profiles(
								CDbFeature*	src_feature,
								CDbFeature*	dst_feature,
								C3dBox&		sel_box );

	CReturn		clean_empty( void );
	CReturn		compress_zones( void );

	bool		is_feature_empty(CDbFeature* db_feat);

	CReturn RebuildClampedPart( CDbFeature* dbFeature, double clamp_buffer );
	CReturn RebuildZonedPart( CDbFeature* dbFeature );

	CReturn		clamped_instances( 
							CDbEntityList*	dst_list, 
							C2dBox&			clamp,
							C2dBox&			column,
							int				zidx );

	void	deselect_part_holes( CSelector* select );

	void	strip_leads( CDbFeature* db_feat );

	bool PostRepoInterference( const C2dBox& part_box );

	bool IsSplittable( const CDbCurve* dbCurve );
	bool IsRepoPart( const CDbFeature* dbFeature );

	void SubpiecesDistribute( CDbFeature* dbLargePart );
	int WhichZone( const C2dBox& box );

private:

	CModel*			m_model;
	int				m_autoleadid;
	CString			m_configdb;

	CArray<C2dBox, C2dBox>				m_zone_array;
	CArray<CDbFeature*,CDbFeature*>		m_zonefeat_array;

	CArray<C2dBox, C2dBox>*				m_clamp_array;	
	CArray<CDbFeature*,CDbFeature*>		m_clampfeat_array;

	double	m_clamp_repo;
	bool	m_must_rebuild;
};

#endif

