#include "Offset.hpp"

#include "Common/DstException.hpp"

#include <type_traits>

static constexpr Offset::File
fileNtscU
{
	.executable
	{

	},
},
filePal
{
	.executable
	{

	},
},
filePalEn
{
	.executable
	{

	},
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