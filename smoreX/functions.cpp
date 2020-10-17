#include "functions.h"
#include "config.h"
#include "offsets.h"
#include "helper.h"
#include "drawing.h"
#include "menu.h"

#include <Windows.h>
#include <stdint.h>
#include <iostream>

GL::Font font;
void Meds() {
	if (cfg::bHealth) {
		offsets::localPlayer->Health = cfg::Health;
		offsets::localPlayer->Armor = cfg::Armor;
	}
}

void Ammo() {
	if (cfg::bAmmo) {
		*(int*)offsets::localPlayer->CurrentWeaponPtr->Ammo = cfg::Ammo;
		*(int*)offsets::localPlayer->CurrentWeaponPtr->Mag = cfg::AmmoClip;

		uintptr_t* LP = reinterpret_cast<uintptr_t*>(offsets::base + 0x10F4F4);
		alternate_patch(LP, 0x158, cfg::AmmoGrenade);
	}
}

void AntiRecoil() {
	if (cfg::bRecoil) {
		nop((BYTE*)0x463786, 10);
	}
	else {
		patch((BYTE*)0x463786, (BYTE*)"\x50\x8D\x4C\x24\x1C\x51\x8B\xCE\xFF\xD2", 10);
	}
}

void Sprint() {
	if (cfg::bSprint) {
		if (GetAsyncKeyState(VK_LSHIFT)) {
			uintptr_t* LP = reinterpret_cast<uintptr_t*>(offsets::base + 0x10F4F4);
			alternate_patch(LP, 0x80, cfg::SprintSpeed);
		}
	}
}

void SuperJump() {
	if (cfg::bJump) {
		if (GetAsyncKeyState(VK_SPACE)) {
			DWORD GameBase = *(PDWORD)0x509B74;
			*(PFLOAT)(GameBase + 0x18) = cfg::JumpHeight;

		}
	}
}

void Fly() {
	if (cfg::bFly) {
		DWORD old_protect;
		VirtualProtect((void*)0x45ADD8, 1, PAGE_EXECUTE_READWRITE, &old_protect);
		memcpy((void*)0x45ADD8, (PBYTE)("\x01"), 1);
		VirtualProtect((void*)0x45ADD8, 1, old_protect, &old_protect);
	}
	else {
		DWORD old_protect;
		VirtualProtect((void*)0x45ADD8, 1, PAGE_EXECUTE_READWRITE, &old_protect);
		memcpy((void*)0x45ADD8, (PBYTE)("\x00"), 1);
		VirtualProtect((void*)0x45ADD8, 1, old_protect, &old_protect);
	}
}

void Damage() {
	if (cfg::bDamage) {
		if (offsets::localPlayer == offsets::localPlayer->CurrentWeaponPtr->WeaponOwnerPtr) {
			offsets::localPlayer->CurrentWeaponPtr->WeaponInfoPtr->Damage = (short)cfg::Damage;
		}
		else {
			switch (offsets::localPlayer->CurrentWeaponPtr->WeaponId) {
			case 1: offsets::localPlayer->CurrentWeaponPtr->WeaponInfoPtr->Damage = Weapon::Pistol::damage; break;
			case 2: offsets::localPlayer->CurrentWeaponPtr->WeaponInfoPtr->Damage = Weapon::Carbine::damage; break;
			case 3: offsets::localPlayer->CurrentWeaponPtr->WeaponInfoPtr->Damage = Weapon::Shotgun::damage; break;
			case 4: offsets::localPlayer->CurrentWeaponPtr->WeaponInfoPtr->Damage = Weapon::SMG::damage; break;
			case 5: offsets::localPlayer->CurrentWeaponPtr->WeaponInfoPtr->Damage = Weapon::Sniper::damage; break;
			case 6: offsets::localPlayer->CurrentWeaponPtr->WeaponInfoPtr->Damage = Weapon::AR::damage; break;
			case 7: offsets::localPlayer->CurrentWeaponPtr->WeaponInfoPtr->Damage = Weapon::SMG::damage; break;
			case 8: offsets::localPlayer->CurrentWeaponPtr->WeaponInfoPtr->Damage = Weapon::Grenade::damage; break;
			case 9: offsets::localPlayer->CurrentWeaponPtr->WeaponInfoPtr->Damage = Weapon::Akimbo::damage; break;
			}
		}
	}
}

// Thanks to https://guidedhacking.com/members/kix.87156/ for the esp
void Esp() {
	if (cfg::bEsp) {
		float viewport[4];
		glGetFloatv(GL_VIEWPORT, viewport); 
		GL::SetupOrtho();

		for (int i = 0; i < *offsets::numOfPlayers; i++) {
			if (isValidEntity(offsets::entList->ents[i]) && isTeamGamemode() && offsets::localPlayer->Team != offsets::entList->ents[i]->Team && offsets::entList->ents[i]->Health > 0 || isValidEntity(offsets::entList->ents[i]) && !isTeamGamemode() && offsets::entList->ents[i]->Health > 0) {
				Entity* p = offsets::entList->ents[i];

				Vector3 headPos = p->HeadPos;
				Vector3 feetPos = p->Pos;

				headPos.z += 0.8f; 

				if (p->bCrouching) {
					headPos.z -= 0.5625; 
				}

				Vector2 headScreenPos, feetScreenPos;
				if (WorldToScreen(headPos, headScreenPos, offsets::vMatix, (int)GL::GetRes().x, (int)GL::GetRes().y) && WorldToScreen(feetPos, feetScreenPos, offsets::vMatix, (int)GL::GetRes().x, (int)GL::GetRes().y)) {
					float height = feetScreenPos.y - headScreenPos.y; 
					float width = height / 2.0f; 
					Vector2 tl; 
					tl.x = headScreenPos.x - width / 2.0f;
					tl.y = headScreenPos.y;
					Vector2 br; 
					br.x = headScreenPos.x + width / 2.0f;
					br.y = headScreenPos.y + height;

					if (cfg::bVisibleEsp) {
						if (isVisible(p)) {
							GL::DrawOutline(tl.x, tl.y, br.x, br.y, 2.0f, rgb::enemyBoxVisible);
						}
						else {
							GL::DrawOutline(tl.x, tl.y, br.x, br.y, 2.0f, rgb::enemyBox);
						}
					}
					else {
						GL::DrawOutline(tl.x, tl.y, br.x, br.y, 2.0f, rgb::enemyBox);
					}


					if (cfg::bSnaplines) {
						GL::DrawLine(GL::GetRes().x / 2, GL::GetRes().y, feetScreenPos.x, feetScreenPos.y, 2.0f, rgb::snapline);
					}

					if (cfg::bName) {
						std::string name = p->Name;
						int tWidth = name.length() * cfg::fWidth;
						float dif = width - tWidth;

						font.Print(tl.x + dif / 2, tl.y - cfg::fHeight / 2, rgb::text, "%s", p->Name);
					}

					if (cfg::bHealthbar) {
						float x = width / 8.0f;
						if (x < 3.5) {
							x = 3.5;
						}

						Vector2 healthB;
						healthB.x = br.x + x;
						healthB.y = br.y;
						Vector2 healthT;
						healthT.x = br.x + x;
						healthT.y = tl.y;

						GL::DrawLine(healthB.x, healthB.y, healthT.x, healthT.y, 5, rgb::healthBarBackground);

						float healthBarHeight = healthT.y - healthB.y;
						float health = p->Health * healthBarHeight / 100;
						Vector2 healthA;
						healthA.y = healthB.y + health;

						GL::DrawLine(healthB.x, healthB.y, healthT.x, healthA.y, 5, rgb::healthBar);
					}
				}
			}

			if (cfg::bTeamEsp && isTeamGamemode()) {
				if (isValidEntity(offsets::entList->ents[i]) && offsets::localPlayer->Team == offsets::entList->ents[i]->Team && offsets::entList->ents[i]->Health > 0) {
					Entity* p = offsets::entList->ents[i];

					Vector3 headPos = p->HeadPos;
					Vector3 feetPos = p->Pos;

					headPos.z += 0.8f;

					Vector2 headScreenPos, feetScreenPos;
					if (WorldToScreen(headPos, headScreenPos, offsets::vMatix, (int)GL::GetRes().x, (int)GL::GetRes().y) && WorldToScreen(feetPos, feetScreenPos, offsets::vMatix, (int)GL::GetRes().x, (int)GL::GetRes().y)) {
						float height = feetScreenPos.y - headScreenPos.y;
						float width = height / 2.0f;

						Vector2 tl;
						tl.x = headScreenPos.x - width / 2.0f;
						tl.y = headScreenPos.y;
						Vector2 br;
						br.x = headScreenPos.x + width / 2.0f;
						br.y = headScreenPos.y + height;

						GL::DrawOutline(tl.x, tl.y, br.x, br.y, 2.0f, rgb::teamBox);

						if (cfg::bName) {
							std::string name = p->Name;
							int tWidth = name.length() * cfg::fWidth;
							float dif = width - tWidth;

							font.Print(tl.x + dif / 2, tl.y - cfg::fHeight / 2, rgb::text, "%s", p->Name);
						}
					}
				}
			}
		}
		GL::RestoreGL();
	}
}


// Thanks to https://guidedhacking.com/members/kix.87156/ for the Aimbot
void Aimbot() {
	if (cfg::bAimbot) {
		Entity* tgt = NULL;
		Vector3 aim;

		switch (cfg::selectedAimKey) {
		case 0:
			if (GetAsyncKeyState(VK_RBUTTON)) {
				if (cfg::bCrosshair) {
					tgt = GetClosestTargetCrosshair(offsets::localPlayer, offsets::entList);
				}

				if (cfg::bDistance) {
					tgt = GetClosestTargetDistance(offsets::localPlayer, offsets::entList);
				}

				if (tgt != NULL) {
					aim = CalcAngle(offsets::localPlayer->HeadPos, tgt->HeadPos);

					if (cfg::bSmooth) {
						Vector3 diff = Vector3Sub(aim, offsets::localPlayer->ViewAngles);

						offsets::localPlayer->ViewAngles.x += diff.x / cfg::Smooth;
						offsets::localPlayer->ViewAngles.y += diff.y / cfg::Smooth;
					}
					else {
						offsets::localPlayer->ViewAngles.x = aim.x;
						offsets::localPlayer->ViewAngles.y = aim.y;
					}
				}
			}
			break;

		case 1:
			if (GetAsyncKeyState(VK_LBUTTON)) {
				if (cfg::bCrosshair) {
					tgt = GetClosestTargetCrosshair(offsets::localPlayer, offsets::entList);
				}

				if (cfg::bDistance) {
					tgt = GetClosestTargetDistance(offsets::localPlayer, offsets::entList);
				}

				if (tgt != NULL) {
					aim = CalcAngle(offsets::localPlayer->HeadPos, tgt->HeadPos);

					if (cfg::bSmooth) {
						Vector3 diff = Vector3Sub(aim, offsets::localPlayer->ViewAngles);

						offsets::localPlayer->ViewAngles.x += diff.x / cfg::Smooth;
						offsets::localPlayer->ViewAngles.y += diff.y / cfg::Smooth;
					}
					else {
						offsets::localPlayer->ViewAngles.x = aim.x;
						offsets::localPlayer->ViewAngles.y = aim.y;
					}
				}
			}
			break;

		case 2:
			if (GetAsyncKeyState(VK_LCONTROL)) {
				if (cfg::bCrosshair) {
					tgt = GetClosestTargetCrosshair(offsets::localPlayer, offsets::entList);
				}

				if (cfg::bDistance) {
					tgt = GetClosestTargetDistance(offsets::localPlayer, offsets::entList);
				}

				if (tgt != NULL) {
					aim = CalcAngle(offsets::localPlayer->HeadPos, tgt->HeadPos);

					if (cfg::bSmooth) {
						Vector3 diff = Vector3Sub(aim, offsets::localPlayer->ViewAngles);

						offsets::localPlayer->ViewAngles.x += diff.x / cfg::Smooth;
						offsets::localPlayer->ViewAngles.y += diff.y / cfg::Smooth;
					}
					else {
						offsets::localPlayer->ViewAngles.x = aim.x;
						offsets::localPlayer->ViewAngles.y = aim.y;
					}
				}
			}
			break;
		}
	}
}

void RapidFire() {
	if (cfg::bAuto) {
		*(int*)offsets::localPlayer->CurrentWeaponPtr->Wait = 0;
		offsets::localPlayer->CurrentWeaponPtr->WeaponInfoPtr->bAuto = true;
	}
	else {
		switch (offsets::localPlayer->CurrentWeaponPtr->WeaponId) {
		case 1: *(int*)offsets::localPlayer->CurrentWeaponPtr->Wait = Weapon::Pistol::wait; offsets::localPlayer->CurrentWeaponPtr->WeaponInfoPtr->bAuto = Weapon::Pistol::bAuto; break;
		case 2: *(int*)offsets::localPlayer->CurrentWeaponPtr->Wait = Weapon::Carbine::wait; offsets::localPlayer->CurrentWeaponPtr->WeaponInfoPtr->bAuto = Weapon::Carbine::bAuto; break;
		case 3: *(int*)offsets::localPlayer->CurrentWeaponPtr->Wait = Weapon::Shotgun::wait; offsets::localPlayer->CurrentWeaponPtr->WeaponInfoPtr->bAuto = Weapon::Shotgun::bAuto; break;
		case 4: *(int*)offsets::localPlayer->CurrentWeaponPtr->Wait = Weapon::SMG::wait; offsets::localPlayer->CurrentWeaponPtr->WeaponInfoPtr->bAuto = Weapon::SMG::bAuto; break;
		case 5: *(int*)offsets::localPlayer->CurrentWeaponPtr->Wait = Weapon::Sniper::wait; offsets::localPlayer->CurrentWeaponPtr->WeaponInfoPtr->bAuto = Weapon::Sniper::bAuto; break;
		case 6: *(int*)offsets::localPlayer->CurrentWeaponPtr->Wait = Weapon::AR::wait; offsets::localPlayer->CurrentWeaponPtr->WeaponInfoPtr->bAuto = Weapon::AR::bAuto; break;
		case 7: *(int*)offsets::localPlayer->CurrentWeaponPtr->Wait = Weapon::SMG::wait;  offsets::localPlayer->CurrentWeaponPtr->WeaponInfoPtr->bAuto = Weapon::SMG::bAuto; break;
		case 8: *(int*)offsets::localPlayer->CurrentWeaponPtr->Wait = Weapon::Grenade::wait; offsets::localPlayer->CurrentWeaponPtr->WeaponInfoPtr->bAuto = Weapon::Grenade::bAuto; break;
		case 9: *(int*)offsets::localPlayer->CurrentWeaponPtr->Wait = Weapon::Akimbo::wait; offsets::localPlayer->CurrentWeaponPtr->WeaponInfoPtr->bAuto = Weapon::Akimbo::bAuto; break;
		}
	}
}

void Triggerbot()
{
	if (cfg::bTriggerbot) {
		Entity* crosshairEnt = offsets::GetCrosshairEnt(); 

		if (!GetAsyncKeyState(VK_LBUTTON)){
			if (isValidEntity(crosshairEnt))  {
				if (crosshairEnt->Team != offsets::localPlayer->Team || !isTeamGamemode()) {
					offsets::localPlayer->bAttacking = 1; 
				}
			}
			else {
				offsets::localPlayer->bAttacking = 0; 
			}
		}
	}
}

void OnlyHeadshot() {
	if (cfg::bHeadshot) {
		nop((BYTE*)0x461767, 2); 
	}
	else {
		patch((BYTE*)0x461767, (BYTE*)"\x75\x09", 2);
	}
}

void PaperWalls() {
	if (cfg::bPaperwalls) {
		glDisable(GL_DEPTH_TEST);
	}
}