#pragma once

enum EEWMFlags
{
	EWM_ACTIVE     = 0x001,
	EWM_FILTERED   = 0x002,
	EWM_WARNING    = 0x004,
	EWM_USER       = 0x008,
	EWM_INTERNAL   = 0x010,
	EWM_DIAGNOSTIC = 0x020,
	EWM_PORTAL     = 0x040,
	EWM_UI         = 0x080,
	EWM_DAO        = 0x100,
	EWM_NESTING    = 0x200,
	EWM_SEED       = 0x400,
	EWM_FILE       = 0x800
};
