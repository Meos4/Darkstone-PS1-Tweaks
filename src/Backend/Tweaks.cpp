#include "Tweaks.hpp"

#include "Backend/CustomCode.hpp"
#include "Backend/Mips.hpp"
#include "Common/DstException.hpp"

#include <type_traits>

Tweaks::Tweaks(std::shared_ptr<Game> game) 
	: m_game(std::move(game))
{
}

Tweaks::HudColorArray Tweaks::hudColor() const
{
	return m_game->executable().read<Tweaks::HudColorArray>(m_game->offset().file.executable.hudColor);
}

void Tweaks::unlockCostumeByDefault() const
{
	auto executable{ m_game->executable() };

	executable.write(m_game->offset().file.executable.chooseClassLoopFn + 0x430, Mips_t(0));
	executable.write(m_game->offset().file.executable.chooseClassLoopFn + 0x558, Mips_t(0));
}

void Tweaks::expandHeroAndLegendShops() const
{
	auto executable{ m_game->executable() };

	const auto li32_difficulty{ Mips::li32(Mips::Register::v0, m_game->offset().game.difficulty) };

	const CustomCode::SetHeroAndLegendBonusShop setHeroAndLegendBonusShopFn
	{
		0x00C31821, // addu v1, a2, v1
		li32_difficulty[0],
		li32_difficulty[1],
		0x8C420000, // lw v0, 0(v0)

		// if is Hero
		0x24080004, // li t0, 4
		0x14480006, // bne v0, t0, +6
		0x24080005, // li t0, 5
		0x8FA80664, // lw t0, 0x664(sp)
		0x2652001C, // addiu s2, 0x1C
		0x25080060, // addiu t0, 0x60
		0x10000006, // b, +6
		0xAFA80664, // sw t0, 0x664(sp)

		// else if is Legend
		0x14480004, // bne v0, t0, +4
		0x8FA80664, // lw t0, 0x664(sp)
		0x2652001C, // addiu s2, 0x1C
		0x25080080, // addiu t0, 0x80
		0xAFA80664, // sw t0, 0x664(sp)

		0x03E00008, // jr ra
		0x00000000  // nop
	};

	const auto setHeroAndLegendBonusShopOffset{ m_game->setHeroAndLegendBonusShopOffset() };

	const std::array<Mips_t, 2> jal_sll_v1_2{ Mips::jal(setHeroAndLegendBonusShopOffset.game), 0x00031880 }; // sll v1, 2

	executable.write(setHeroAndLegendBonusShopOffset.file, setHeroAndLegendBonusShopFn);
	executable.write(m_game->offset().file.executable.generateShopFn + 0xC4, jal_sll_v1_2);
}

void Tweaks::permanentShopsItems() const
{
	m_game->executable().write(m_game->offset().file.executable.buyShopFn + 0x1B8, Mips_t(0));
}

void Tweaks::spellDurability3Stacks() const
{
	using Buff_t = u32;

	enum : Buff_t
	{
		BUFF_LYCANTHROPY,
		BUFF_STATS,
		BUFF_TIMING
	};

	struct BuffInfo
	{
		u32 locShift;
		u32 structShift;
		Buff_t buff;
	};

	static constexpr std::array<BuffInfo, 14> buffInfos
	{{
		// Lycanthropy
		{ 0x10C, 0x454, BUFF_LYCANTHROPY },
		// Meditation
		{ 0x210, 0x3F4, BUFF_STATS },
		// Concentration
		{ 0x278, 0x3FC, BUFF_STATS },
		// Orientation
		{ 0x2EC, 0x404, BUFF_TIMING },
		// Light
		{ 0x354, 0x3A4, BUFF_TIMING },
		// Haste
		{ 0x3BC, 0x2F4, BUFF_TIMING },
		// Night Vision
		{ 0x41C, 0x2EC, BUFF_TIMING },
		// Ray Of Injury
		{ 0x4D4, 0x394, BUFF_TIMING },
		// Absorption
		{ 0x534, 0x30C, BUFF_TIMING },
		// Invisibility
		{ 0x594, 0x32C, BUFF_TIMING },
		// Reflections
		{ 0x1828, 0x374, BUFF_TIMING },
		// Detection
		{ 0x192C, 0x354, BUFF_TIMING },
		// Detection Wizard
		{ 0x198C, 0x42C, BUFF_TIMING },
		// Berserker
		{ 0x1A88, 0x364, BUFF_TIMING }
	}};

	const auto setSpellDurability3StacksOffset{ m_game->setSpellDurability3StacksOffset() };

	const auto j_function{ Mips::j(CustomCode::SetSpellDurability3Stacks::functionOffset(setSpellDurability3StacksOffset.game)) };

	const auto 
		li32_setPlayerStats{ Mips::li32(Mips::Register::t0, m_game->offset().game.setPlayerStatsFn) },
		li32_setTiming{ Mips::li32(Mips::Register::t0, m_game->offset().game.setTimingFn) };

	const CustomCode::SetSpellDurability3Stacks setSpellDurability3StacksFn
	{
		.lycanthropy =
		{
			0x01334821, // addu t1, s3
			li32_setPlayerStats[0],
			j_function,
			li32_setPlayerStats[1]
		},
		.stats =
		{
			0x01304821, // addu t1, s0
			li32_setPlayerStats[0],
			j_function,
			li32_setPlayerStats[1]
		},
		.timing =
		{
			0x01304821, // addu t1, s0
			li32_setTiming[0],
			j_function,
			li32_setTiming[1]
		},
		.function =
		{
			0x27BDFFF0, // addiu sp, -0x10
			0x8D2A0000, // lw t2, 0(t1)
			0x00405821, // move t3, v0
			0x01425021, // addu t2, v0
			0x000B5840, // sll t3, 1
			0x01625821, // addu t3, v0
			0x014B102B, // sltu v0, t2, t3
			0x14400002, // beqz v0, +2
			0xAFBF0000, // sw ra, 0(sp)
			0x01605021, // move t2, t3
			0x0100F809, // jalr t0
			0xAD2A0000, // sw t2, 0(t1)
			0x8FBF0000, // lw ra, 0(sp)
			0x00000000, // nop
			0x03E00008, // jr ra
			0x27BD0010  // addiu sp, 0x10
		},
	};

	auto executable{ m_game->executable() };

	executable.write(setSpellDurability3StacksOffset.file, setSpellDurability3StacksFn);

	const auto
		lycanthropyOffset{ CustomCode::SetSpellDurability3Stacks::lycanthropyOffset(setSpellDurability3StacksOffset.game) },
		statsOffset{ CustomCode::SetSpellDurability3Stacks::statsOffset(setSpellDurability3StacksOffset.game) },
		timingOffset{ CustomCode::SetSpellDurability3Stacks::timingOffset(setSpellDurability3StacksOffset.game) };

	for (const auto& info : buffInfos)
	{
		auto buffOffset = [&]()
		{
			switch (info.buff)
			{
			case BUFF_LYCANTHROPY: return lycanthropyOffset;
			case BUFF_STATS: return statsOffset;
			case BUFF_TIMING: return timingOffset;
			default: throw DstException{ "Invalid buff: {}", info.buff };
			}
		};

		executable.write(m_game->offset().file.executable.setPrayerFn + info.locShift, 
			std::array<Mips_t, 2>{ Mips::jal(buffOffset()), Mips::li(Mips::Register::t1, info.structShift) });
	}
}

void Tweaks::talkToNPCsWhileInvisible() const
{
	auto executable{ m_game->executable() };

	// Shop
	executable.write(m_game->offset().file.executable.inGameBehaviorFn + 0x4258, Mips_t(0));
	// NPC
	executable.write(m_game->offset().file.executable.inGameBehaviorFn + 0x42E4, Mips_t(0));
}

void Tweaks::hudColor(const Tweaks::HudColorArray& hud) const
{
	m_game->executable().write(m_game->offset().file.executable.hudColor, hud);
}

void Tweaks::theftBlock() const
{
	auto executable{ m_game->executable() };

	executable.write(m_game->offset().file.executable.resetVariableNewMapFn + 0x20, Mips_t(0xAF800154)); // sw zero, 0x154(gp)
}

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

	const CustomCode::GenerateJewelryBonus generateJewelryBonusFn
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

	// Mini Map
	static constexpr s16 miniMapSpeed{ 12 / 2 };

	executable.write(m_game->offset().file.executable.inGameMenuLoopFn + 0x1E24, Mips::li(Mips::Register::s2, miniMapSpeed));
	executable.write(m_game->offset().file.executable.inGameMenuLoopFn + 0x1E54, Mips::li(Mips::Register::s2, -miniMapSpeed));
	executable.write(m_game->offset().file.executable.inGameMenuLoopFn + 0x1E7C, Mips::li(Mips::Register::s5, miniMapSpeed));
	executable.write(m_game->offset().file.executable.inGameMenuLoopFn + 0x1EAC, Mips::li(Mips::Register::s5, -miniMapSpeed));

	executable.write(m_game->offset().file.executable.drawMiniMapFn + 0xE8, Mips_t(0));
	executable.write(m_game->offset().file.executable.drawMiniMapFn + 0x138, Mips_t(0));
}