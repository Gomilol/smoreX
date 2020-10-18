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

ImVec4 rgb::enemyBox = ImColor(236, 35, 35, 1);
ImVec4 rgb::enemyBoxVisible = ImColor(255, 57, 57, 1);
ImVec4 rgb::teamBox = ImColor(100, 149, 237, 1);
ImVec4 rgb::text = ImColor(255, 255, 255, 1);
ImVec4 rgb::snapline = ImColor(105, 105, 105, 1);
ImVec4 rgb::healthBarBackground = ImColor(240, 128, 128, 1);
ImVec4 rgb::healthBar = ImColor(152, 251, 152, 1);