#include "TweaksWidget.hpp"

#include "Common/DstException.hpp"

#include "nlohmann/json.hpp"

#include <format>

#include <QMessageBox>

TweaksWidget::TweaksWidget(QWidget* parent)
	: QWidget(parent)
{
	m_ui.setupUi(this);
	
	setStyleSheet("font-weight: bold;");

	m_qCheckBox =
	{
		{ SETTINGS(m_ui.tweaksUnlockCostumeByDefault) },
		{ SETTINGS(m_ui.tweaksExpandHeroAndLegendShops) },
		{ SETTINGS(m_ui.tweaksPermanentShopsItems) },
		{ SETTINGS(m_ui.tweaksSpellDurability3Stacks) },
		{ SETTINGS(m_ui.fixesTheftBlock) },
		{ SETTINGS(m_ui.fixesTheftEmptyJewelry) },
		{ SETTINGS(m_ui.duckstation60FPS) }
	};

	m_qComboBox =
	{

	};

	m_ui.tweaksHudColorCombo->setStyleSheet("font-weight: normal;");
	m_ui.tweaksHudColorR->setStyleSheet("font-weight: normal;");
	m_ui.tweaksHudColorG->setStyleSheet("font-weight: normal;");
	m_ui.tweaksHudColorB->setStyleSheet("font-weight: normal;");

	static constexpr std::array<const char*, 8> hudNames
	{
		"Warrior", "Amazon", "Wizard", "Sorceress",
		"Assassin", "Thief", "Monk", "Priestess"
	};

	for (const auto& name : hudNames)
	{
		m_ui.tweaksHudColorCombo->addItem(name);
	}

	connect(m_ui.tweaksHudColorCombo, &QComboBox::currentIndexChanged, this, &TweaksWidget::updateHudThemes);
	connect(m_ui.tweaksHudColorR, &QSpinBox::valueChanged, this, &TweaksWidget::updateHudColorRGB);
	connect(m_ui.tweaksHudColorG, &QSpinBox::valueChanged, this, &TweaksWidget::updateHudColorRGB);
	connect(m_ui.tweaksHudColorB, &QSpinBox::valueChanged, this, &TweaksWidget::updateHudColorRGB);
}

void TweaksWidget::enableUI(std::shared_ptr<Game> game)
{
	m_tweaks = std::make_unique<Tweaks>(game);

	if (m_isFirstEnableUI)
	{
		m_colors = m_tweaks->hudColor();
		m_previousThemeIndex = m_ui.tweaksHudColorCombo->currentIndex();
		colorToUI(m_previousThemeIndex);
		m_isFirstEnableUI = false;
	}

	setEnabled(true);
}

void TweaksWidget::disableUI()
{
	setDisabled(true);
	if (m_tweaks)
	{
		m_tweaks.reset();
	}
}

void TweaksWidget::write() const
{
	if (!m_tweaks)
	{
		throw DstException{ "Game is uninitialized" };
	}

	if (m_ui.tweaksUnlockCostumeByDefault->isChecked())
	{
		m_tweaks->unlockCostumeByDefault();
	}

	if (m_ui.tweaksExpandHeroAndLegendShops->isChecked())
	{
		m_tweaks->expandHeroAndLegendShops();
	}

	if (m_ui.tweaksPermanentShopsItems->isChecked())
	{
		m_tweaks->permanentShopsItems();
	}

	if (m_ui.tweaksSpellDurability3Stacks->isChecked())
	{
		m_tweaks->spellDurability3Stacks();
	}

	m_tweaks->hudColor(hudColor());

	if (m_ui.fixesTheftBlock->isChecked())
	{
		m_tweaks->theftBlock();
	}

	if (m_ui.fixesTheftEmptyJewelry->isChecked())
	{
		m_tweaks->theftEmptyJewelry();
	}

	if (m_ui.duckstation60FPS->isChecked())
	{
		m_tweaks->framerate60();
	}
}

void TweaksWidget::loadSettings(const std::filesystem::path& path)
{
	try
	{
		std::ifstream jsonFile(path);
		nlohmann::json json;
		jsonFile >> json;

		for (auto& checkBox : m_qCheckBox)
		{
			checkBox.load(json);
		}

		for (auto& comboBox : m_qComboBox)
		{
			comboBox.load(json);
		}

		for (std::size_t i{}; i < m_colors.size(); ++i)
		{
			Json::set<u8>(json["colors"][i], "red", [&](auto v) { m_colors[i].red = v; });
			Json::set<u8>(json["colors"][i], "green", [&](auto v) { m_colors[i].green = v; });
			Json::set<u8>(json["colors"][i], "blue", [&](auto v) { m_colors[i].blue = v; });
		}
		colorToUI(m_ui.tweaksHudColorCombo->currentIndex());
	}
	catch (const nlohmann::json::exception& e)
	{
		QString errorMessage
		{
			#ifdef _WIN32
				QString::fromStdWString(std::format(L"\"{}\" is not a valid json file, ", path.wstring()))
			#else
				QString::fromStdString(std::format("\"{}\" is not a valid json file, ", path.string()))
			#endif
		};
		errorMessage += QString::fromStdString(std::format("Reason:\n{}", e.what()));
		QMessageBox::critical(this, "Error", errorMessage);
	}
}

void TweaksWidget::saveSettings(const std::filesystem::path& path)
{
	nlohmann::ordered_json json;

	for (const auto& checkBox : m_qCheckBox)
	{
		checkBox.save(&json);
	}

	for (const auto& comboBox : m_qComboBox)
	{
		comboBox.save(&json);
	}

	UIToColor(m_previousThemeIndex);
	for (std::size_t i{}; i < m_colors.size(); ++i)
	{
		json["colors"][i]["red"] = m_colors[i].red;
		json["colors"][i]["green"] = m_colors[i].green;
		json["colors"][i]["blue"] = m_colors[i].blue;
	}

	std::ofstream jsonFile(path);
	jsonFile << std::setw(4) << json;
}

Tweaks::HudColorArray TweaksWidget::hudColor() const
{
	auto hud{ m_colors };
	const auto theme{ m_ui.tweaksHudColorCombo->currentIndex() };

	hud[theme].red = static_cast<u8>(m_ui.tweaksHudColorR->value());
	hud[theme].green = static_cast<u8>(m_ui.tweaksHudColorG->value());
	hud[theme].blue = static_cast<u8>(m_ui.tweaksHudColorB->value());

	return hud;
}

void TweaksWidget::colorToUI(s32 theme)
{
	m_ui.tweaksHudColorR->setValue(m_colors[theme].red);
	m_ui.tweaksHudColorG->setValue(m_colors[theme].green);
	m_ui.tweaksHudColorB->setValue(m_colors[theme].blue);
}

void TweaksWidget::UIToColor(s32 theme)
{
	m_colors[theme].red = static_cast<u8>(m_ui.tweaksHudColorR->value());
	m_colors[theme].green = static_cast<u8>(m_ui.tweaksHudColorG->value());
	m_colors[theme].blue = static_cast<u8>(m_ui.tweaksHudColorB->value());
}

void TweaksWidget::updateHudThemes()
{
	UIToColor(m_previousThemeIndex);
	m_previousThemeIndex = m_ui.tweaksHudColorCombo->currentIndex();
	colorToUI(m_previousThemeIndex);
}

void TweaksWidget::updateHudColorRGB()
{
	const auto colorRGB
	{
		std::format("background-color:rgb({},{},{});",
			m_ui.tweaksHudColorR->value(), m_ui.tweaksHudColorG->value(), m_ui.tweaksHudColorB->value())
	};
	m_ui.tweaksHudColorResult->setStyleSheet(QString::fromStdString(colorRGB));
}