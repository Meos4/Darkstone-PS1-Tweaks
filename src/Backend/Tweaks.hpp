#pragma once

#include "Backend/Game.hpp"
#include "Common/Types.hpp"

#include <memory>

class Tweaks final
{
public:
	struct Color
	{
		u8 red;
		u8 green;
		u8 blue;
	};

	using HudColorArray = std::array<Tweaks::Color, 8>;

	Tweaks(std::shared_ptr<Game> game);

	Tweaks::HudColorArray hudColor() const;

	void unlockCostumeByDefault() const;
	void expandHeroAndLegendShops() const;
	void legendDifficultyRequirement60To50() const;
	void permanentShopsItems() const;
	void spellDurability3Stacks() const;
	void talkToNPCsWhileInvisible() const;
	void hudColor(const Tweaks::HudColorArray& hud) const;
	void theftBlock() const;
	void theftEmptyJewelry() const;
	void framerate60() const;
private:
	std::shared_ptr<Game> m_game;
};