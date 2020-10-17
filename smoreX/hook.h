#pragma once

bool detour32(BYTE* src, BYTE* dst, const uintptr_t len);
BYTE* tramphook32(BYTE* src, BYTE* dst, const uintptr_t len);

struct hook {
	bool bStatus{ false };
	BYTE* src{ nullptr };
	BYTE* dst{ nullptr };
	BYTE* PtrToGatewayFnPtr{ nullptr };
	uintptr_t len{ 0 };
	BYTE originalBytes[10]{ 0 };
	hook(BYTE* src, BYTE* dst, BYTE* PtrToGatewayPtr, uintptr_t len);
	hook(const char* exportName, const char* modName, BYTE* dst, BYTE* PtrToGateWayPtr, uintptr_t len);
	void enable();
	void disable();
};

