#pragma once

#ifndef cfg_H
#define cfg_H 
#include "ImGui/imgui.h"

namespace cfg {
	extern bool showMenu;
	extern bool closeMenu;
	extern bool bHealth;
	extern bool bAmmo;
	extern bool bRecoil;
	extern bool bDamage;
	extern bool bSprint;
	extern bool bFly;
	extern bool bPaperwalls;
	extern bool bEsp;
	extern bool bVisibleEsp;
	extern bool bSnaplines;
	extern bool bName;
	extern bool bVisibleAim;
	extern bool bTeamEsp;
	extern bool bHealthbar;
	extern bool bTriggerbot;
	extern bool bHeadshot;
	extern bool bAimbot;
	extern bool bDistance;
	extern bool bCrosshair;
	extern bool bSmooth;
	extern bool bAuto;
	extern bool bJump;

	extern float JumpHeight;
	extern float Smooth;
	extern int AmmoGrenade;
	extern int selectedAimKey;
	extern int SprintSpeed;
	extern int Ammo;
	extern int AmmoClip;
	extern int Health;
	extern int Armor;
	extern int Damage;
	extern const int fHeight;
	extern const int fWidth;
}

namespace rgb
{
	extern ImVec4 enemyBox;
	extern ImVec4 enemyBoxVisible;
	extern ImVec4 teamBox;
	extern ImVec4 text;
	extern ImVec4 snapline;
	extern ImVec4 healthBarBackground;
	extern ImVec4 healthBar;
}

#endif // !cfg


