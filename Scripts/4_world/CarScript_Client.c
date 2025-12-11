modded class CarScript
{
	protected const vector BM_BF_PTC_ORI = "180 0 0";
	protected bool BM_BackFire_VFXInProcess;
	protected float BM_BackFire_SinceLastThrust;
	protected ref array<ref array<float>> BM_BackFire_Sounds;
	protected ref array<vector> BM_BackFire_Pos;
	protected int BM_BackFire_Count, BM_BackFire_CountLast;
	protected int BM_BackFire_Steps, BM_BackFire_CurrentStep;
	protected bool BM_BackFire_Valid, BM_BackFire_Inited;

	//// Serv

	protected float BM_BackFire_PrevRPM;
	protected float BM_BackFire_PrevThrust;
	protected float BM_BackFire_NextTriggerTime;
	protected float BM_BackFire_RedlineRPM;
	protected bool  BM_BackFire_RedlineInited;
	protected float BM_BackFire_Cooldown = 1.3;
	protected float BM_BackFire_RpmDropThreshold = 1500.0;

	////

	void HandleBackfiresVFX(float dt)
	{
		if (!BM_BackFire_Sounds || BM_BackFire_Count < 0 || BM_BackFire_Count >= BM_BackFire_Sounds.Count())
			return;

		bool playParticles = false;

		BM_BackFire_SinceLastThrust += dt;

		float effectTime = BM_BackFire_Sounds[BM_BackFire_Count][BM_BackFire_CurrentStep];
		float future = BM_BackFire_SinceLastThrust + dt;

		if (BM_BackFire_SinceLastThrust >= effectTime)
		{
			playParticles = true;
			BM_BackFire_SinceLastThrust = 0;
			BM_BackFire_CurrentStep++;

			if (BM_BackFire_CurrentStep >= BM_BackFire_Steps)
			{
				BM_BackFire_CurrentStep   = 0;
				BM_BackFire_VFXInProcess  = false;

				BM_BackFire_Count = -1;
				BM_BackFire_CountLast = -1;
			}
		}
		else if (future >= effectTime)
		{
			playParticles = true;
		}

		if (playParticles)
		{
			BMBFPlayParticles();
		}
	}

	array<ref array<float>> BM_BackFire_InitSounds()
	{
		array<ref array<float>> bfSounds = new array<ref array<float>>();

		bfSounds.Insert({1.0});
		bfSounds.Insert({0.175});
		bfSounds.Insert({0.155, 0.195});
		bfSounds.Insert({0.143, 0.225, 0.194});
		bfSounds.Insert({0.143, 0.235, 0.142, 0.191});
		bfSounds.Insert({0.143, 0.216, 0.136, 0.127});
		bfSounds.Insert({0.096, 0.215, 0.138, 0.121, 0.144, 0.186});
		bfSounds.Insert({0.146, 0.214, 0.14});

		return bfSounds;
	}

	void BMBFPlayParticles()
	{
    	if (!GetGame() || !GetGame().IsClient())
        return;

    	if (!BM_BackFire_Pos || BM_BackFire_Pos.Count() == 0)
        return;

    	foreach (vector pipePos : BM_BackFire_Pos)
    	{
        	ParticleManager.GetInstance().PlayOnObject( ParticleList.BM_BACK_FIRE_PTC, this, pipePos, BM_BF_PTC_ORI);
        	vector worldPos = ModelToWorld(pipePos);
        	ScriptedLightBase light = ScriptedLightBase.CreateLight( BM_BackFire_Light, worldPos);
    	}
	}


	void BMBFPlaySound(string soundSetName)
	{
		if (!GetGame() || !GetGame().IsClient())
			return;

		PlayerBase player = PlayerBase.Cast(GetGame().GetPlayer());
		if (!player)
			return;

		WaveKind waveKind = WaveKind.WAVEEFFECT;
		if (!player.IsCameraInsideVehicle())
			waveKind = WaveKind.WAVEEFFECTEX;

		EffectSound sound = SEffectManager.CreateSound(soundSetName,GetPosition(),0,0,false,false);

		if (sound)
		{
			sound.SetParent(this);
			sound.SetLocalPosition(GetMemoryPointPos("ptcexhaust_start"));
			sound.SetSoundWaveKind(waveKind);
			sound.SoundPlay();
			sound.SetAutodestroy(true);
		}
	}

	override void OnVariablesSynchronized()
	{
		super.OnVariablesSynchronized();

		if (!GetGame() || !GetGame().IsClient())
			return;

		if (BM_BackFire_Count != BM_BackFire_CountLast)
		{
			BM_BackFire_CountLast = BM_BackFire_Count;

			if (BM_BackFire_Inited && !BM_BackFire_Valid)
				return;

			if (!BM_BackFire_Inited)
			{
				BM_BackFire_DataInit();
				if (!BM_BackFire_Valid)
				return;
			}

			string soundSetName = string.Format("BM_BackFire_SoundSet", BM_BackFire_Count);
			BMBFPlaySound(soundSetName);

			if (!BM_BackFire_Sounds || BM_BackFire_Sounds.Count() == 0)
			{
			    BMBFPlayParticles();
				return;
			}

			BM_BackFire_Steps = BM_BackFire_Sounds[BM_BackFire_Count].Count();
			BM_BackFire_CurrentStep = 0;
			BM_BackFire_VFXInProcess = true;
			BM_BackFire_SinceLastThrust = 0;
		}
	}

	void BM_BackFire_DataInit()
	{
		BM_BackFire_Inited = true;

		BM_BackFire_Sounds = BM_BackFire_InitSounds();
		BM_BackFire_Pos = new array<vector>();

		array<float> temp = new array<float>();
		ConfigGetFloatArray("BM_BackFire_Data", temp);

		if (temp.Count() % 3 != 0 || temp.Count() < 3)
		{
			BM_BackFire_Valid = false;
			Error(string.Format("%1 BM_BackFire invalid items count", GetType()));
			return;
		}

		for (int i = 0; i < temp.Count(); i += 3)
		{
			BM_BackFire_Pos.Insert(Vector(temp[i], temp[i + 1], temp[i + 2]));
		}

		BM_BackFire_Valid = BM_BackFire_Pos.Count() > 0;
	}

	override void EOnPostSimulate(IEntity other, float timeSlice)
	{
		super.EOnPostSimulate(other, timeSlice);

		if (!GetGame() || !GetGame().IsClient())
		return;

		if (BM_BackFire_VFXInProcess)
		{
			HandleBackfiresVFX(timeSlice);
		}
	}

	///// Serv
	void CarScript()
	{
		RegisterNetSyncVariableInt("BM_BackFire_Count", 0, 10);
	}

	protected void BM_BackFire_InitRedline()
	{
		if (BM_BackFire_RedlineInited)
		return;

		string basePath = CFG_VEHICLESPATH + " " + GetType();
		string redPath = basePath + " SimulationModule Engine RPMRedline";

		if (GetGame() && GetGame().ConfigIsExisting(redPath))
		{
			BM_BackFire_RedlineRPM = GetGame().ConfigGetFloat(redPath);
		}
		else
		{
			BM_BackFire_RedlineRPM = EngineGetRPMMax();
		}

		string cdPath = basePath + " BMBackfiresCooldown";
		if (GetGame().ConfigIsExisting(cdPath))
		{
			BM_BackFire_Cooldown = GetGame().ConfigGetFloat(cdPath);
		}

		BM_BackFire_RedlineInited = true;
	}

	protected void BM_BackFire_Trigger(int count = -1)
	{
		if (!GetGame() || !GetGame().IsServer())
		return;

		if (!EngineIsOn())
		return;

		float now = GetGame().GetTime() * 0.001;
		if (now < BM_BackFire_NextTriggerTime)
		return;

		if (count < 0)
		{
			count = Math.RandomIntInclusive(0, 7);
		}

		BM_BackFire_Count = count;
		SetSynchDirty();

		BM_BackFire_NextTriggerTime = now + BM_BackFire_Cooldown;
	}

	override void OnUpdate(float dt)
	{
		super.OnUpdate(dt);

		if (!GetGame() || !GetGame().IsServer())
		return;

		if (!EngineIsOn())
		{
			BM_BackFire_PrevRPM = 0;
			BM_BackFire_PrevThrust = 0;
			return;
		}

		BM_BackFire_InitRedline();

		float rpm = EngineGetRPM();
		float rpmIdle = EngineGetRPMIdle();
		float thrust = GetThrust();
		int gear = GetGear();

		bool allowedGear = (gear == 0 || gear == 1);
		if (!allowedGear)
		{
			BM_BackFire_PrevRPM = rpm;
			BM_BackFire_PrevThrust = thrust;
			return;
		}

		float redThreshold = BM_BackFire_RedlineRPM * 0.99; 
		float nearRedHigh  = BM_BackFire_RedlineRPM * 1.02;

		bool crossedIntoRed = (BM_BackFire_PrevRPM < redThreshold && rpm >= redThreshold);
		bool hardOverRed = (rpm >= BM_BackFire_RedlineRPM && rpm <= nearRedHigh && thrust > 0.7);

		if (crossedIntoRed || (hardOverRed && Math.RandomFloat01() < 0.05))
		{
			BM_BackFire_Trigger();
		}

		bool wasOnThrottle  = (BM_BackFire_PrevThrust > 0.75) && (BM_BackFire_PrevRPM > (rpmIdle + 2000.0));
		bool nowOffThrottle = (thrust < 0.05);
		float rpmDrop = BM_BackFire_PrevRPM - rpm;

		if (wasOnThrottle && nowOffThrottle && rpmDrop > BM_BackFire_RpmDropThreshold && BM_BackFire_PrevRPM > (BM_BackFire_RedlineRPM * 0.9))
		{
			if (Math.RandomFloat01() < 0.35)
			BM_BackFire_Trigger();
		}

		BM_BackFire_PrevRPM = rpm;
		BM_BackFire_PrevThrust = thrust;
	}
}