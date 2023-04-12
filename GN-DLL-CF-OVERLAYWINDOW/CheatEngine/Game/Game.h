#pragma once
#include "../../pch.h"
#include "DataStruct.h"
#include "../MemoryTools/MemoryTools.h"


class Game :public MemoryTools
{
private:
	DWORD game_processid = 0;								//进程PID
	WNDPROC original_proc = nullptr;						//原始游戏窗口回调句柄

	//障碍判断结构
	IntersectInfo Info;
	IntersectQuery Query;
	//Driver* driver = nullptr;

public:
	void SetGameProcessId(DWORD processid) { game_processid = processid; }
	DWORD GetGameProcessId() { return game_processid; }

public:
	bool aimbot = false;												//鼠标自瞄
	bool track = false;													//子弹追踪
	bool redname_track = false;											//红名追踪
	bool silence_track = false;											//静默追踪
	bool silence_track_switch = false;									//静默追踪开关
	bool range_track = false;											//范围追踪
	bool judgementbarrier = true;										//判断障碍
	bool no_backseat = false;											//子弹无后
	bool no_blood_loss = false;											//卡不掉血
	bool zero_second_reload = false;									//零秒换弹
	bool space_fly = false;												//空格飞天
	bool weapon_fire = false;											//武器连发
	bool teleport_track_enemy = false;									//瞬移追敌
	bool spacecontinuousjump = false;									//空格连跳

	bool seckill_monster = false;										//怪物秒杀
	bool destroyer_seckill = false;										//破坏者秒杀
	bool breakers_seckill = false;										//爆裂者秒杀
	bool is_average = false;											//平均分数
	bool absorb_monster = false;										//全图吸怪

	//功能数值
	int aim_speed = 18;													//自瞄速度
	int aim_hotkey = AIM_HOTKEY_MOUSE_RIGHT;							//瞄准热键
	int aim_position = AIM_POSITION_BODY;								//打击位置
	int track_range = 30;												//追踪范围
	float range_track_value = 80.0f;									//范围追踪范围
	int range_track_position = RANGE_TRACK_POSITION_CLAVICLE;			//范围追踪部位
	float static_acceleration_value = 1;								//静步加速值
	float squat_acceleration_value = 1;									//下蹲加速值
	float seckill_speed = 40.0f;										//秒杀速度

	//追踪变量
	__int64 m_distance = 0;												//距离
	__int64 m_locking_pawn;												//锁定pawn
	int m_lastdistance;													//上次距离
	int m_recentdistance;												//最近距离
	int m_locking_bone_id;												//锁定BoneID
	int m_currentdistance;												//当前距离
	bool FirstCallKnifeDistance = TRUE;									//第一次调用刀距

	m_D3DCoordinate m_silence_track_coordinates = { 0.0f };				//静默追踪打击坐标
	BaseAddress GameBase = { NULL };

public:
	Game();
	~Game();

};


