#pragma once
typedef unsigned short ModelInstanceHandle_t;
class Chams {
public:
	enum model_type_t : uint32_t {
		invalid = 0,
		player,
		weapon,
		arms,
		view_weapon
	};
	struct CHitMatrixEntry {
		int ent_index;
		ModelRenderInfo_t info;
		DrawModelState_t state;
		matrix3x4_t pBoneToWorld[128] = {};
		float time;
		matrix3x4_t model_to_world;
	};

public:
	model_type_t GetModelType(const ModelRenderInfo_t& info);
	bool IsInViewPlane(const vec3_t& world);

	void SetColor(Color col, IMaterial* mat = nullptr);
	void SetAlpha(float alpha, IMaterial* mat = nullptr);
	void SetupMaterial(IMaterial* mat, Color col, bool z_flag);

	void init();

	bool OverridePlayer(int index);
	bool GenerateLerpedMatrix(int index, BoneArray* out);
	void RenderHistoryChams(int index);
	bool DrawModel(IMatRenderContext* ctx, const DrawModelState_t& state, const ModelRenderInfo_t& info, matrix3x4_t* bone);
	void DrawChams(void* ecx, IMatRenderContext* ctx, const DrawModelState_t& state, const ModelRenderInfo_t& info, matrix3x4_t* bone);
	void SceneEnd();

	void RenderPlayer(Player* player);
	bool SortPlayers();

	void AddHitmatrix(LagRecord* record);

	void DrawHitMatrix(const std::function<void(IMatRenderContext* context, DrawModelState_t& state, ModelRenderInfo_t& info, matrix3x4_t* pBoneToWorld)> original);

public:
	std::deque<CHitMatrixEntry> m_Hitmatrix;
	std::vector< Player* > m_players;
	bool m_running;
	IMaterial* debugambientcube;
	IMaterial* debugdrawflat;
	IMaterial* materialMetall;
	IMaterial* materialMetall2;
	IMaterial* materialMetall3;
	IMaterial* materialMetallnZ;
	IMaterial* skeet;
	IMaterial* onetap;
	IMaterial* yeti;
};

extern Chams g_chams;