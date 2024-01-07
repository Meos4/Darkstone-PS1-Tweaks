#pragma once

#include "ui_MainWindow.h"

#include "Backend/Game.hpp"
#include "FrontendQt/GuiSettings.hpp"

#include <QMainWindow>

#include <array>
#include <filesystem>
#include <memory>

class TopInfoWidget;
class TweaksWidget;
class QAction;
class QActionGroup;
class QDragEnterEvent;
class QDropEvent;

class MainWindow final : public QMainWindow
{
	Q_OBJECT
public:
	MainWindow(QWidget* parent = nullptr);

	void enableUI(const std::filesystem::path& isoPath);
	void disableUI();
public Q_SLOTS:
	void loadSettings();
	void saveSettings();
	void onFileOpen();
	void onFileSaveAs();
	void onThemeChanged();
	void onHelpAbout();
private:
	void dragEnterEvent(QDragEnterEvent* event) override;
	void dropEvent(QDropEvent* event) override;

	Ui::MainWindow m_ui;

	TopInfoWidget* m_topInfoWidget;
	TweaksWidget* m_tweaksWidget;
	QActionGroup* m_themeActionsGroup;
	std::array<QAction*, static_cast<std::size_t>(Theme::Count)> m_themeActions;

	std::shared_ptr<Game> m_game;
	GuiSettings m_guiSettings;
};