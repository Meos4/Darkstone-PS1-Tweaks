#pragma once

#include "Backend/Game.hpp"

#include <memory>

class Tweaks final
{
public:
	Tweaks(std::shared_ptr<Game> game);
private:
	std::shared_ptr<Game> m_game;
};