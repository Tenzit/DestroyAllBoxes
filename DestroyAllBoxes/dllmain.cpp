// dllmain.cpp : Defines the entry point for the DLL application.
#include "pch.h"

#pragma pack(1)
struct DestroyThings {
	uint32_t item1;
	uint32_t item2;
};

union MovInst {
	struct {
		uint8_t opcode;
		uint32_t addr;
	};
	uint8_t u8[5];

	MovInst(uint8_t op, uint32_t newAddr) {
		opcode = op;
		addr = newAddr;
	}
};

union MovC7Inst {
	struct {
		uint8_t opcode;
		uint8_t op2;
		uint8_t modrm;
		uint8_t regoff;
		uint32_t imm32;
	};
	uint8_t u8[8];

	MovC7Inst(uint8_t in_op2, uint8_t in_modrm, uint8_t in_regoff, uint32_t i32) {
		opcode = 0xc7;
		op2 = in_op2;
		modrm = in_modrm;
		regoff = in_regoff;
		imm32 = i32;
	}
};

union Cmp83Inst {
	struct {
		uint8_t opcode;
		uint8_t modrm;
		uint8_t imm;
	};
	uint8_t u8[3];

	Cmp83Inst(uint8_t mrm, uint8_t i8) {
		opcode = 0x83;
		modrm = mrm;
		imm = i8;
	}
};
#pragma pack()

extern "C"
{
	__declspec(dllexport) void __cdecl Init(const char *path, const HelperFunctions &helperFunctions)
	{
		// Executed at startup, contains helperFunctions and the path to your mod (useful for getting the config file.)
		// This is where we override functions, replace static data, etc.

		// Can only do 0x7c because need a multiple of 4 and because inst uses a signed imm8 instead of unsigned imm8
		uint8_t size = 0x7c;

		struct DestroyThings *destroyThingsArr = (struct DestroyThings *)malloc(sizeof(struct DestroyThings) * size);

		for (int i = 0; i < size; i++) {
			destroyThingsArr[i].item1 = 0xffffffff;
			destroyThingsArr[i].item2 = 0x00000000;
		}

		PrintDebug("Addr of array: 0x%x", destroyThingsArr);
		MovInst movInst(0xb8, (uint32_t)destroyThingsArr);
		Cmp83Inst cmpInst(0xf9, size);
		//AddToDestroyArray function
		WriteData((void *)0x6ce462, movInst.u8);
		WriteData((void *)0x6ce471, cmpInst.u8);

		//FUN_0069a770
		WriteData((void *)0x69a793, movInst.u8);
		WriteData((void *)0x69a7a1, cmpInst.u8);

		//FUN_0069c300
		WriteData((void *)0x69c311, movInst.u8);
		WriteData((void *)0x69c31f, cmpInst.u8);

		//FUN_006ce490
		WriteData((void *)0x6ce492, movInst.u8);
		WriteData((void *)0x6ce4a0, cmpInst.u8);

		//POISON_delete
		WriteData((void *)0x7772fa, movInst.u8);
		WriteData((void *)0x777309, cmpInst.u8);

		//Weird offset MOV inst
		MovInst offMovInst_2_0(0xb8, (uint32_t)&destroyThingsArr[2]);
		MovInst offMovInst_0_1(0xb8, (uint32_t)&destroyThingsArr[0].item2);
		MovInst offMovInst_1_1(0xb9, (uint32_t)&destroyThingsArr[1].item2);

		MovC7Inst offCmpInst_2_0(0x44, 0x24, 0x20, (uint32_t)(size >> 2));
		MovInst offCmpInst_1_1(0xba, (uint32_t)(size >> 2));

		//FUN_006a10a0
		// MISSILE/RMISSILE update in Iron Gate
		WriteData((void *)0x6a12a4, offMovInst_2_0.u8);
		WriteData((void *)0x6a12a9, offCmpInst_2_0.u8);

		WriteData((void *)0x6a1189, offMovInst_2_0.u8);
		WriteData((void *)0x6a118e, offCmpInst_2_0.u8);

		WriteData((void *)0x6a1a90, offMovInst_0_1.u8);
		WriteData((void *)0x6a1a55, offCmpInst_1_1.u8);

		WriteData((void *)0x6a1dde, offMovInst_1_1.u8);
		WriteData((void *)0x6a1de3, offCmpInst_1_1.u8);
		
		//ROCKETMISSILE_update
		WriteData((void *)0x6ce982, offMovInst_2_0.u8);
		MovC7Inst offCmpInst_2_0_off2c(0x44, 0x24, 0x2c, (uint32_t)(size >> 2));
		WriteData((void *)0x6ce987, offCmpInst_2_0_off2c.u8);

		WriteData((void *)0x6cf2a8, offMovInst_0_1.u8);
		WriteData((void *)0x6cf28a, offCmpInst_1_1.u8);

		WriteData((void *)0x6cf41b, offMovInst_1_1.u8);
		WriteData((void *)0x6cf420, offCmpInst_1_1.u8);
	}

	__declspec(dllexport) ModInfo SA2ModInfo = { ModLoaderVer }; // This is needed for the Mod Loader to recognize the DLL.
}
