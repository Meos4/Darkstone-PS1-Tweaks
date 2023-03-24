#include "Tweaks.hpp"

#include "Backend/CustomCode.hpp"
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

//.id = 
//{
//	// ITEM_XAMULET
//	0x00DF, 0x00E0, 0x00E1, 0x00E2,
//	0x00E3, 0x00E4, 0x00E5, 0x00E6,
//	0x00E7, 0x00E8,
//	// ITEM_XRING
//	0x01B0, 0x01B1, 0x01B2, 0x01B3,
//	0x01B4, 0x01B5, 0x01B6, 0x01B7,
//	0x01B8,
//	// 4 Bytes Padding
//	0x0000
//},

void Tweaks::theftEmptyJewelry() const
{
	auto executable{ m_game->executable() };

	const auto generateJewelryBonusOffset{ m_game->setTheftJewelryBonusOffset() };

	const auto 
		li32_mapId{ Mips::li32(Mips::Register::a1, m_game->offset().game.mapId) },
		li32_mapInformations{ Mips::li32(Mips::Register::a1, m_game->offset().game.mapInformations) },
		li32_difficulty{ Mips::li32(Mips::Register::a2, m_game->offset().game.difficulty) },
		li32_xamuletStr{ Mips::li32(Mips::Register::a0, CustomCode::GenerateJewelryBonus::xamuletStrOffset(generateJewelryBonusOffset.game)) },
		li32_xringStr{ Mips::li32(Mips::Register::a0, CustomCode::GenerateJewelryBonus::xringStrOffset(generateJewelryBonusOffset.game)) };

	CustomCode::GenerateJewelryBonus generateJewelryBonusFn
	{
		.xamuletStr = { 0x58, 0x41, 0x4D, 0x55, 0x4C, 0x45, 0x54 },
		.xringStr = { 0x58, 0x52, 0x49, 0x4E, 0x47 },
		.function =
		{
			0x80430000, // lb v1, 0(v0)
			0x27BDFFE0, // addiu sp, -0x20
			0xAFA40000, // sw a0, 0(sp)
			0xAFA50004, // sw a1, 4(sp)
			0xAFA60008, // sw a2, 8(sp)
			0xAFA7000C, // sw a3, 0xC(sp)
			0xAFA20010, // sw v0, 0x10(sp)
			0xAFA30014, // sw v1, 0x14(sp)
			0xAFBF0018, // sw ra, 0x18(sp)

			// If is an amulet
			0x27A50038, // addiu a1, sp, 0x38
			0x8CA50000, // lw a1, 0(a1)
			0x24060007, // li a2, 7
			0x00003821, // move a3, zero
			li32_xamuletStr[0],
			li32_xamuletStr[1],
			0x90A20005, // lbu v0, 5(a1)
			0x90830000, // lbu v1, 0(a0)
			0x24840001, // addiu a0, 1
			0x14430004, // bne v0, v1, 4
			0x24E70001, // addiu a3, 1
			0x14E6FFFA, // bne a3, a2, -5
			0x24A50001, // addiu a1, 1
			0x1000000D, // b +0xD

			// Or a ring
			0x27A50038, // addiu a1, sp, 0x38
			0x8CA50000, // lw a1, 0(a1)
			0x24060005, // li a2, 5
			0x00003821, // move a3, zero
			li32_xringStr[0],
			li32_xringStr[1],
			0x90A20005, // lbu v0, 5(a1)
			0x90830000, // lbu v1, 0(a0)
			0x24840001, // addiu a0, 1
			0x14430012, // bne v0, v1, 0x12
			0x24E70001, // addiu a3, 1
			0x14E6FFFA, // bne a3, a2, -5
			0x24A50001, // addiu a1, 1

			// Generate bonus
			li32_mapId[0],
			li32_mapId[1],
			Mips::jal(m_game->offset().game.getLevelPoolIdFn),
			0x8CA50000, // lw a1, 0(a1)
			li32_mapInformations[0],
			li32_mapInformations[1],
			0x27A50038, // addiu a1, sp, 0x38
			li32_difficulty[0],
			li32_difficulty[1],
			0x8CC60000, // lw a2, 0(a2)
			0x24070001, // li a3, 1
			0x24C6FFFF, // addiu a2, -1
			0x00063140, // sll a2, 5
			Mips::jal(m_game->offset().game.setItemBonusFn),
			0x00463021, // addu a2, v0, a2

			0x8FBF0018, // lw ra, 0x18(sp)
			0x8FA30014, // lw v1, 0x14(sp)
			0x8FA20010, // lw v0, 0x10(sp)
			0x8FA7000C, // lw a3, 0xC(sp)
			0x8FA60008, // lw a2, 8(sp)
			0x8FA50004, // lw a1, 4(sp)
			0x8FA40000, // lw a0, 0(sp)
			0x03E00008, // jr ra
			0x27BD0020  // addiu sp, 0x20
		}
	};

	executable.write(generateJewelryBonusOffset.file, generateJewelryBonusFn);
	executable.write(m_game->offset().file.executable.generateTheftItemFn + 0x394,
		Mips::jal(CustomCode::GenerateJewelryBonus::functionOffset(generateJewelryBonusOffset.game)));
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