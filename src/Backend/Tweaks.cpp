#include "Tweaks.hpp"

#include <type_traits>

Tweaks::Tweaks(std::shared_ptr<Game> game) 
	: m_game(std::move(game))
{
}