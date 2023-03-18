#pragma once

#include "ui_MainWindow.h"

#include "Backend/Game.hpp"

#include <QMainWindow>

#include <filesystem>
#include <memory>

class TopInfoWidget;
class TweaksWidget;
class QDragEnterEvent;
class QDropEvent;

class MainWindow final : public QMainWindow
{
	Q_OBJECT
public:
	MainWindow(QWidget* parent = nullptr);

	void enableUI(std::filesystem::path* isoPath);
	void disableUI();
public Q_SLOTS:
	void loadSettings();
	void saveSettings();
	void onFileOpen();
	void onFileSaveAs();
	void onHelpAbout();
private:
	void dragEnterEvent(QDragEnterEvent* event) override;
	void dropEvent(QDropEvent* event) override;

	Ui::MainWindow m_ui;

	TopInfoWidget* m_topInfoWidget;
	TweaksWidget* m_tweaksWidget;

	std::shared_ptr<Game> m_game;
};