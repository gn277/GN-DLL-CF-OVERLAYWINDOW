#pragma once
#include "../../pch.h"
#include "DataStruct.h"
#include "../MemoryTools/MemoryTools.h"


class Game :public MemoryTools
{
private:
	DWORD game_processid = 0;								//����PID
	WNDPROC original_proc = nullptr;						//ԭʼ��Ϸ���ڻص����

	//�ϰ��жϽṹ
	IntersectInfo Info;
	IntersectQuery Query;
	//Driver* driver = nullptr;

public:
	void SetGameProcessId(DWORD processid) { game_processid = processid; }
	DWORD GetGameProcessId() { return game_processid; }

public:
	bool aimbot = false;												//�������
	bool track = false;													//�ӵ�׷��
	bool redname_track = false;											//����׷��
	bool silence_track = false;											//��Ĭ׷��
	bool silence_track_switch = false;									//��Ĭ׷�ٿ���
	bool range_track = false;											//��Χ׷��
	bool judgementbarrier = true;										//�ж��ϰ�
	bool no_backseat = false;											//�ӵ��޺�
	bool no_blood_loss = false;											//������Ѫ
	bool zero_second_reload = false;									//���뻻��
	bool space_fly = false;												//�ո����
	bool weapon_fire = false;											//��������
	bool teleport_track_enemy = false;									//˲��׷��
	bool spacecontinuousjump = false;									//�ո�����

	bool seckill_monster = false;										//������ɱ
	bool destroyer_seckill = false;										//�ƻ�����ɱ
	bool breakers_seckill = false;										//��������ɱ
	bool is_average = false;											//ƽ������
	bool absorb_monster = false;										//ȫͼ����

	//������ֵ
	int aim_speed = 18;													//�����ٶ�
	int aim_hotkey = AIM_HOTKEY_MOUSE_RIGHT;							//��׼�ȼ�
	int aim_position = AIM_POSITION_BODY;								//���λ��
	int track_range = 30;												//׷�ٷ�Χ
	float range_track_value = 80.0f;									//��Χ׷�ٷ�Χ
	int range_track_position = RANGE_TRACK_POSITION_CLAVICLE;			//��Χ׷�ٲ�λ
	float static_acceleration_value = 1;								//��������ֵ
	float squat_acceleration_value = 1;									//�¶׼���ֵ
	float seckill_speed = 40.0f;										//��ɱ�ٶ�

	//׷�ٱ���
	__int64 m_distance = 0;												//����
	__int64 m_locking_pawn;												//����pawn
	int m_lastdistance;													//�ϴξ���
	int m_recentdistance;												//�������
	int m_locking_bone_id;												//����BoneID
	int m_currentdistance;												//��ǰ����
	bool FirstCallKnifeDistance = TRUE;									//��һ�ε��õ���

	m_D3DCoordinate m_silence_track_coordinates = { 0.0f };				//��Ĭ׷�ٴ������
	BaseAddress GameBase = { NULL };

public:
	Game();
	~Game();

};


