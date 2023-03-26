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
		{ SETTINGS(m_ui.unlockCostumeByDefaultEnable) },
		{ SETTINGS(m_ui.expandHeroAndLegendShopsEnable) },
		{ SETTINGS(m_ui.theftBlockFix) },
		{ SETTINGS(m_ui.theftEmptyJewelryFix) },
		{ SETTINGS(m_ui.framerate60Enable) }
	};

	m_qComboBox =
	{

	};

	m_ui.hudColorCombo->setStyleSheet("font-weight: normal;");
	m_ui.hudColorR->setStyleSheet("font-weight: normal;");
	m_ui.hudColorG->setStyleSheet("font-weight: normal;");
	m_ui.hudColorB->setStyleSheet("font-weight: normal;");

	static constexpr std::array<const char*, 8> hudNames
	{
		"Warrior", "Amazon", "Wizard", "Sorceress",
		"Assassin", "Thief", "Monk", "Priestess"
	};

	for (const auto& name : hudNames)
	{
		m_ui.hudColorCombo->addItem(name);
	}

	connect(m_ui.hudColorCombo, &QComboBox::currentIndexChanged, this, &TweaksWidget::updateHudThemes);
	connect(m_ui.hudColorR, &QSpinBox::valueChanged, this, &TweaksWidget::updateHudColorRGB);
	connect(m_ui.hudColorG, &QSpinBox::valueChanged, this, &TweaksWidget::updateHudColorRGB);
	connect(m_ui.hudColorB, &QSpinBox::valueChanged, this, &TweaksWidget::updateHudColorRGB);
}

void TweaksWidget::enableUI(std::shared_ptr<Game> game)
{
	m_tweaks = std::make_unique<Tweaks>(game);

	if (m_isFirstEnableUI)
	{
		m_colors = m_tweaks->hudColor();
		m_previousThemeIndex = m_ui.hudColorCombo->currentIndex();
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

	if (m_ui.unlockCostumeByDefaultEnable->isChecked())
	{
		m_tweaks->unlockCostumeByDefault();
	}

	if (m_ui.expandHeroAndLegendShopsEnable->isChecked())
	{
		m_tweaks->expandHeroAndLegendShops();
	}

	m_tweaks->hudColor(hudColor());

	if (m_ui.theftBlockFix->isChecked())
	{
		m_tweaks->theftBlock();
	}

	if (m_ui.theftEmptyJewelryFix->isChecked())
	{
		m_tweaks->theftEmptyJewelry();
	}

	if (m_ui.framerate60Enable->isChecked())
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
		colorToUI(m_ui.hudColorCombo->currentIndex());
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
	const auto theme{ m_ui.hudColorCombo->currentIndex() };

	hud[theme].red = static_cast<u8>(m_ui.hudColorR->value());
	hud[theme].green = static_cast<u8>(m_ui.hudColorG->value());
	hud[theme].blue = static_cast<u8>(m_ui.hudColorB->value());

	return hud;
}

void TweaksWidget::colorToUI(s32 theme)
{
	m_ui.hudColorR->setValue(m_colors[theme].red);
	m_ui.hudColorG->setValue(m_colors[theme].green);
	m_ui.hudColorB->setValue(m_colors[theme].blue);
}

void TweaksWidget::UIToColor(s32 theme)
{
	m_colors[theme].red = static_cast<u8>(m_ui.hudColorR->value());
	m_colors[theme].green = static_cast<u8>(m_ui.hudColorG->value());
	m_colors[theme].blue = static_cast<u8>(m_ui.hudColorB->value());
}

void TweaksWidget::updateHudThemes()
{
	UIToColor(m_previousThemeIndex);
	m_previousThemeIndex = m_ui.hudColorCombo->currentIndex();
	colorToUI(m_previousThemeIndex);
}

void TweaksWidget::updateHudColorRGB()
{
	const auto colorRGB
	{
		std::format("background-color:rgb({},{},{});",
			m_ui.hudColorR->value(), m_ui.hudColorG->value(), m_ui.hudColorB->value())
	};
	m_ui.hudColorResult->setStyleSheet(QString::fromStdString(colorRGB));
}