#pragma once

#include "Backend/Mips.hpp"
#include "Common/Types.hpp"

#include <array>

namespace CustomCode
{
	struct GenerateJewelryBonus
	{
		std::array<char, 7> xamuletStr;
		std::array<char, 5> xringStr;
		std::array<Mips_t, 60> function;

		static constexpr u32 xamuletStrOffset(u32 begin) { return begin; };
		static constexpr u32 xringStrOffset(u32 begin) { return xamuletStrOffset(begin) + sizeof(xamuletStr); };
		static constexpr u32 functionOffset(u32 begin) { return xringStrOffset(begin) + sizeof(xringStr); };
	};

	using SetHeroAndLegendBonusShop = std::array<Mips_t, 19>;
}