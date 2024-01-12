#pragma once

#include "Backend/CustomCode.hpp"
#include "Backend/Offset.hpp"
#include "Backend/Version.hpp"
#include "Common/RawFile.hpp"
#include "Common/TemplateTypes.hpp"
#include "Common/Types.hpp"

#include <filesystem>
#include <memory>
#include <optional>

class Game final
{
public:
	struct CustomCodeOffset
	{
		u32 file;
		u32 game;

		constexpr auto& operator+=(u32 val)
		{
			file += val;
			game += val;
			return *this;
		}
	};

	static constexpr auto sectorSize{ 2048u };

	Game(const std::filesystem::path& isoPath, Version version);

	static std::optional<Version> isAValidIso(const std::filesystem::path& isoPath);

	std::filesystem::path filePath(s32 file) const;
	std::unique_ptr<RawFile> file(s32 file) const;
	RawFile executable() const;
	RawFile launcherExecutable() const;
	CustomCodeOffset customCodeOffset(CustomCode::Id id) const;
	const char* versionText() const;
	const char* serialText() const;
	std::filesystem::path isoPath() const;
	std::filesystem::path isoFilename() const;

	template <SameAs<Version>... Args>
	bool isVersion(Args... versions) const
	{
		return ((m_version == versions) || ...);
	}

	Version version() const;
	const Offset& offset() const;
	bool isVanilla() const;

	void setIsoPath(const std::filesystem::path& isoPath);
	void setNotVanilla();
private:
	bool m_isVanilla;
	std::filesystem::path m_isoPath;
	Version m_version;
	Offset m_offset;
};