#pragma once

class ShotRecord;

class Resolver {
public:
	enum Modes : size_t {
		RESOLVE_NONE = 0,
		RESOLVE_WALK,
		RESOLVE_STAND,
		RESOLVE_STAND1,
		RESOLVE_STAND2,
		RESOLVE_AIR,
		RESOLVE_BODY,
		RESOLVE_STOPPED_MOVING,
		RESOLVE_OVERRIDE,
		RESOLVE_LASTMOVE,
		RESOLVE_UNKNOWM,
		RESOLVE_BRUTEFORCE,
	};

public:
	LagRecord* FindIdealRecord(AimPlayer* data);
	LagRecord* FindLastRecord(AimPlayer* data);

	LagRecord* FindFirstRecord(AimPlayer* data);

	float GetLBYRotatedYaw(float lby, float yaw);

	bool IsYawSideways(Player* entity, float yaw);

	void OnBodyUpdate(Player* player, float value);
	float GetAwayAngle(LagRecord* record);

	void MatchShot(AimPlayer* data, LagRecord* record);
	void SetMode(LagRecord* record);

	void ResolveAngles(Player* player, LagRecord* record);
	void ResolveWalk(AimPlayer* data, LagRecord* record);
	void ResolveYawBruteforce(LagRecord* record, Player* player, AimPlayer* data);
	float GetDirectionAngle(int index, Player* player);
	void LastMoveLby(LagRecord* record, AimPlayer* data, Player* player);
	void ResolveStand(AimPlayer* data, LagRecord* record);
	void StandNS(AimPlayer* data, LagRecord* record);
	void ResolveAir(AimPlayer* data, LagRecord* record, Player* player);

	void AirNS(AimPlayer* data, LagRecord* record);
	void ResolvePoses(Player* player, LagRecord* record);
	void ResolveOverride(Player* player, LagRecord* record, AimPlayer* data);

	void AntiFreestand(Player* pEnemy, float& y, float flLeftDamage, float flRightDamage, float flRightFraction, float flLeftFraction, float flToMe, int& iShotsMissed);

public:
	std::array< vec3_t, 64 > m_impacts;
	int	   iPlayers[64];
	bool   m_step_switch;
	int    m_random_lag;
	float  m_next_random_update;
	float  m_random_angle;
	float  m_direction;
	float  m_auto;
	float  m_auto_dist;
	float  m_auto_last;
	float  m_view;

	class PlayerResolveRecord
	{
	public:
		struct AntiFreestandingRecord
		{
			int right_damage = 0, left_damage = 0;
			float right_fraction = 0.f, left_fraction = 0.f;
		};

	public:
		AntiFreestandingRecord m_sAntiEdge;
	};

	PlayerResolveRecord player_resolve_records[33];
};

extern Resolver g_resolver;