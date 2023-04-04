#include "Game.hpp"

#include "Backend/CustomCode.hpp"
#include "Backend/File.hpp"
#include "Backend/Mips.hpp"
#include "Backend/Path.hpp"
#include "Common/Buffer.hpp"
#include "Common/DstException.hpp"

#include <array>
#include <format>
#include <type_traits>

const std::unordered_map<Version, Game::VersionSerialText> Game::s_versionSerial
{
	{ Version::NtscU,	{ "NTSC-U", "SLUS-01182" } },
	{ Version::Pal,		{ "PAL", "SLES-00612" } },
	{ Version::PalEn,	{ "PAL-EN", "SLES-03447" } }
};

Game::Game(const std::filesystem::path& isoPath, Version version)
	: m_isoPath(isoPath), m_version(version), m_offset(version)
{
	// Expand Executable
	auto executable{ this->executable() };

	auto
		luiBeginHeap{ executable.read<Mips_t>(offset().file.executable.startFn + 0x18) },
		addiuBeginHeap{ executable.read<Mips_t>(offset().file.executable.startFn + 0x1C) };

	u32 newBeginHeap{ (static_cast<u16>(luiBeginHeap) << 16) + static_cast<u16>(addiuBeginHeap) + Game::sectorSize };
	const u32 rest{ newBeginHeap % Game::sectorSize };
	if (rest)
	{
		newBeginHeap += Game::sectorSize - rest;
	}

	const u32 newExecutableSize{ (newBeginHeap + Game::sectorSize - 0x10000) & ~0x80000000 };

	if (newExecutableSize > 0x00200000 + Game::sectorSize)
	{
		throw DstException{ "New executable size exceed limit: {}", newExecutableSize };
	}
	else if (executable.size() != newExecutableSize)
	{
		executable.write(0x1C, (newBeginHeap - 0x10000) & ~0x80000000);
		std::filesystem::resize_file(filePath(File::DRAGON_B_EXE), newExecutableSize);

		const bool isRightPartPositive{ static_cast<s16>(newBeginHeap) >= 0 };

		luiBeginHeap = ((luiBeginHeap >> 16) << 16);
		luiBeginHeap += isRightPartPositive ? (newBeginHeap >> 16) : (newBeginHeap >> 16) + 1;
		addiuBeginHeap = ((addiuBeginHeap >> 16) << 16) + static_cast<u16>(newBeginHeap);

		executable.write(offset().file.executable.startFn + 0x54, luiBeginHeap + 0x10000);
		executable.write(offset().file.executable.startFn + 0x58, addiuBeginHeap + 0x210000);
	}
}

std::filesystem::path Game::filePath(s32 file) const
{
	const std::filesystem::path path{ std::format("{}/{}/{}", Path::dstTempDirectory, Path::filesDirectory, File::names[file]) };
	if (!std::filesystem::is_regular_file(path))
	{
		throw DstException{ "\"{}\" file doesn't exist in \"{}\"", path.filename().string(), path.parent_path().string() };
	}
	return path;
}

std::unique_ptr<RawFile> Game::file(s32 file) const
{
	return std::make_unique<RawFile>(Game::filePath(file));
}

RawFile Game::executable() const
{
	return RawFile{ Game::filePath(File::DRAGON_B_EXE) };
}

const char* Game::versionText() const
{
	return s_versionSerial.at(m_version).version;
}

const char* Game::serialText() const
{
	return s_versionSerial.at(m_version).serial;
}

std::filesystem::path Game::isoPath() const
{
	return m_isoPath;
}

std::filesystem::path Game::isoFilename() const
{
	return m_isoPath.filename();
}

Version Game::version() const
{
	return m_version;
}

const Offset& Game::offset() const
{
	return m_offset;
}

void Game::setIsoPath(const std::filesystem::path& isoPath)
{
	m_isoPath = isoPath;
}

Game::CustomCodeOffset Game::setTheftJewelryBonusOffset() const
{
	CustomCodeOffset cc
	{
		.file = offset().file.executable.cc_begin,
		.game = offset().game.cc_begin
	};

	return cc;
}

Game::CustomCodeOffset Game::setHeroAndLegendBonusShopOffset() const
{
	return setTheftJewelryBonusOffset() += sizeof(CustomCode::GenerateJewelryBonus);
}

Game::CustomCodeOffset Game::setSpellDurability3StacksOffset() const
{
	return setHeroAndLegendBonusShopOffset() += sizeof(CustomCode::SetHeroAndLegendBonusShop);
}

Game::CustomCodeOffset Game::divideXpBarSizeBy10Offset() const
{
	return setSpellDurability3StacksOffset() += sizeof(CustomCode::SetSpellDurability3Stacks);
}

Game::CustomCodeOffset Game::checkCharacterLevelToSaveOffset() const
{
	return divideXpBarSizeBy10Offset() += sizeof(CustomCode::DivideXpBarSizeBy10);
}

std::optional<Version> Game::isAValidIso(const std::filesystem::path& isoPath)
{
	if (!std::filesystem::is_regular_file(isoPath))
	{
		return std::nullopt;
	}

	RawFile iso{ isoPath };

	using OffsetPattern = std::pair<u32, SBuffer<11>>;
	static constexpr std::array<OffsetPattern, static_cast<std::size_t>(Version::Count)> ver
	{{
		{ 0x0000D374, { 0x53, 0x4C, 0x55, 0x53, 0x5F, 0x30, 0x31, 0x31, 0x2E, 0x38, 0x32 } },
		{ 0x0000D374, { 0x53, 0x4C, 0x45, 0x53, 0x5F, 0x30, 0x30, 0x36, 0x2E, 0x31, 0x32 } },
		{ 0x0000D374, { 0x53, 0x4C, 0x45, 0x53, 0x5F, 0x30, 0x33, 0x34, 0x2E, 0x34, 0x37 } },
	}};

	static constexpr auto maxOffset{ (*std::max_element(ver.begin(), ver.end(),
		[](const auto& a, const auto& b) { return a.first < b.first; })).first };

	if (iso.size() < maxOffset + sizeof(OffsetPattern::second))
	{
		return std::nullopt;
	}

	for (std::size_t i{}; i < ver.size(); ++i)
	{
		if (iso.read<std::remove_const_t<decltype(ver[i].second)>>(ver[i].first) == ver[i].second)
		{
			return static_cast<Version>(i);
		}
	}

	return std::nullopt;
}