#pragma once

enum Stage_t {
	FRAME_UNDEFINED = -1,
	FRAME_START,
	FRAME_NET_UPDATE_START,
	FRAME_NET_UPDATE_POSTDATAUPDATE_START,
	FRAME_NET_UPDATE_POSTDATAUPDATE_END,
	FRAME_NET_UPDATE_END,
	FRAME_RENDER_START,
	FRAME_RENDER_END
};

enum Hitgroups_t {
	HITGROUP_GENERIC  = 0,
	HITGROUP_HEAD     = 1,
	HITGROUP_CHEST    = 2,
	HITGROUP_STOMACH  = 3,
	HITGROUP_LEFTARM  = 4,
	HITGROUP_RIGHTARM = 5,
	HITGROUP_LEFTLEG  = 6,
	HITGROUP_RIGHTLEG = 7,
	HITGROUP_NECK     = 8,
	HITGROUP_GEAR     = 10
};

class CHLClient {
public:
	enum indices : size_t {
		INIT                = 0,
		LEVELINITPREENTITY  = 5,
		LEVELINITPOSTENTITY = 6,
		LEVELSHUTDOWN		= 7,
		GETALLCLASSES       = 8,
		HUDPROCESSINPUT     = 10,
		INACTIVATEMOUSE     = 15,
		INKEYEVENT          = 20,
		CREATEMOVE          = 21, // 21
		RENDERVIEW          = 27,
		FRAMESTAGENOTIFY    = 36, // 36
		DISPATCHUSERMESSAGE = 37,
	};

public:
	__forceinline ClientClass* GetAllClasses( ) {
		return util::get_method< ClientClass*( __thiscall* )( decltype( this ) )>( this, GETALLCLASSES )( this );
	}
};


#define Assert( _exp ) ((void)0)
#define OFFSETRS(funcname, type, offset) type& funcname( int index ) \
{ \
	static uint16_t _offset = offset; \
	Assert(_offset); \
	return *reinterpret_cast<type*>(uintptr_t(this) + _offset + index * 4); \
}

#define OFFSET(funcname, type, offset) type& funcname() \
{ \
	static uint16_t _offset = offset; \
	Assert(_offset); \
	return *reinterpret_cast<type*>(uintptr_t(this) + _offset); \
}

class PlayerResource
{
public:
	OFFSETRS(get_ping, int, 0x0AE4)
	OFFSET(get_c4_player, int, 0x161C)
};

#undef OFFSET
#undef OFFSETRS
#undef Assert
#undef CHECK_VALID