#include "Tweaks.hpp"

#include "Backend/Mips.hpp"

#include <type_traits>

Tweaks::Tweaks(std::shared_ptr<Game> game) 
	: m_game(std::move(game))
{
}

void Tweaks::unlockCostumeByDefault() const
{
	auto executable{ m_game->executable() };

	executable.write(m_game->offset().file.executable.chooseClassLoopFn + 0x430, Mips_t(0));
	executable.write(m_game->offset().file.executable.chooseClassLoopFn + 0x558, Mips_t(0));
}

void Tweaks::theftBlock() const
{
	auto executable{ m_game->executable() };

	executable.write(m_game->offset().file.executable.resetVariableNewMapFn + 0x20, Mips_t(0xAF800154)); // sw zero, 0x154(gp)
}

void Tweaks::framerate60() const
{
	auto executable{ m_game->executable() };

	static constexpr auto
		li_a0_4{ Mips::li(Mips::Register::a0, 4) },
		li_a0_5{ Mips::li(Mips::Register::a0, 5) };

	// Inventory, Shop, Pause
	executable.write(m_game->offset().file.executable.initInGameMenuLoopFn + 0x5FB0, li_a0_5);
	// Mini map
	executable.write(m_game->offset().file.executable.initInGameMenuLoopFn + 0x5FB4, li_a0_5);
	// Quit Game Text Box
	executable.write(m_game->offset().file.executable.inGameMenuLoopFn + 0x2670, li_a0_5);
	// Init Menu, Title Screen, Quit Game Text Box
	executable.write(m_game->offset().file.executable.drawOnScreenFn + 0x68, li_a0_5);
	// In Game
	executable.write(m_game->offset().file.executable.drawOnScreenFn + 0x5C, li_a0_4);

	// D-Pad
	static constexpr s16 dpadCameraSpeed{ 80 / 3 };

	static constexpr auto
		li_v1_dpadCameraSpeedPlus{ Mips::li(Mips::Register::v1, dpadCameraSpeed) },
		li_v1_dpadCameraSpeedMinus{ Mips::li(Mips::Register::v1, -dpadCameraSpeed) };

	executable.write(m_game->offset().file.executable.cameraSpeedRotateXPlusFn + 0x14C, li_v1_dpadCameraSpeedPlus);
	executable.write(m_game->offset().file.executable.cameraSpeedRotateXMinusFn + 0x14C, li_v1_dpadCameraSpeedMinus);
	executable.write(m_game->offset().file.executable.cameraSpeedRotateYPlusFn + 0x154, li_v1_dpadCameraSpeedPlus);
	executable.write(m_game->offset().file.executable.cameraSpeedRotateYMinusFn + 0x170, li_v1_dpadCameraSpeedMinus);

	// Right Joystick
	static constexpr s32 joystickCameraSpeed{ 204 * 3 };

	executable.write(m_game->offset().file.executable.initVariableFn + 0x60, Mips::li(Mips::Register::a2, joystickCameraSpeed));
	executable.write(m_game->offset().file.executable.initVariableFn + 0x68, Mips::li(Mips::Register::a3, joystickCameraSpeed));

	// Death Camera
	executable.write(m_game->offset().file.executable.mainLoopFn + 0x408, Mips::li(Mips::Register::t0, 32 / 3));
}