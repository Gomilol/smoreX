#include <Windows.h>
#include <stdint.h>
#include "Imgui/imgui.h"
#include "offsets.h"
#include "drawing.h"
#include "config.h"

int cfg::Health = 100;
int cfg::Armor = 100;
int cfg::AmmoClip = 90;
int cfg::Ammo = 30;
int cfg::SprintSpeed = 2;
int cfg::AmmoGrenade = 1;
int cfg::Damage = 100;
int cfg::selectedAimKey = 0;
int cfg::selectedTheme = 0;
float cfg::Smooth = 1;
float cfg::JumpHeight = 4.0f;
const int cfg::fHeight = 15; 
const int cfg::fWidth = 8; 
bool cfg::bFly = false;
bool cfg::bJump = false;
bool cfg::bSprint = false;
bool cfg::bRecoil = false;
bool cfg::showMenu= true;
bool cfg::closeMenu = false;
bool cfg::bHealth = false;
bool cfg::bTriggerbot = false;
bool cfg::bAmmo = false;
bool cfg::bAuto = false;
bool cfg::bDamage = false;
bool cfg::bAimbot = false;
bool cfg::bVisibleAim = false;
bool cfg::bSmooth = false;
bool cfg::bCrosshair = false;
bool cfg::bDistance = false;
bool cfg::bEsp = false;
bool cfg::bPaperwalls = false;
bool cfg::bHeadshot = false;
bool cfg::bVisibleEsp = true;
bool cfg::bSnaplines = true;
bool cfg::bName = true;
bool cfg::bHealthbar = true;
bool cfg::bTeamEsp = true;

ImVec4 rgb::enemyBox = ImColor(242, 131, 12, 1);
ImVec4 rgb::enemyBoxVisible = ImColor(255, 202, 79, 1);
ImVec4 rgb::teamBox = ImColor(79, 255, 252, 1);
ImVec4 rgb::text = ImColor(138, 138, 138, 26);
ImVec4 rgb::snapline = ImColor(255, 48, 48, 1);
ImVec4 rgb::healthBarBackground = ImColor(255, 105, 147, 1);
ImVec4 rgb::healthBar = ImColor(0, 224, 153, 1);