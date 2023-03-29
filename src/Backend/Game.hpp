#pragma once

#include "Backend/Offset.hpp"
#include "Backend/Version.hpp"
#include "Common/RawFile.hpp"
#include "Common/TemplateTypes.hpp"
#include "Common/Types.hpp"

#include <filesystem>
#include <memory>
#include <optional>
#include <unordered_map>

class Game final
{
public:
	struct CustomCodeOffset
	{
		u32 file;
		u32 game;
	};

	static constexpr auto sectorSize{ 2048u };

	Game(const std::filesystem::path& isoPath, Version version);

	std::filesystem::path filePath(s32 file) const;

	std::unique_ptr<RawFile> file(s32 file) const;
	RawFile executable() const;

	template <SameAs<Version>... Args>
	bool isVersion(Args... versions) const
	{
		return ((m_version == versions) || ...);
	}

	const char* versionText() const;
	const char* serialText() const;
	std::filesystem::path isoPath() const;
	std::filesystem::path isoFilename() const;
	Version version() const;
	const Offset& offset() const;

	void setIsoPath(const std::filesystem::path& isoPath);

	CustomCodeOffset setTheftJewelryBonusOffset() const;
	CustomCodeOffset setHeroAndLegendBonusShopOffset() const;
	CustomCodeOffset setSpellDurability3StacksOffset() const;

	static [[nodiscard]] std::optional<Version> isAValidIso(const std::filesystem::path& isoPath);
private:
	struct VersionSerialText
	{
		const char* version;
		const char* serial;
	};

	std::filesystem::path m_isoPath;
	Version m_version;
	Offset m_offset;
	static const std::unordered_map<Version, VersionSerialText> s_versionSerial;
};