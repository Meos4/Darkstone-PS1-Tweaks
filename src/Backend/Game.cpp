#include "Game.hpp"

#include "Backend/File.hpp"
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