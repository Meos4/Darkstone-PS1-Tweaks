#include "Offset.hpp"

#include "Common/DstException.hpp"

#include <type_traits>

static constexpr Offset::File
fileNtscU
{
	.executable
	{

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
		.initVariableFn = 0x00008874
	}
},
filePalEn
{
	.executable
	{

	}
};

static constexpr Offset::Game
gameNtscU
{
	
},
gamePal
{

},
gamePalEn
{

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
	case Version::PalEn: return filePalEn;
	default: throw DstException{ "Invalid game version : {}", static_cast<std::underlying_type_t<Version>>(version) };
	}
}

const Offset::Game& Offset::setGame(Version version)
{
	switch (version)
	{
	case Version::NtscU: return gameNtscU;
	case Version::Pal: return gamePal;
	case Version::PalEn: return gamePalEn;
	default: throw DstException{ "Invalid game version : {}", static_cast<std::underlying_type_t<Version>>(version) };
	}
}