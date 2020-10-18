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

void updateThemes() {
	switch (cfg::selectedTheme) {
	case 0: smoreXTheme(); break;
	case 1: OrangeTheme(); break;
	case 2: DarkTheme(); break;
	case 3: GrayTheme(); break;
	case 4: ImGui::StyleColorsClassic(); break;
	case 5: ImGui::StyleColorsDark(); break;
	case 6: ImGui::StyleColorsLight(); break;
	case 7: Kix48Theme(); break;
	}
}

void smoreXTheme() {
	ImGuiStyle* style = &ImGui::GetStyle();
	ImVec4* colors = style->Colors;

	colors[ImGuiCol_Text] = ImVec4(0.80f, 0.80f, 0.83f, 1.00f);
	colors[ImGuiCol_TextDisabled] = ImVec4(0.24f, 0.23f, 0.29f, 1.00f);
	colors[ImGuiCol_WindowBg] = ImVec4(0.06f, 0.05f, 0.07f, 1.00f);
	colors[ImGuiCol_PopupBg] = ImVec4(0.07f, 0.07f, 0.09f, 1.00f);
	colors[ImGuiCol_Border] = ImVec4(0.80f, 0.80f, 0.83f, 0.88f);
	colors[ImGuiCol_BorderShadow] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
	colors[ImGuiCol_Tab] = ImVec4(0.15f, 0.15f, 0.20f, 1.00f);
	colors[ImGuiCol_TabHovered] = ImVec4(0.10f, 0.10f, 0.15f, .75f);
	colors[ImGuiCol_TabActive] = ImVec4(0.20f, 0.20f, 0.25f, .60f);
	colors[ImGuiCol_FrameBg] = ImVec4(0.10f, 0.09f, 0.12f, 1.00f);
	colors[ImGuiCol_FrameBgHovered] = ImVec4(0.24f, 0.23f, 0.29f, 1.00f);
	colors[ImGuiCol_FrameBgActive] = ImVec4(0.56f, 0.56f, 0.58f, 1.00f);
	colors[ImGuiCol_Separator] = ImVec4(0.58f, 0.58f, 0.58f, 0.50f);
	colors[ImGuiCol_SeparatorHovered] = ImVec4(0.81f, 0.81f, 0.81f, 0.64f);
	colors[ImGuiCol_SeparatorActive] = ImVec4(0.81f, 0.81f, 0.81f, 0.64f);
	colors[ImGuiCol_TitleBg] = ImVec4(0.10f, 0.09f, 0.12f, 1.00f);
	colors[ImGuiCol_TitleBgCollapsed] = ImVec4(.10f, 0.10f, 0.10f, 0.75f);
	colors[ImGuiCol_TitleBgActive] = ImVec4(0.07f, 0.07f, 0.09f, 1.00f);
	colors[ImGuiCol_MenuBarBg] = ImVec4(0.10f, 0.09f, 0.12f, 1.00f);
	colors[ImGuiCol_ScrollbarBg] = ImVec4(0.10f, 0.09f, 0.12f, 1.00f);
	colors[ImGuiCol_ScrollbarGrab] = ImVec4(0.80f, 0.80f, 0.83f, 0.31f);
	colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(0.56f, 0.56f, 0.58f, 1.00f);
	colors[ImGuiCol_ScrollbarGrabActive] = ImVec4(0.06f, 0.05f, 0.07f, 1.00f);
	colors[ImGuiCol_CheckMark] = ImVec4(0.80f, 0.80f, 0.83f, 0.31f);
	colors[ImGuiCol_SliderGrab] = ImVec4(0.80f, 0.80f, 0.83f, 0.31f);
	colors[ImGuiCol_SliderGrabActive] = ImVec4(0.06f, 0.05f, 0.07f, 1.00f);
	colors[ImGuiCol_Button] = ImVec4(0.10f, 0.09f, 0.12f, 1.00f);
	colors[ImGuiCol_ButtonHovered] = ImVec4(0.24f, 0.23f, 0.29f, 1.00f);
	colors[ImGuiCol_ButtonActive] = ImVec4(0.56f, 0.56f, 0.58f, 1.00f);
	colors[ImGuiCol_Header] = ImVec4(0.10f, 0.09f, 0.12f, 1.00f);
	colors[ImGuiCol_HeaderHovered] = ImVec4(0.50f, 0.50f, 0.50f, .50f);
	colors[ImGuiCol_HeaderActive] = ImVec4(0.06f, 0.05f, 0.07f, 1.00f);
	colors[ImGuiCol_ResizeGrip] = ImVec4(0.30f, 0.30f, 0.30f, 1.00f);
	colors[ImGuiCol_ResizeGripHovered] = ImVec4(0.56f, 0.56f, 0.58f, 1.00f);
	colors[ImGuiCol_ResizeGripActive] = ImVec4(0.06f, 0.05f, 0.07f, 1.00f);
	colors[ImGuiCol_TextSelectedBg] = ImVec4(0.25f, 1.00f, 0.00f, 0.43f);
	colors[ImGuiCol_ModalWindowDarkening] = ImVec4(1.00f, 0.98f, 0.95f, 0.73f);


}

// created by https://github.com/Kix48 project https://github.com/Kix48/AssaultCube-Internal
void Kix48Theme() {
	ImGuiStyle* style = &ImGui::GetStyle();

	style->Colors[ImGuiCol_Text] = ImVec4(1.00f, 1.00f, 1.00f, 1.00f);
	style->Colors[ImGuiCol_TextDisabled] = ImVec4(0.50f, 0.50f, 0.50f, 1.00f);
	style->Colors[ImGuiCol_WindowBg] = ImVec4(0.06f, 0.06f, 0.06f, 0.94f);
	style->Colors[ImGuiCol_ChildBg] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
	style->Colors[ImGuiCol_PopupBg] = ImVec4(0.20f, 0.22f, 0.26f, 0.94f);
	style->Colors[ImGuiCol_Border] = ImVec4(0.43f, 0.43f, 0.50f, 0.50f);
	style->Colors[ImGuiCol_BorderShadow] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
	style->Colors[ImGuiCol_FrameBg] = ImVec4(0.20f, 0.22f, 0.26f, 0.54f);
	style->Colors[ImGuiCol_FrameBgHovered] = ImVec4(0.40f, 0.26f, 0.98f, 0.40f);
	style->Colors[ImGuiCol_FrameBgActive] = ImVec4(0.40f, 0.26f, 0.98f, 1.00f);
	style->Colors[ImGuiCol_TitleBg] = ImVec4(0.40f, 0.26f, 0.98f, 1.00f);
	style->Colors[ImGuiCol_TitleBgActive] = ImVec4(0.40f, 0.26f, 0.98f, 1.00f);
	style->Colors[ImGuiCol_TitleBgCollapsed] = ImVec4(0.00f, 0.00f, 0.00f, 0.51f);
	style->Colors[ImGuiCol_MenuBarBg] = ImVec4(0.14f, 0.14f, 0.14f, 1.00f);
	style->Colors[ImGuiCol_ScrollbarBg] = ImVec4(0.02f, 0.02f, 0.02f, 0.53f);
	style->Colors[ImGuiCol_ScrollbarGrab] = ImVec4(0.31f, 0.31f, 0.31f, 1.00f);
	style->Colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(0.41f, 0.41f, 0.41f, 1.00f);
	style->Colors[ImGuiCol_ScrollbarGrabActive] = ImVec4(0.51f, 0.51f, 0.51f, 1.00f);
	style->Colors[ImGuiCol_CheckMark] = ImVec4(0.40f, 0.26f, 1.00f, 1.00f);
	style->Colors[ImGuiCol_SliderGrab] = ImVec4(0.40f, 0.26f, 1.00f, 0.63f);
	style->Colors[ImGuiCol_SliderGrabActive] = ImVec4(0.40f, 0.26f, 1.00f, 1.00f);
	style->Colors[ImGuiCol_Button] = ImVec4(0.40f, 0.26f, 1.00f, 1.00f);
	style->Colors[ImGuiCol_ButtonHovered] = ImVec4(0.40f, 0.26f, 1.00f, 0.51f);
	style->Colors[ImGuiCol_ButtonActive] = ImVec4(0.40f, 0.26f, 1.00f, 1.00f);
	style->Colors[ImGuiCol_Header] = ImVec4(0.40f, 0.26f, 1.00f, 1.00f);
	style->Colors[ImGuiCol_HeaderHovered] = ImVec4(0.40f, 0.26f, 1.00f, 0.50f);
	style->Colors[ImGuiCol_HeaderActive] = ImVec4(0.40f, 0.26f, 1.00f, 1.00f);
	style->Colors[ImGuiCol_Separator] = ImVec4(0.43f, 0.43f, 0.50f, 0.50f);
	style->Colors[ImGuiCol_SeparatorHovered] = ImVec4(0.43f, 0.43f, 0.50f, 0.50f);
	style->Colors[ImGuiCol_SeparatorActive] = ImVec4(0.43f, 0.43f, 0.50f, 0.50f);
	style->Colors[ImGuiCol_ResizeGrip] = ImVec4(0.40f, 0.26f, 1.00f, 1.00f);
	style->Colors[ImGuiCol_ResizeGripHovered] = ImVec4(0.40f, 0.26f, 1.00f, 0.50f);
	style->Colors[ImGuiCol_ResizeGripActive] = ImVec4(0.40f, 0.26f, 1.00f, 1.00f);
	style->Colors[ImGuiCol_Tab] = ImVec4(0.40f, 0.26f, 1.00f, 1.00f);
	style->Colors[ImGuiCol_TabHovered] = ImVec4(0.40f, 0.26f, 1.00f, 0.56f);
	style->Colors[ImGuiCol_TabActive] = ImVec4(0.20f, 0.22f, 0.26f, 0.54f);
	style->Colors[ImGuiCol_TabUnfocused] = ImVec4(0.40f, 0.26f, 1.00f, 1.00f);
	style->Colors[ImGuiCol_TabUnfocusedActive] = ImVec4(0.40f, 0.26f, 1.00f, 1.00f);
	style->Colors[ImGuiCol_PlotLines] = ImVec4(0.61f, 0.61f, 0.61f, 1.00f);
	style->Colors[ImGuiCol_PlotLinesHovered] = ImVec4(1.00f, 0.43f, 0.35f, 1.00f);
	style->Colors[ImGuiCol_PlotHistogram] = ImVec4(0.90f, 0.70f, 0.00f, 1.00f);
	style->Colors[ImGuiCol_PlotHistogramHovered] = ImVec4(1.00f, 0.60f, 0.00f, 1.00f);
	style->Colors[ImGuiCol_TextSelectedBg] = ImVec4(0.26f, 0.59f, 0.98f, 0.35f);
	style->Colors[ImGuiCol_DragDropTarget] = ImVec4(1.00f, 1.00f, 0.00f, 0.90f);
	style->Colors[ImGuiCol_NavHighlight] = ImVec4(0.26f, 0.59f, 0.98f, 1.00f);
	style->Colors[ImGuiCol_NavWindowingHighlight] = ImVec4(1.00f, 1.00f, 1.00f, 0.70f);
	style->Colors[ImGuiCol_NavWindowingDimBg] = ImVec4(0.80f, 0.80f, 0.80f, 0.20f);
	style->Colors[ImGuiCol_ModalWindowDimBg] = ImVec4(0.80f, 0.80f, 0.80f, 0.35f);
}

void OrangeTheme() { // start of imgui theme thanks to https://github.com/CookiePLMonster
	ImGuiStyle* style = &ImGui::GetStyle();
	ImVec4* colors = style->Colors;

	colors[ImGuiCol_Text] = ImVec4(0.92f, 0.92f, 0.92f, 1.00f);
	colors[ImGuiCol_TextDisabled] = ImVec4(0.44f, 0.44f, 0.44f, 1.00f);
	colors[ImGuiCol_WindowBg] = ImVec4(0.06f, 0.06f, 0.06f, 1.00f);
	colors[ImGuiCol_ChildBg] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
	colors[ImGuiCol_PopupBg] = ImVec4(0.08f, 0.08f, 0.08f, 0.94f);
	colors[ImGuiCol_Border] = ImVec4(0.51f, 0.36f, 0.15f, 1.00f);
	colors[ImGuiCol_BorderShadow] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
	colors[ImGuiCol_FrameBg] = ImVec4(0.11f, 0.11f, 0.11f, 1.00f);
	colors[ImGuiCol_FrameBgHovered] = ImVec4(0.51f, 0.36f, 0.15f, 1.00f);
	colors[ImGuiCol_FrameBgActive] = ImVec4(0.78f, 0.55f, 0.21f, 1.00f);
	colors[ImGuiCol_TitleBg] = ImVec4(0.51f, 0.36f, 0.15f, 1.00f);
	colors[ImGuiCol_TitleBgActive] = ImVec4(0.91f, 0.64f, 0.13f, 1.00f);
	colors[ImGuiCol_TitleBgCollapsed] = ImVec4(0.00f, 0.00f, 0.00f, 0.51f);
	colors[ImGuiCol_MenuBarBg] = ImVec4(0.11f, 0.11f, 0.11f, 1.00f);
	colors[ImGuiCol_ScrollbarBg] = ImVec4(0.06f, 0.06f, 0.06f, 0.53f);
	colors[ImGuiCol_ScrollbarGrab] = ImVec4(0.21f, 0.21f, 0.21f, 1.00f);
	colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(0.47f, 0.47f, 0.47f, 1.00f);
	colors[ImGuiCol_ScrollbarGrabActive] = ImVec4(0.81f, 0.83f, 0.81f, 1.00f);
	colors[ImGuiCol_CheckMark] = ImVec4(0.78f, 0.55f, 0.21f, 1.00f);
	colors[ImGuiCol_SliderGrab] = ImVec4(0.91f, 0.64f, 0.13f, 1.00f);
	colors[ImGuiCol_SliderGrabActive] = ImVec4(0.91f, 0.64f, 0.13f, 1.00f);
	colors[ImGuiCol_Button] = ImVec4(0.51f, 0.36f, 0.15f, 1.00f);
	colors[ImGuiCol_ButtonHovered] = ImVec4(0.91f, 0.64f, 0.13f, 1.00f);
	colors[ImGuiCol_ButtonActive] = ImVec4(0.78f, 0.55f, 0.21f, 1.00f);
	colors[ImGuiCol_Header] = ImVec4(0.51f, 0.36f, 0.15f, 1.00f);
	colors[ImGuiCol_HeaderHovered] = ImVec4(0.91f, 0.64f, 0.13f, 1.00f);
	colors[ImGuiCol_HeaderActive] = ImVec4(0.93f, 0.65f, 0.14f, 1.00f);
	colors[ImGuiCol_Separator] = ImVec4(0.21f, 0.21f, 0.21f, 1.00f);
	colors[ImGuiCol_SeparatorHovered] = ImVec4(0.91f, 0.64f, 0.13f, 1.00f);
	colors[ImGuiCol_SeparatorActive] = ImVec4(0.78f, 0.55f, 0.21f, 1.00f);
	colors[ImGuiCol_ResizeGrip] = ImVec4(0.21f, 0.21f, 0.21f, 1.00f);
	colors[ImGuiCol_ResizeGripHovered] = ImVec4(0.91f, 0.64f, 0.13f, 1.00f);
	colors[ImGuiCol_ResizeGripActive] = ImVec4(0.78f, 0.55f, 0.21f, 1.00f);
	colors[ImGuiCol_Tab] = ImVec4(0.51f, 0.36f, 0.15f, 1.00f);
	colors[ImGuiCol_TabHovered] = ImVec4(0.91f, 0.64f, 0.13f, 1.00f);
	colors[ImGuiCol_TabActive] = ImVec4(0.78f, 0.55f, 0.21f, 1.00f);
	colors[ImGuiCol_TabUnfocused] = ImVec4(0.07f, 0.10f, 0.15f, 0.97f);
	colors[ImGuiCol_TabUnfocusedActive] = ImVec4(0.14f, 0.26f, 0.42f, 1.00f);
	colors[ImGuiCol_PlotLines] = ImVec4(0.61f, 0.61f, 0.61f, 1.00f);
	colors[ImGuiCol_PlotLinesHovered] = ImVec4(1.00f, 0.43f, 0.35f, 1.00f);
	colors[ImGuiCol_PlotHistogram] = ImVec4(0.90f, 0.70f, 0.00f, 1.00f);
	colors[ImGuiCol_PlotHistogramHovered] = ImVec4(1.00f, 0.60f, 0.00f, 1.00f);
	colors[ImGuiCol_TextSelectedBg] = ImVec4(0.26f, 0.59f, 0.98f, 0.35f);
	colors[ImGuiCol_DragDropTarget] = ImVec4(1.00f, 1.00f, 0.00f, 0.90f);
	colors[ImGuiCol_NavHighlight] = ImVec4(0.26f, 0.59f, 0.98f, 1.00f);
	colors[ImGuiCol_NavWindowingHighlight] = ImVec4(1.00f, 1.00f, 1.00f, 0.70f);
	colors[ImGuiCol_NavWindowingDimBg] = ImVec4(0.80f, 0.80f, 0.80f, 0.20f);
	colors[ImGuiCol_ModalWindowDimBg] = ImVec4(0.80f, 0.80f, 0.80f, 0.35f);
} // end of imgui theme

void DarkTheme() { // start of imgui theme thanks to https://github.com/codz01
	ImGuiStyle* style = &ImGui::GetStyle();
	ImVec4* colors = style->Colors;

	colors[ImGuiCol_Text] = ImVec4(1.00f, 1.00f, 1.00f, 0.95f);
	colors[ImGuiCol_TextDisabled] = ImVec4(0.50f, 0.50f, 0.50f, 1.00f);
	colors[ImGuiCol_WindowBg] = ImVec4(0.13f, 0.12f, 0.12f, 1.00f);
	colors[ImGuiCol_ChildBg] = ImVec4(1.00f, 1.00f, 1.00f, 0.00f);
	colors[ImGuiCol_PopupBg] = ImVec4(0.05f, 0.05f, 0.05f, 0.94f);
	colors[ImGuiCol_Border] = ImVec4(0.53f, 0.53f, 0.53f, 0.46f);
	colors[ImGuiCol_BorderShadow] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
	colors[ImGuiCol_FrameBg] = ImVec4(0.00f, 0.00f, 0.00f, 0.85f);
	colors[ImGuiCol_FrameBgHovered] = ImVec4(0.22f, 0.22f, 0.22f, 0.40f);
	colors[ImGuiCol_FrameBgActive] = ImVec4(0.16f, 0.16f, 0.16f, 0.53f);
	colors[ImGuiCol_TitleBg] = ImVec4(0.00f, 0.00f, 0.00f, 1.00f);
	colors[ImGuiCol_TitleBgActive] = ImVec4(0.00f, 0.00f, 0.00f, 1.00f);
	colors[ImGuiCol_TitleBgCollapsed] = ImVec4(0.00f, 0.00f, 0.00f, 0.51f);
	colors[ImGuiCol_MenuBarBg] = ImVec4(0.12f, 0.12f, 0.12f, 1.00f);
	colors[ImGuiCol_ScrollbarBg] = ImVec4(0.02f, 0.02f, 0.02f, 0.53f);
	colors[ImGuiCol_ScrollbarGrab] = ImVec4(0.31f, 0.31f, 0.31f, 1.00f);
	colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(0.41f, 0.41f, 0.41f, 1.00f);
	colors[ImGuiCol_ScrollbarGrabActive] = ImVec4(0.48f, 0.48f, 0.48f, 1.00f);
	colors[ImGuiCol_CheckMark] = ImVec4(0.79f, 0.79f, 0.79f, 1.00f);
	colors[ImGuiCol_SliderGrab] = ImVec4(0.48f, 0.47f, 0.47f, 0.91f);
	colors[ImGuiCol_SliderGrabActive] = ImVec4(0.56f, 0.55f, 0.55f, 0.62f);
	colors[ImGuiCol_Button] = ImVec4(0.50f, 0.50f, 0.50f, 0.63f);
	colors[ImGuiCol_ButtonHovered] = ImVec4(0.67f, 0.67f, 0.68f, 0.63f);
	colors[ImGuiCol_ButtonActive] = ImVec4(0.26f, 0.26f, 0.26f, 0.63f);
	colors[ImGuiCol_Header] = ImVec4(0.54f, 0.54f, 0.54f, 0.58f);
	colors[ImGuiCol_HeaderHovered] = ImVec4(0.64f, 0.65f, 0.65f, 0.80f);
	colors[ImGuiCol_HeaderActive] = ImVec4(0.25f, 0.25f, 0.25f, 0.80f);
	colors[ImGuiCol_Separator] = ImVec4(0.58f, 0.58f, 0.58f, 0.50f);
	colors[ImGuiCol_SeparatorHovered] = ImVec4(0.81f, 0.81f, 0.81f, 0.64f);
	colors[ImGuiCol_SeparatorActive] = ImVec4(0.81f, 0.81f, 0.81f, 0.64f);
	colors[ImGuiCol_ResizeGrip] = ImVec4(0.87f, 0.87f, 0.87f, 0.53f);
	colors[ImGuiCol_ResizeGripHovered] = ImVec4(0.87f, 0.87f, 0.87f, 0.74f);
	colors[ImGuiCol_ResizeGripActive] = ImVec4(0.87f, 0.87f, 0.87f, 0.74f);
	colors[ImGuiCol_Tab] = ImVec4(0.01f, 0.01f, 0.01f, 0.86f);
	colors[ImGuiCol_TabHovered] = ImVec4(0.29f, 0.29f, 0.29f, 1.00f);
	colors[ImGuiCol_TabActive] = ImVec4(0.31f, 0.31f, 0.31f, 1.00f);
	colors[ImGuiCol_TabUnfocused] = ImVec4(0.02f, 0.02f, 0.02f, 1.00f);
	colors[ImGuiCol_TabUnfocusedActive] = ImVec4(0.19f, 0.19f, 0.19f, 1.00f);
	colors[ImGuiCol_PlotLines] = ImVec4(0.61f, 0.61f, 0.61f, 1.00f);
	colors[ImGuiCol_PlotLinesHovered] = ImVec4(0.68f, 0.68f, 0.68f, 1.00f);
	colors[ImGuiCol_PlotHistogram] = ImVec4(0.90f, 0.77f, 0.33f, 1.00f);
	colors[ImGuiCol_PlotHistogramHovered] = ImVec4(0.87f, 0.55f, 0.08f, 1.00f);
	colors[ImGuiCol_TextSelectedBg] = ImVec4(0.47f, 0.60f, 0.76f, 0.47f);
	colors[ImGuiCol_DragDropTarget] = ImVec4(0.58f, 0.58f, 0.58f, 0.90f);
	colors[ImGuiCol_NavHighlight] = ImVec4(0.60f, 0.60f, 0.60f, 1.00f);
	colors[ImGuiCol_NavWindowingHighlight] = ImVec4(1.00f, 1.00f, 1.00f, 0.70f);
	colors[ImGuiCol_NavWindowingDimBg] = ImVec4(0.80f, 0.80f, 0.80f, 0.20f);
	colors[ImGuiCol_ModalWindowDimBg] = ImVec4(0.80f, 0.80f, 0.80f, 0.35f);

} // end of imgui theme

// thanks to https://github.com/malamanteau for the theme
void GrayTheme() {
	ImGuiStyle* style = &ImGui::GetStyle();
	ImVec4* colors = style->Colors;

	colors[ImGuiCol_Text] = ImVec4(1.00f, 1.00f, 1.00f, 1.00f);
	colors[ImGuiCol_TextDisabled] = ImVec4(0.40f, 0.40f, 0.40f, 1.00f);
	colors[ImGuiCol_ChildBg] = ImVec4(0.25f, 0.25f, 0.25f, 1.00f);
	colors[ImGuiCol_WindowBg] = ImVec4(0.25f, 0.25f, 0.25f, 1.00f);
	colors[ImGuiCol_PopupBg] = ImVec4(0.25f, 0.25f, 0.25f, 1.00f);
	colors[ImGuiCol_Border] = ImVec4(0.12f, 0.12f, 0.12f, 0.71f);
	colors[ImGuiCol_BorderShadow] = ImVec4(1.00f, 1.00f, 1.00f, 0.06f);
	colors[ImGuiCol_FrameBg] = ImVec4(0.42f, 0.42f, 0.42f, 0.54f);
	colors[ImGuiCol_FrameBgHovered] = ImVec4(0.42f, 0.42f, 0.42f, 0.40f);
	colors[ImGuiCol_FrameBgActive] = ImVec4(0.56f, 0.56f, 0.56f, 0.67f);
	colors[ImGuiCol_TitleBg] = ImVec4(0.19f, 0.19f, 0.19f, 1.00f);
	colors[ImGuiCol_TitleBgActive] = ImVec4(0.22f, 0.22f, 0.22f, 1.00f);
	colors[ImGuiCol_TitleBgCollapsed] = ImVec4(0.17f, 0.17f, 0.17f, 0.90f);
	colors[ImGuiCol_MenuBarBg] = ImVec4(0.335f, 0.335f, 0.335f, 1.000f);
	colors[ImGuiCol_ScrollbarBg] = ImVec4(0.24f, 0.24f, 0.24f, 0.53f);
	colors[ImGuiCol_ScrollbarGrab] = ImVec4(0.41f, 0.41f, 0.41f, 1.00f);
	colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(0.52f, 0.52f, 0.52f, 1.00f);
	colors[ImGuiCol_ScrollbarGrabActive] = ImVec4(0.76f, 0.76f, 0.76f, 1.00f);
	colors[ImGuiCol_CheckMark] = ImVec4(0.65f, 0.65f, 0.65f, 1.00f);
	colors[ImGuiCol_SliderGrab] = ImVec4(0.52f, 0.52f, 0.52f, 1.00f);
	colors[ImGuiCol_SliderGrabActive] = ImVec4(0.64f, 0.64f, 0.64f, 1.00f);
	colors[ImGuiCol_Button] = ImVec4(0.54f, 0.54f, 0.54f, 0.35f);
	colors[ImGuiCol_ButtonHovered] = ImVec4(0.52f, 0.52f, 0.52f, 0.59f);
	colors[ImGuiCol_ButtonActive] = ImVec4(0.76f, 0.76f, 0.76f, 1.00f);
	colors[ImGuiCol_Header] = ImVec4(0.38f, 0.38f, 0.38f, 1.00f);
	colors[ImGuiCol_HeaderHovered] = ImVec4(0.47f, 0.47f, 0.47f, 1.00f);
	colors[ImGuiCol_HeaderActive] = ImVec4(0.76f, 0.76f, 0.76f, 0.77f);
	colors[ImGuiCol_Separator] = ImVec4(0.000f, 0.000f, 0.000f, 0.137f);
	colors[ImGuiCol_SeparatorHovered] = ImVec4(0.700f, 0.671f, 0.600f, 0.290f);
	colors[ImGuiCol_SeparatorActive] = ImVec4(0.702f, 0.671f, 0.600f, 0.674f);
	colors[ImGuiCol_ResizeGrip] = ImVec4(0.87f, 0.87f, 0.87f, 0.53f);
	colors[ImGuiCol_ResizeGripHovered] = ImVec4(0.87f, 0.87f, 0.87f, 0.74f);
	colors[ImGuiCol_ResizeGripActive] = ImVec4(0.87f, 0.87f, 0.87f, 0.74f);
	colors[ImGuiCol_PlotLines] = ImVec4(0.61f, 0.61f, 0.61f, 1.00f);
	colors[ImGuiCol_PlotLinesHovered] = ImVec4(1.00f, 0.43f, 0.35f, 1.00f);
	colors[ImGuiCol_PlotHistogram] = ImVec4(0.90f, 0.70f, 0.00f, 1.00f);
	colors[ImGuiCol_PlotHistogramHovered] = ImVec4(1.00f, 0.60f, 0.00f, 1.00f);
	colors[ImGuiCol_TextSelectedBg] = ImVec4(0.73f, 0.73f, 0.73f, 0.35f);
	colors[ImGuiCol_ModalWindowDimBg] = ImVec4(0.80f, 0.80f, 0.80f, 0.35f);
	colors[ImGuiCol_DragDropTarget] = ImVec4(1.00f, 1.00f, 0.00f, 0.90f);
	colors[ImGuiCol_NavHighlight] = ImVec4(0.26f, 0.59f, 0.98f, 1.00f);
	colors[ImGuiCol_NavWindowingHighlight] = ImVec4(1.00f, 1.00f, 1.00f, 0.70f);
	colors[ImGuiCol_NavWindowingDimBg] = ImVec4(0.80f, 0.80f, 0.80f, 0.20f);
	colors[ImGuiCol_Tab] = ImVec4(0.15f, 0.15f, 0.15f, 0.85f);
	colors[ImGuiCol_TabHovered] = ImVec4(0.25f, 0.25f, 0.25f, 1.00f);
	colors[ImGuiCol_TabActive] = ImVec4(0.30f, 0.30f, 0.30f, 1.00f);

}