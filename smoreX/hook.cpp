#include <Windows.h>
#include "helper.h"
#include "hook.h"

bool detour32(BYTE* src, BYTE* dst, const uintptr_t len) {
	if (len < 5) { return false; }
	DWORD curProtection;
	VirtualProtect(src, len, PAGE_EXECUTE_READWRITE, &curProtection);
	uintptr_t relativeAddress = (uintptr_t)dst - (uintptr_t)src - 5;
	*(uintptr_t*)src = 0xE9;
	*(uintptr_t*)((uintptr_t)src + 1) = relativeAddress;
	VirtualProtect(src, len, curProtection, &curProtection);
	return true;
}

BYTE* tramphook32(BYTE* src, BYTE* dst, const uintptr_t len) {
	if (len < 5) return 0;
	BYTE* gateway = (BYTE*)VirtualAlloc(0, len, MEM_COMMIT | MEM_RESERVE, PAGE_EXECUTE_READWRITE);
	memcpy_s(gateway, len, src, len);
	uintptr_t gateWayRelativeAddress = src - gateway - 5;
	*(gateway + len) = 0xE9;
	*(uintptr_t*)((uintptr_t)gateway + len + 1) = gateWayRelativeAddress;
	detour32(src, dst, len);
	return gateway;
}

hook::hook(BYTE* src, BYTE* dst, BYTE* PtrToGatewayPtr, uintptr_t len) {
	this->src = src;
	this->dst = dst;
	this->len = len;
	this->PtrToGatewayFnPtr = PtrToGatewayPtr;
}

hook::hook(const char* exportName, const char* modName, BYTE* dst, BYTE* PtrToGatewayPtr, uintptr_t len) {
	HMODULE hMod = GetModuleHandleA(modName);

	this->src = (BYTE*)GetProcAddress(hMod, exportName);
	this->dst = dst;
	this->len = len;
	this->PtrToGatewayFnPtr = PtrToGatewayPtr;
}

void hook::enable() {
	memcpy(originalBytes, src, len);
	*(uintptr_t*)PtrToGatewayFnPtr = (uintptr_t)tramphook32(src, dst, len);
	bStatus = true;
}

void hook::disable() {
	patch(src, originalBytes, len);
	bStatus = false;
}