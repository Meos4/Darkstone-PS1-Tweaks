#pragma once

#include "Backend/Offset.hpp"
#include "Backend/Version.hpp"
#include "Common/RawFile.hpp"
#include "Common/TemplateTypes.hpp"

#include <filesystem>
#include <optional>
#include <unordered_map>

class Game final
{
public:
	Game(const std::filesystem::path& isoPath, const std::filesystem::path& exePath, Version version);
	
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

	static [[nodiscard]] std::optional<Version> isAValidIso(const std::filesystem::path& isoPath);
private:
	struct VersionSerialText
	{
		const char* version;
		const char* serial;
	};

	std::filesystem::path m_isoPath, m_exePath;
	Version m_version;
	Offset m_offset;
	static const std::unordered_map<Version, VersionSerialText> s_versionSerial;
};