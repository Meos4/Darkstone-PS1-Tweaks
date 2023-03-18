#pragma once

#include "ui_TweaksWidget.h"

#include "Backend/Game.hpp"
#include "Backend/Tweaks.hpp"
#include "FrontendQt/TweaksWidgetSettings.hpp"

#include <vector>

#include <QWidget>

class TweaksWidget final : public QWidget
{
	Q_OBJECT
public:
	TweaksWidget(QWidget* parent = nullptr);

	void enableUI(std::shared_ptr<Game> game);
	void disableUI();
	void write() const;
	void loadSettings(const std::filesystem::path& path);
	void saveSettings(const std::filesystem::path& path);
public Q_SLOTS:
	
private:
	Ui::TweaksWidget m_ui;

	std::unique_ptr<Tweaks> m_tweaks;
	std::vector<TweaksQCheckBox> m_qCheckBox;
	std::vector<TweaksQComboBox> m_qComboBox;
};