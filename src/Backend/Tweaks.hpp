#pragma once

#include "Backend/Game.hpp"

#include <memory>

class Tweaks final
{
public:
	Tweaks(std::shared_ptr<Game> game);

	void unlockCostumeByDefault() const;
	void framerate60() const;
private:
	std::shared_ptr<Game> m_game;
};