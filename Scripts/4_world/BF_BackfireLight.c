class BM_BackFire_Light : ScriptedLightBase
{
	void BM_BackFire_Light()
	{
		SetDiffuseColor(1.0, 0.6, 0.25);
		SetAmbientColor(1.0, 0.35, 0.1);

		SetRadiusTo(4.0);
		SetBrightnessTo(3.0);
		SetLifetime(0.18);

		EnableSpecular(true);
		SetFlareVisible(false);
		SetCastShadow(false);
	}
}