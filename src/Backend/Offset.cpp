#include "Offset.hpp"

#include "Common/DstException.hpp"

#include <type_traits>

static constexpr Offset::File
fileNtscU
{
	.executable
	{
		.initInGameMenuLoopFn = 0x00057E48,
		.inGameMenuLoopFn = 0x00051154,
		.drawOnScreenFn = 0x00008A70,
		.cameraSpeedRotateXPlusFn = 0x000662FC,
		.cameraSpeedRotateXMinusFn = 0x00066150,
		.cameraSpeedRotateYPlusFn = 0x00066678,
		.cameraSpeedRotateYMinusFn = 0x000664A8,
		.mainLoopFn = 0x00009464,
		.initVariableFn = 0x00008824,
		.chooseClassLoopFn = 0x00078D2C,
		.startFn = 0x000B3504,
		.resetVariableNewMapFn = 0x0000E760,
		.generateTheftItemFn = 0x0008701C,
		.hudColor = 0x000D5790,
		.generateShopFn = 0x000AE5EC,
		.drawMiniMapFn = 0x000366A0,
		.buyShopFn = 0x000AFB98,
		.setPrayerFn = 0x00099168,
		.inGameBehaviorFn = 0x0008E0DC,
		.startDifficultyMenuFn = 0x000759EC,
		.drawHudFn = 0x0004DCEC,
		.overrideCharacterFn = 0x00011A3C,
		.inventoryLoopFn = 0x00057E48,
		.cc_begin = 0x001206D0
	},
	.launcher
	{
		.VSyncFn = 0x000077FC
	}
},
filePal
{
	.executable
	{
		.initInGameMenuLoopFn = 0x000581DC,
		.inGameMenuLoopFn = 0x000514F0,
		.drawOnScreenFn = 0x00008AD4,
		.cameraSpeedRotateXPlusFn = 0x00066884,
		.cameraSpeedRotateXMinusFn = 0x000666D8,
		.cameraSpeedRotateYPlusFn = 0x00066C00,
		.cameraSpeedRotateYMinusFn = 0x00066A30,
		.mainLoopFn = 0x000094C8,
		.initVariableFn = 0x00008874,
		.chooseClassLoopFn = 0x00079314,
		.startFn = 0x000B3ABC,
		.resetVariableNewMapFn = 0x0000E7D0,
		.generateTheftItemFn = 0x00087604,
		.hudColor = 0x000D5D48,
		.generateShopFn = 0x000AEBDC,
		.drawMiniMapFn = 0x000369BC,
		.buyShopFn = 0x000B0188,
		.setPrayerFn = 0x0009975C,
		.inGameBehaviorFn = 0x0008E6D0,
		.startDifficultyMenuFn = 0x00075FD4,
		.drawHudFn = 0x0004E088,
		.overrideCharacterFn = 0x00011AAC,
		.inventoryLoopFn = 0x000581DC,
		.cc_begin = 0x00120CD0
	},
	.launcher
	{
		.VSyncFn = 0x00007C24
	}
},
filePalAustralia
{
	.executable
	{
		.initInGameMenuLoopFn = 0x0005813C,
		.inGameMenuLoopFn = 0x00051450,
		.drawOnScreenFn = 0x00008ABC,
		.cameraSpeedRotateXPlusFn = 0x000667E4,
		.cameraSpeedRotateXMinusFn = 0x00066638,
		.cameraSpeedRotateYPlusFn = 0x00066B60,
		.cameraSpeedRotateYMinusFn = 0x00066990,
		.mainLoopFn = 0x000094B0,
		.initVariableFn = 0x0000885C,
		.chooseClassLoopFn = 0x00079274,
		.startFn = 0x000B3A1C,
		.resetVariableNewMapFn = 0x0000E7B8,
		.generateTheftItemFn = 0x00087564,
		.hudColor = 0x000D5CA8,
		.generateShopFn = 0x000AEB3C,
		.drawMiniMapFn = 0x000369A4,
		.buyShopFn = 0x000B00E8,
		.setPrayerFn = 0x000996BC,
		.inGameBehaviorFn = 0x0008E630,
		.startDifficultyMenuFn = 0x00075F34,
		.drawHudFn = 0x0004DFE8,
		.overrideCharacterFn = 0x00011A94,
		.inventoryLoopFn = 0x0005813C,
		.cc_begin = 0x00120C30
	},
	.launcher
	{
		.VSyncFn = 0x000077FC
	}
};

static constexpr Offset::Game
gameNtscU
{
	.setItemBonusFn = 0x8004A950,
	.difficulty = 0x800E2FB4,
	.getLevelPoolIdFn = 0x80043530,
	.mapId = 0x80103CFC,
	.mapInformations = 0x800EEBB8,
	.setPlayerStatsFn = 0x800A527C,
	.setTimingFn = 0x8007B858,
	.baseStatsPtr = 0x800E8478,
	.cc_begin = 0x8012FEF0
},
gamePal
{
	.setItemBonusFn = 0x8004AC6C,
	.difficulty = 0x800E356C,
	.getLevelPoolIdFn = 0x80043850,
	.mapId = 0x801042FC,
	.mapInformations = 0x800EF1B8,
	.setPlayerStatsFn = 0x800A5870,
	.setTimingFn = 0x8007BDE0,
	.baseStatsPtr = 0x800E8A50,
	.cc_begin = 0x801304F0
},
gamePalAustralia
{
	.setItemBonusFn = 0x8004AC54,
	.difficulty = 0x800E34CC,
	.getLevelPoolIdFn = 0x80043838,
	.mapId = 0x8010425C,
	.mapInformations = 0x800EF118,
	.setPlayerStatsFn = 0x800A57D0,
	.setTimingFn = 0x8007BD40,
	.baseStatsPtr = 0x800E89B0,
	.cc_begin = 0x80130450
};

Offset::Offset(Version version)
	: file(setFile(version)), game(setGame(version))
{
}

const Offset::File& Offset::setFile(Version version)
{
	switch (version)
	{
	case Version::NtscU: return fileNtscU;
	case Version::Pal: return filePal;
	case Version::PalAustralia: return filePalAustralia;
	default: throw DstException{ "Invalid game version : {}", static_cast<std::underlying_type_t<Version>>(version) };
	}
}

const Offset::Game& Offset::setGame(Version version)
{
	switch (version)
	{
	case Version::NtscU: return gameNtscU;
	case Version::Pal: return gamePal;
	case Version::PalAustralia: return gamePalAustralia;
	default: throw DstException{ "Invalid game version : {}", static_cast<std::underlying_type_t<Version>>(version) };
	}
}