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
	void loadPresets(const std::filesystem::path& path);
	void savePresets(const std::filesystem::path& path);
	Tweaks::HudColorArray hudColor() const;
public Q_SLOTS:
	void updateHudThemes();
	void updateHudColorRGB();
private:
	void colorToUI(s32 theme);
	void UIToColor(s32 theme);

	Ui::TweaksWidget m_ui;

	Tweaks::HudColorArray m_colors{};
	s32 m_previousThemeIndex{};
	bool m_isFirstEnableUI{ true };

	std::unique_ptr<Tweaks> m_tweaks;
	std::vector<TweaksQCheckBox> m_qCheckBox;
};