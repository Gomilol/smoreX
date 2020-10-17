#define _USE_MATH_DEFINES

#include <Windows.h>
#include <iostream>
#include <math.h>
#include <float.h>
#include "helper.h"
#include "config.h"
#include "offsets.h"
#include "drawing.h"

void patch(BYTE* dst, BYTE* src, unsigned int size) {
	DWORD old_protect;
	VirtualProtect(dst, size, PAGE_EXECUTE_READWRITE, &old_protect);
	memcpy(dst, src, size);
	VirtualProtect(dst, size, old_protect, &old_protect);
}

void nop(BYTE* dst, unsigned int size) {
	DWORD old_protect;
	VirtualProtect(dst, size, PAGE_EXECUTE_READWRITE, &old_protect);
	memset(dst, 0x90, size);
	VirtualProtect(dst, size, old_protect, &old_protect);
}

void alternate_patch(uintptr_t* base, int offset, int value) {
	*reinterpret_cast<int*>(*base + offset) = value;
}

Vector3 Vector3Add(Vector3 src, Vector3 dst) {
	Vector3 add;
	add.x = src.x + dst.x;
	add.y = src.y + dst.y;
	add.z = src.z + dst.z;
	return add;
}

Vector3 Vector3Sub(Vector3 src, Vector3 dst) {
	Vector3 sub;
	sub.x = src.x - dst.x;
	sub.y = src.y - dst.y;
	sub.z = src.z - dst.z;
	return sub;
}

Vector3 Vector3Multiply(Vector3 src, Vector3 dst) {
	Vector3 multiply;
	multiply.x = src.x * dst.x;
	multiply.y = src.y * dst.y;
	multiply.z = src.z * dst.z;
	return multiply;
}

Vector3 Vector3Divide(Vector3 src, Vector3 dst) {
	Vector3 divide;
	divide.x = src.x / dst.x;
	divide.y = src.y / dst.y;
	divide.z = src.z / dst.z;
	return divide;
}

//Checks if the entity is valid. Credit: Rake for having it somewhere
bool isValidEntity(Entity* ent) {
	if (ent) {
		if (ent->vTable == 0x4E4A98 || ent->vTable == 0x4E4AC0) {
			return true;
		}
	}
	return false;
}

bool isVisible(Entity* ent) {
	TraceResult trace;
	trace.collided = false;
	Vector3 from = offsets::localPlayer->HeadPos;
	Vector3 to = ent->HeadPos;

	__asm {
		push 0; bSkipTags
		push 0; bCheckPlayers
		push offsets::localPlayer
		push to.z
		push to.y
		push to.x
		push from.z
		push from.y
		push from.x
		lea eax, [trace]
		call offsets::traceLine;
		add esp, 36
	}
	return !trace.collided;
}

bool isTeamGamemode() {
	int gameMode = *(int*)offsets::gameMode;

	return (gameMode == 0 || gameMode == 4 || gameMode == 5 || gameMode == 7 || gameMode == 11 || gameMode == 13 || gameMode == 14 || gameMode == 16 || gameMode == 17 || gameMode == 20 || gameMode == 21);
}

float DistanceVec2(Vector2 dst, Vector2 src) {
	float distance;
	distance = sqrtf(powf(dst.x - src.x, 2) + powf(dst.y - src.y, 2));
	return distance;
}

float DistanceVec3(Vector3 dst, Vector3 src) {
	float distance;
	distance = sqrtf(powf(dst.x - src.x, 2) + powf(dst.y - src.y, 2) + powf(dst.z - src.z, 2));
	return distance;
}

Vector3 CalcAngle(Vector3 src, Vector3 dst) {
	Vector3 angle;
	angle.x = -atan2f(dst.x - src.x, dst.y - src.y) / (float)M_PI * 180 + 180;
	angle.y = asinf((dst.z - src.z) / DistanceVec3(src, dst)) * 180 / (float)M_PI;
	angle.z = 0;

	return angle;
}

Entity* GetClosestTargetDistance(Entity* localPlayer, EntList* entList) {
	float oDist = FLT_MAX;
	float nDist = 0;
	Entity* target = nullptr;

	for (int i = 0; i < *offsets::numOfPlayers; i++) {
		if (cfg::bVisibleAim) {
			if (isValidEntity(offsets::entList->ents[i]) && offsets::entList->ents[i]->Health > 0 && isTeamGamemode() && offsets::entList->ents[i]->Team != offsets::localPlayer->Team && isVisible(entList->ents[i]) || isValidEntity(offsets::entList->ents[i]) && offsets::entList->ents[i]->Health > 0 && !isTeamGamemode() && isVisible(entList->ents[i])) {

				nDist = DistanceVec3(localPlayer->Pos, offsets::entList->ents[i]->Pos);

				if (nDist < oDist) {
					oDist = nDist;
					target = offsets::entList->ents[i];
				}
			}
		}
		else
		{
			if (isValidEntity(offsets::entList->ents[i]) && offsets::entList->ents[i]->Health > 0 && isTeamGamemode() && offsets::entList->ents[i]->Team != offsets::localPlayer->Team || isValidEntity(offsets::entList->ents[i]) && offsets::entList->ents[i]->Health > 0 && !isTeamGamemode()) {

				nDist = DistanceVec3(localPlayer->Pos, offsets::entList->ents[i]->Pos);

				if (nDist < oDist) {
					oDist = nDist;
					target = offsets::entList->ents[i];
				}
			}
		}
	}
	return target;
}

Entity* GetClosestTargetCrosshair(Entity* localPlayer, EntList* entList) {
	float oDist = FLT_MAX;
	float nDist = 0;
	Entity* target = nullptr;
	Vector2 screenPos;

	for (int i = 0; i < *offsets::numOfPlayers; i++) {
		Vector2 screenRes = GL::GetRes();

		if (cfg::bVisibleAim) {
			if (isValidEntity(offsets::entList->ents[i]) && offsets::entList->ents[i]->Health > 0 && isTeamGamemode() && offsets::entList->ents[i]->Team != offsets::localPlayer->Team && isVisible(entList->ents[i]) || isValidEntity(offsets::entList->ents[i]) && offsets::entList->ents[i]->Health > 0 && !isTeamGamemode() && isVisible(entList->ents[i])) {
				if (WorldToScreen(offsets::entList->ents[i]->HeadPos, screenPos, offsets::vMatix, (int)screenRes.x, (int)screenRes.y)) {
					screenRes.x /= 2;
					screenRes.y /= 2;

					nDist = DistanceVec2(screenPos, screenRes);

					if (nDist < oDist) {
						oDist = nDist;
						target = offsets::entList->ents[i];
					}
				}
			}
		}
		else {
			if (isValidEntity(offsets::entList->ents[i]) && offsets::entList->ents[i]->Health > 0 && isTeamGamemode() && offsets::entList->ents[i]->Team != offsets::localPlayer->Team || isValidEntity(offsets::entList->ents[i]) && offsets::entList->ents[i]->Health > 0 && !isTeamGamemode()) {
				if (WorldToScreen(offsets::entList->ents[i]->HeadPos, screenPos, offsets::vMatix, (int)screenRes.x, (int)screenRes.y)) {
					screenRes.x /= 2;
					screenRes.y /= 2;
					nDist = DistanceVec2(screenPos, screenRes);
					if (nDist < oDist) {
						oDist = nDist;
						target = offsets::entList->ents[i];
					}
				}
			}
		}
	}

	return target;
}
bool WorldToScreen(Vector3 pos, Vector2& screen, glMatrix* matrix, int windowWidth, int windowHeight) {
	Vector4 clipCoords;
	clipCoords.x = pos.x * matrix->matrix[0] + pos.y * matrix->matrix[4] + pos.z * matrix->matrix[8] + matrix->matrix[12];
	clipCoords.y = pos.x * matrix->matrix[1] + pos.y * matrix->matrix[5] + pos.z * matrix->matrix[9] + matrix->matrix[13];
	clipCoords.z = pos.x * matrix->matrix[2] + pos.y * matrix->matrix[6] + pos.z * matrix->matrix[10] + matrix->matrix[14];
	clipCoords.w = pos.x * matrix->matrix[3] + pos.y * matrix->matrix[7] + pos.z * matrix->matrix[11] + matrix->matrix[15];

	if (clipCoords.w < 0.1f)
		return false;

	Vector3 NDC;
	NDC.x = clipCoords.x / clipCoords.w;
	NDC.y = clipCoords.y / clipCoords.w;
	NDC.z = clipCoords.z / clipCoords.w;
	screen.x = (windowWidth / 2 * NDC.x) + (NDC.x + windowWidth / 2);
	screen.y = -(windowHeight / 2 * NDC.y) + (NDC.y + windowHeight / 2);
	return true;
}