#include "TweaksWidget.hpp"

#include "Common/DstException.hpp"

#include "nlohmann/json.hpp"

#include <QMessageBox>

TweaksWidget::TweaksWidget(QWidget* parent)
{
	m_ui.setupUi(this);
	
	setStyleSheet("font-weight: bold;");

	m_qCheckBox =
	{

	};

	m_qComboBox =
	{

	};
}

void TweaksWidget::enableUI(std::shared_ptr<Game> game)
{
	m_tweaks = std::make_unique<Tweaks>(game);
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

	std::ofstream jsonFile(path);
	jsonFile << std::setw(4) << json;
}