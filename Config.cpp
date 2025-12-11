class CfgPatches
{
	class BM_BackFire_Client
	{
		units[]={};
		weapons[]={};
		requiredVersion=0.1;
		requiredAddons[]=
		{
			"DZ_Data",
			"DZ_Sounds_Effects" 
		};
	};
};
// I'm never gonna dance again guilty feet have got no rhytchm
class CfgMods
{
	class BM_BackFire_Client
	{
		dir="BM_BackFire_Client";
		picture="";
		action="";
		hideName=1;
		hidePicture=1;
		name="BM_BackFire_Client";
		credits="";
		author="Breathe";
		authorID="76561199653268394";
		version="1.0";
		extra=0;
		type="mod";
		class defs
		{
			class gameScriptModule
			{
				value="";
				files[]=
				{
					"BM_BackFire_Client/scripts/3_Game"
				};
			};
			class worldScriptModule
			{
				value="";
				files[]=
				{
					"BM_BackFire_Client/scripts/4_World"
				};
			};
			class missionScriptModule
			{
				value="";
				files[]=
				{
					"BM_BackFire_Client/scripts/5_Mission"
				};
			};
		};
	};
};

class CfgSoundShaders
{
	class BM_BackFire_Base_SoundShader
	{
		range=250;
		volume=5;
	};
	class BM_BackFire_SoundShader: BM_BackFire_Base_SoundShader
	{
		samples[]=
		{
			
			{
				"BM_BackFire_Client\Assets\Sounds\BackFire",
				1
			}
		};
	};
};
class CfgSoundSets
{
	class baseEngine_EXT_SoundSet;
	class BM_BackFire_Base_SoundSet: baseEngine_EXT_SoundSet
	{
		sound3DProcessingType = "Vehicle_Ext_3DProcessingType";
		distanceFilter = "softVehiclesDistanceFreqAttenuationFilter";
		volumeCurve = "vehicleEngineAttenuationCurve";
		spatial = 1;
		loop = 0;
		obstructionFactor=0;
		occlusionFactor=0;
		positionOffset[]={0,0,0};
	};
	class BM_BackFire_SoundSet: BM_BackFire_Base_SoundSet
	{
		soundShaders[]=
		{
			"BM_BackFire_SoundShader"
		};
	};
};