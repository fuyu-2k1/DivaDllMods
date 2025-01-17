#include "Config.h"

// v1.01: 0x1416E3A59
// v1.02: 0x1416E2BA9
bool* pvMode;

// v1.01: 0x140C8C688
// v1.02: 0x140C8B6C0
SIG_SCAN(sigPVMark, 0x140C8B6C0, "pv_mark", "xxxxxxx");

// v1.01: 0x140CBDF50
// v1.02: 0x140CBCF20
SIG_SCAN(sigCopyright, 0x140CBCF20, "rom/copyright.farc", "xxxxxxxxxxxxxxxxxx");

// v1.01: 0x1402454C0
// v1.02: 0x140245350
SIG_SCAN
(
	sigLyrics,
	0x140245350,
	"\x40\x53\x48\x83\xEC\x20\x48\x8B\xD9\xE8\x00\x00\x00\x00\x80\xBB\x00\x00\x00\x00\x00",
	"xxxxxxxxxx????xx?????"
);

// v1.01: 0x140602FB0
// v1.02: 0x140601740
SIG_SCAN
(
	sigScreenshot,
	0x140601740,
	"\x48\x89\x5C\x24\x00\x48\x89\x4C\x24\x00\x57\x48\x83\xEC\x20\x48\x8B\xD9\x48\x8D\x05\x00\x00\x00\x00\x48\x89\x01\x33\xFF\x89\x79\x08\x48\x89\x79\x10\x48\x89\x79\x18\x48\x83\xC1\x20",
	"xxxx?xxxx?xxxxxxxxxxx????xxxxxxxxxxxxxxxxxxxx"
);

// v1.01: 0x1406F3E20
// v1.02: 0x1406F2820
SIG_SCAN
(
	sigGameMode,
	0x1406F2820,
	"\x48\x89\x5C\x24\x00\x55\x56\x57\x41\x54\x41\x55\x48\x8D\x6C\x24\x00\x48\x81\xEC\x00\x00\x00\x00\x48\x8B\x05\x00\x00\x00\x00\x48\x33\xC4\x48\x89\x45\x70\x48\x8B\xD9\x48\x89\x4C\x24\x00\x48\x8D\x0D\x00\x00\x00\x00\xE8\x00\x00\x00\x00\x41\xBC\x00\x00\x00\x00\x48\x8D\x55\xD8\x45\x8B\xC4\x4C\x8B\xE8\x66\x0F\x1F\x44\x00\x00",
	"xxxx?xxxxxxxxxxx?xxx????xxx????xxxxxxxxxxxxxx?xxx????x????xx????xxxxxxxxxxxxxxx?"
)

// v1.01: 0x14040B370
// v1.02: 0x14040B270
SIG_SCAN
(
	sigGetPVMode,
	0x14040B270,
	"\x48\x89\x5C\x24\x00\x48\x89\x6C\x24\x00\x48\x89\x74\x24\x00\x57\x48\x83\xEC\x20\x33\xED\x48\xC7\x41\x00\x00\x00\x00\x00\x48\x8B\xF9\x48\x89\x29\x48\x89\x69\x08\x48\x8D\x99\x00\x00\x00\x00\x40\x88\x69\x10\x8D\x75\x04\x48\x89\x69\x14\x66\x89\x69\x1C\x89\x69\x28\xC7\x41\x00\x00\x00\x00\x00",
	"xxxx?xxxx?xxxx?xxxxxxxxxx?????xxxxxxxxxxxxx????xxxxxxxxxxxxxxxxxxxx?????"
)

// v1.01: 0x14065DD90
// v1.02: 0x14065C520
SIG_SCAN
(
	sigPhotoMode,
	0x14065C520,
	"\x48\x83\xEC\x28\xE8\x00\x00\x00\x00\x48\x85\xC0\x74\x26",
	"xxxxx????xxxxx"
)

HOOK(void, __fastcall, _SetGameMode, sigGameMode(), __int64 a1)
{
	original_SetGameMode(a1);

	WRITE_MEMORY((char*)sigLyrics() + 0x38, bool, *pvMode);
}

extern "C" __declspec(dllexport) void Init()
{
	if (!sigValid)
	{
		versionWarning(TEXT("Disable Watermark"));
		return;
	}

	Config::init();

	if (Config::pvMark)
	{
		WRITE_MEMORY(sigPVMark(), uint8_t, 0x00);
	}

	if (Config::copyrightMark == 1) // Redirect to disablewm.farc
	{
		WRITE_MEMORY(sigCopyright(), const char, "rom/disablewm.farc");
	}
	else if (Config::copyrightMark == 2) // Block game from overriding Steam screenshot
	{
		WRITE_MEMORY(sigScreenshot(), uint8_t, 0xC3);
	}

	if (Config::hideLyrics)
	{
		WRITE_MEMORY((char*)sigPhotoMode() + 0x2A, bool, true);

		uint8_t* instrAddr = (uint8_t*)sigGetPVMode() - 0x10;
		pvMode = (bool*)(instrAddr + readUnalignedU32(instrAddr + 0x3) + 0x20);
		printf("[Disable Watermarks] pvMode: 0x%llx\n", pvMode);
		INSTALL_HOOK(_SetGameMode);
	}
}
