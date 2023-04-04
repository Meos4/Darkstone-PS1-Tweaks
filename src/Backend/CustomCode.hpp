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

	struct SetSpellDurability3Stacks
	{
		std::array<Mips_t, 4> lycanthropy;
		std::array<Mips_t, 4> stats;
		std::array<Mips_t, 4> timing;
		std::array<Mips_t, 16> function;

		static constexpr u32 lycanthropyOffset(u32 begin) { return begin; }
		static constexpr u32 statsOffset(u32 begin) { return lycanthropyOffset(begin) + sizeof(lycanthropy); };
		static constexpr u32 timingOffset(u32 begin) { return statsOffset(begin) + sizeof(stats); };
		static constexpr u32 functionOffset(u32 begin) { return timingOffset(begin) + sizeof(timing); };
	};

	using DivideXpBarSizeBy10 = std::array<Mips_t, 6>;
}