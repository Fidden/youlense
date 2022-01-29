#pragma once

struct mstudiobbox_t {
	int     m_bone;                 // 0x0000
	int     m_group;                // 0x0004
	vec3_t  m_mins;                 // 0x0008
	vec3_t  m_maxs;                 // 0x0014
	int     m_name_id;				// 0x0020
	ang_t   m_angle;                // 0x0024
	float   m_radius;               // 0x0030
	PAD( 0x10 );                    // 0x0034
};

struct mstudiohitboxset_t {
	int	m_name_id;
	int	m_hitboxes;
	int	m_hitbox_id;

	__forceinline mstudiobbox_t* GetHitbox( int index ) const { 
		return ( mstudiobbox_t* )( ( ( byte* )this ) + m_hitbox_id ) + index; 
	};
};

struct mstudiobone_t {
	int m_bone_name_index;
	int m_parent;
	PAD( 0x98 );
	int m_flags;
	PAD( 0x34 );
};

struct studiohdr_t {
	int    m_id;					// 0x0000
	int    m_version;				// 0x0004
	int    m_checksum;				// 0x0008
	char   m_name[ 64 ];			// 0x000C
	int    m_length;				// 0x004C
	vec3_t m_eye_pos;				// 0x0050
	vec3_t m_illum_pos;				// 0x005C
	vec3_t m_hull_mins;				// 0x0068
	vec3_t m_hull_maxs;             // 0x0074
	vec3_t m_view_mins;             // 0x0080
	vec3_t m_view_maxs;             // 0x008C
	int    m_flags;					// 0x0098
	int    m_num_bones;				// 0x009C
	int    m_bone_id;				// 0x00A0
	int    m_num_controllers;		// 0x00A4
	int    m_controller_id;			// 0x00A8
	int    m_num_sets;				// 0x00AC
	int    m_set_id;				// 0x00B0

	__forceinline mstudiobone_t* GetBone( int index ) const {
		return ( mstudiobone_t* )( ( ( byte* )this ) + m_bone_id ) + index;
	}

	__forceinline mstudiohitboxset_t* GetHitboxSet( int index ) const {
		return ( mstudiohitboxset_t* )( ( ( byte* )this ) + m_set_id ) + index;
	}

	__forceinline mstudiobbox_t* GetHitbox(int i, int set) const {
		mstudiohitboxset_t const* s = GetHitboxSet(set);
		if (!s)
			return nullptr;

		return s->GetHitbox(i);
	}
};

struct brushdata_t
{
	void* pShared;
	int				firstmodelsurface;
	int				nummodelsurfaces;

	// track the union of all lightstyles on this brush.  That way we can avoid
	// searching all faces if the lightstyle hasn't changed since the last update
	int				nLightstyleLastComputedFrame;
	unsigned short	nLightstyleIndex;	// g_ModelLoader actually holds the allocated data here
	unsigned short	nLightstyleCount;

	unsigned short	renderHandle;
	unsigned short	firstnode;
};

struct spritedata_t
{
	int				numframes;
	int				width;
	int				height;
	void* sprite;
};

typedef unsigned short MDLHandle_t;
struct studiohdr_t;
struct studiohwdata_t;
struct vcollide_t;
struct virtualmodel_t;
struct vertexFileHeader_t;

class model_t {
public:
	void*  m_handle;
	char   m_name[ 260 ];
	int    m_load_flags;
	int    m_server_count;
	int    m_type;
	int    m_flags;
	vec3_t m_mins;
	vec3_t m_maxs;
	float  m_radius;
	void*  m_key_values;
	union
	{
		brushdata_t brush;
		MDLHandle_t studio;
		spritedata_t sprite;
	};
};

class IVModelInfo {
public:
	// indexes for virtuals and hooks.
	enum indices : size_t {
        GETMODEL           = 1,
		GETMODELINDEX      = 2,
        GETMODELFRAMECOUNT = 8,
		GETSTUDIOMODEL     = 30,
        FINDORLOADMODEL    = 43
	};

public:
    __forceinline const model_t *GetModel( int modelindex ) {
        return util::get_method< const model_t *(__thiscall *)( void *, int ) >( this, GETMODEL )( this, modelindex );
    }

	__forceinline int GetModelIndex( const char* model ) {
		return util::get_method< int( __thiscall* )( void*, const char* ) >( this, GETMODELINDEX )( this, model );
	}

    __forceinline int GetModelFrameCount( const model_t *model ) {
        return util::get_method< int( __thiscall* )( void*, const model_t * ) >( this, GETMODELFRAMECOUNT )( this, model );
    }

	__forceinline studiohdr_t* GetStudioModel( const model_t *model ) {
		return util::get_method< studiohdr_t*( __thiscall* )( void*, const model_t* ) >( this, GETSTUDIOMODEL )( this, model );
	}

    __forceinline const model_t *FindOrLoadModel( const char *name ) {
        return util::get_method< const model_t *( __thiscall* )( void*, const char * ) >( this, FINDORLOADMODEL )( this, name );
    }
};


typedef unsigned short MDLHandle_t;
struct studiohdr_t;
struct studiohwdata_t;
struct vcollide_t;
struct virtualmodel_t;
struct vertexFileHeader_t;
enum MDLCacheFlush_t
{
	MDLCACHE_FLUSH_STUDIOHDR = 0x01,
	MDLCACHE_FLUSH_STUDIOHWDATA = 0x02,
	MDLCACHE_FLUSH_VCOLLIDE = 0x04,
	MDLCACHE_FLUSH_ANIMBLOCK = 0x08,
	MDLCACHE_FLUSH_VIRTUALMODEL = 0x10,
	MDLCACHE_FLUSH_AUTOPLAY = 0x20,
	MDLCACHE_FLUSH_VERTEXES = 0x40,

	MDLCACHE_FLUSH_IGNORELOCK = 0x80000000,
	MDLCACHE_FLUSH_ALL = 0xFFFFFFFF
};
enum MDLCacheDataType_t
{
	// Callbacks to get called when data is loaded or unloaded for these:
	MDLCACHE_STUDIOHDR = 0,
	MDLCACHE_STUDIOHWDATA,
	MDLCACHE_VCOLLIDE,

	// Callbacks NOT called when data is loaded or unloaded for these:
	MDLCACHE_ANIMBLOCK,
	MDLCACHE_VIRTUALMODEL,
	MDLCACHE_VERTEXES,
	MDLCACHE_DECODEDANIMBLOCK,
};

class IMDLCache
{
public:
	// Used to install callbacks for when data is loaded + unloaded
// Returns the prior notify
	virtual void SetCacheNotify(void* pNotify) = 0;

	// NOTE: This assumes the "GAME" path if you don't use
	// the UNC method of specifying files. This will also increment
	// the reference count of the MDL
	virtual MDLHandle_t FindMDL(const char* pMDLRelativePath) = 0;

	// Reference counting
	virtual int AddRef(MDLHandle_t handle) = 0;
	virtual int Release(MDLHandle_t handle) = 0;
	virtual int GetRef(MDLHandle_t handle) = 0;

	// Gets at the various data associated with a MDL
	virtual studiohdr_t* GetStudioHdr(MDLHandle_t handle) = 0;
	virtual studiohwdata_t* GetHardwareData(MDLHandle_t handle) = 0;
	virtual vcollide_t* GetVCollide(MDLHandle_t handle) = 0;
	virtual unsigned char* GetAnimBlock(MDLHandle_t handle, int nBlock) = 0;
	virtual virtualmodel_t* GetVirtualModel(MDLHandle_t handle) = 0;
	virtual int GetAutoplayList(MDLHandle_t handle, unsigned short** pOut) = 0;
	virtual vertexFileHeader_t* GetVertexData(MDLHandle_t handle) = 0;

	// Brings all data associated with an MDL into memory
	virtual void TouchAllData(MDLHandle_t handle) = 0;

	// Gets/sets user data associated with the MDL
	virtual void SetUserData(MDLHandle_t handle, void* pData) = 0;
	virtual void* GetUserData(MDLHandle_t handle) = 0;

	// Is this MDL using the error model?
	virtual bool IsErrorModel(MDLHandle_t handle) = 0;

	// Flushes the cache, force a full discard
	virtual void Flush(MDLCacheFlush_t nFlushFlags = MDLCACHE_FLUSH_ALL) = 0;

	// Flushes a particular model out of memory
	virtual void Flush(MDLHandle_t handle, int nFlushFlags = MDLCACHE_FLUSH_ALL) = 0;

	// Returns the name of the model (its relative path)
	virtual const char* GetModelName(MDLHandle_t handle) = 0;

	// faster access when you already have the studiohdr
	virtual virtualmodel_t* GetVirtualModelFast(const studiohdr_t* pStudioHdr, MDLHandle_t handle) = 0;

	// all cache entries that subsequently allocated or successfully checked 
	// are considered "locked" and will not be freed when additional memory is needed
	virtual void BeginLock() = 0;

	// reset all protected blocks to normal
	virtual void EndLock() = 0;

	// returns a pointer to a counter that is incremented every time the cache has been out of the locked state (EVIL)
	virtual int* GetFrameUnlockCounterPtrOLD() = 0;

	// Finish all pending async operations
	virtual void FinishPendingLoads() = 0;

	virtual vcollide_t* GetVCollideEx(MDLHandle_t handle, bool synchronousLoad = true) = 0;
	virtual bool GetVCollideSize(MDLHandle_t handle, int* pVCollideSize) = 0;

	virtual bool GetAsyncLoad(MDLCacheDataType_t type) = 0;
	virtual bool SetAsyncLoad(MDLCacheDataType_t type, bool bAsync) = 0;

	virtual void BeginMapLoad() = 0;
	virtual void EndMapLoad() = 0;
	virtual void MarkAsLoaded(MDLHandle_t handle) = 0;

	virtual void InitPreloadData(bool rebuild) = 0;
	virtual void ShutdownPreloadData() = 0;

	virtual bool IsDataLoaded(MDLHandle_t handle, MDLCacheDataType_t type) = 0;

	virtual int* GetFrameUnlockCounterPtr(MDLCacheDataType_t type) = 0;

	virtual studiohdr_t* LockStudioHdr(MDLHandle_t handle) = 0;
	virtual void UnlockStudioHdr(MDLHandle_t handle) = 0;

	virtual bool PreloadModel(MDLHandle_t handle) = 0;

	// Hammer uses this. If a model has an error loading in GetStudioHdr, then it is flagged
	// as an error model and any further attempts to load it will just get the error model.
	// That is, until you call this function. Then it will load the correct model.
	virtual void ResetErrorModelStatus(MDLHandle_t handle) = 0;

	virtual void MarkFrame() = 0;
};