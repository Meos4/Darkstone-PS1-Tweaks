#pragma once

#include "Backend/Game.hpp"

#include <memory>

class Tweaks final
{
public:
	Tweaks(std::shared_ptr<Game> game);

	void unlockCostumeByDefault() const;
	void theftBlock() const;
	void theftEmptyJewelry() const;
	void framerate60() const;
private:
	std::shared_ptr<Game> m_game;
};