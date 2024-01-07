#include "MainWindow.hpp"

#include "Backend/Path.hpp"
#include "Common/DstException.hpp"
#include "FrontendQt/AboutDialog.hpp"
#include "FrontendQt/ExtractGameDialog.hpp"
#include "FrontendQt/GuiPath.hpp"
#include "FrontendQt/SaveGameDialog.hpp"
#include "FrontendQt/TopInfoWidget.hpp"
#include "FrontendQt/TweaksWidget.hpp"

#include "dumpsxiso/dumpsxiso.h"
#include "mkpsxiso/mkpsxiso.h"

#include <QActionGroup>
#include <QDesktopServices>
#include <QDragEnterEvent>
#include <QDropEvent>
#include <QFileDialog>
#include <QMessageBox>
#include <QMimeData>

#include <format>
#include <future>

MainWindow::MainWindow(QWidget* parent)
	: QMainWindow(parent)
{
	m_ui.setupUi(this);

	m_topInfoWidget = new TopInfoWidget(this);
	m_ui.mainLayout->addWidget(m_topInfoWidget);

	m_tweaksWidget = new TweaksWidget(this);
	m_ui.mainLayout->addWidget(m_tweaksWidget);

	m_themeActionsGroup = new QActionGroup(this);
	m_themeActionsGroup->addAction(m_ui.actionSettingsThemeDark);
	m_themeActionsGroup->addAction(m_ui.actionSettingsThemeLight);

	m_themeActions = 
	{
		m_ui.actionSettingsThemeDark,
		m_ui.actionSettingsThemeLight
	};

	connect(m_topInfoWidget, &TopInfoWidget::buttonLoadSettingsClicked, this, &MainWindow::loadSettings);
	connect(m_topInfoWidget, &TopInfoWidget::buttonSaveSettingsClicked, this, &MainWindow::saveSettings);
	connect(m_ui.actionFileOpen, &QAction::triggered, this, &MainWindow::onFileOpen);
	connect(m_ui.actionFileClose, &QAction::triggered, this, &MainWindow::disableUI);
	connect(m_ui.actionFileSaveAs, &QAction::triggered, this, &MainWindow::onFileSaveAs);
	connect(m_ui.actionFileExit, &QAction::triggered, this, &QWidget::close);

	connect(m_ui.actionSettingsThemeDark, &QAction::toggled, this, &MainWindow::onThemeChanged);
	connect(m_ui.actionSettingsThemeLight, &QAction::toggled, this, &MainWindow::onThemeChanged);

	connect(m_ui.actionHelpGitHub, &QAction::triggered, this,
		[]() { QDesktopServices::openUrl(QUrl{ "https://github.com/Meos4/Darkstone-PS1-Tweaks" }); });

	connect(m_ui.actionHelpChangelog, &QAction::triggered, this,
		[]() { QDesktopServices::openUrl(QUrl{ "https://github.com/Meos4/Darkstone-PS1-Tweaks/blob/main/CHANGELOG.md" }); });

	connect(m_ui.actionHelpAbout, &QAction::triggered, this, &MainWindow::onHelpAbout);

	disableUI();

	m_guiSettings.setOsTheme();

	std::filesystem::path guiSettingsPath{ GuiPath::dstGuiSettingsFilename };
	if (std::filesystem::is_regular_file(guiSettingsPath))
	{
		try
		{
			std::ifstream jsonFile(guiSettingsPath);
			nlohmann::json json;
			jsonFile >> json;
			m_guiSettings.loadSettings(json);
		}
		catch (nlohmann::json::exception& e)
		{
			QString errorMessage
			{
				#ifdef _WIN32
					QString::fromStdWString(std::format(L"\"{}\" is not a valid json file, ", guiSettingsPath.wstring()))
				#else
					QString::fromStdString(std::format("\"{}\" is not a valid json file, ", guiSettingsPath.string()))
				#endif
			};
			errorMessage += QString::fromStdString(std::format("Reason:\n{}", e.what()));
			QMessageBox::critical(this, "Error", errorMessage);
		}
	}

	m_themeActions[static_cast<std::size_t>(m_guiSettings.theme())]->setChecked(true);
}

void MainWindow::enableUI(const std::filesystem::path& isoPath)
{
	ExtractGameDialog extractGameDialog(this);

	auto extractGame = [&]()
	{
		try
		{
			emit extractGameDialog.onStateChanged("Extracting game, please wait");

			if (!std::filesystem::is_regular_file(isoPath))
			{
				if (std::filesystem::is_directory(isoPath))
				{
					#ifdef _WIN32 
						throw QString{ QString::fromStdWString(std::format(L"\"{}\" is a directory", isoPath.filename().wstring())) };
					#else
						throw DstException{ "\"{}\" is a directory", isoPath.filename().string() };
					#endif
				}
				else
				{
					#ifdef _WIN32 
						throw QString{ QString::fromStdWString(std::format(L"\"{}\" file does not exist", isoPath.filename().wstring())) };
					#else
						throw DstException{ "\"{}\" file does not exist", isoPath.filename().string() };
					#endif
				}
			}

			const auto version{ Game::isAValidIso(isoPath) };

			if (!version.has_value())
			{
				#ifdef _WIN32 
					throw QString{ QString::fromStdWString(std::format(L"\"{}\" is not a Darkstone binary file", isoPath.filename().wstring())) };
				#else
					throw DstException{ "\"{}\" is not a Darkstone binary file", isoPath.filename().string() };
				#endif
			}

			if (std::filesystem::is_directory(Path::dstTempDirectory))
			{
				std::error_code err;
				const auto nbRemoved{ std::filesystem::remove_all(Path::dstTempDirectory, err) };
				if (nbRemoved == -1)
				{
					throw DstException{ "\"{}\" directory cannot be removed", Path::dstTempDirectory };
				}
			}

			std::filesystem::create_directory(Path::dstTempDirectory);

			const std::filesystem::path
				configPath{ std::format("{}/{}", Path::dstTempDirectory, Path::configXmlFilename) },
				filesPath{ std::format("{}/{}", Path::dstTempDirectory, Path::filesDirectory) };

			const auto dumpArgs{ Path::dumpIsoArgs(&isoPath, &configPath, &filesPath) };

			dumpsxiso(static_cast<int>(dumpArgs.size()), (Path::CStringPlatformPtr)dumpArgs.data());

			m_game = std::make_shared<Game>(isoPath, version.value());

			emit extractGameDialog.shouldClose();

			return true;
		}
		catch (const std::exception& e)
		{
			emit extractGameDialog.onStateError(QString::fromStdString(std::format("An error occured, Reason: {}", e.what())));
		}
		catch (const QString& e)
		{
			const QString error
			{
				#ifdef _WIN32 
					QString::fromStdWString(std::format(L"An error occured, Reason: {}", QtUtility::qStrToPlatformStr(e)))
				#else
					QString::fromStdString(std::format("An error occured, Reason: {}", QtUtility::qStrToPlatformStr(e)))
				#endif
			};
			emit extractGameDialog.onStateError(error);
		}

		emit extractGameDialog.taskCompleted();
		emit extractGameDialog.onOkButtonVisibilityChanged(true);
		return false;
	};

	std::future<bool> future{ std::async(std::launch::async, extractGame) };
	extractGameDialog.exec();

	future.wait();
	if (!future.get())
	{
		disableUI();
	}
	else
	{
		try
		{
			m_tweaksWidget->enableUI(m_game);
			const QString verSerial{ QString::fromStdString(std::format("{} [{}]", m_game->versionText(), m_game->serialText())) };
			const QString filename
			{
				#ifdef _WIN32 
					QString::fromStdWString(m_game->isoFilename().wstring())
				#else
					QString::fromStdString(m_game->isoFilename().string())
				#endif
			};

			m_topInfoWidget->enableUI(verSerial, filename);

			m_ui.actionFileClose->setEnabled(true);
			m_ui.actionFileSaveAs->setEnabled(true);
		}
		catch (const std::exception& e)
		{
			QMessageBox::critical(this, "Error", QString::fromStdString(std::format("An error occured, Reason: {}", e.what())));
			disableUI();
		}
	}
}

void MainWindow::disableUI()
{
	m_ui.actionFileClose->setEnabled(false);
	m_ui.actionFileSaveAs->setEnabled(false);

	m_topInfoWidget->disableUI();
	m_tweaksWidget->disableUI();

	if (m_game)
	{
		m_game.reset();
	}
}

void MainWindow::loadSettings()
{
	const auto settingsPathQStr{ QFileDialog::getOpenFileName(this, "Open Darkstone PS1 Tweaks Settings File", QString{}, "*.json", nullptr) };
	if (!settingsPathQStr.isEmpty())
	{
		const std::filesystem::path settingsPath{ QtUtility::qStrToPlatformStr(settingsPathQStr) };
		m_tweaksWidget->loadSettings(settingsPath);
	}	
}

void MainWindow::saveSettings()
{
	const auto settingsPathQStr{ QFileDialog::getSaveFileName(this, "Save Darkstone PS1 Tweaks Settings File", QString{}, "*.json", nullptr)};
	if (!settingsPathQStr.isEmpty())
	{
		std::filesystem::path settingsPath{ QtUtility::qStrToPlatformStr(settingsPathQStr) };
		m_tweaksWidget->saveSettings(settingsPath);
	}
}

void MainWindow::onFileOpen()
{
	const auto filePathQStr{ QFileDialog::getOpenFileName(this, "Open Darkstone PS1 Binary File", QString{}, "*.bin *.cue", nullptr)};
	if (!filePathQStr.isEmpty())
	{
		std::filesystem::path filePath{ QtUtility::qStrToPlatformStr(filePathQStr) };
		enableUI(filePath);
	}
}

void MainWindow::onFileSaveAs()
{
	const QString filePathQStr{ QFileDialog::getSaveFileName(this, "Save Darkstone Binary File", QString{}, "*.bin", nullptr)};
	if (filePathQStr.isEmpty())
	{
		return;
	}
	else if (const auto isoPath{ m_game->isoPath() };
		 m_game->isVanilla() && QtUtility::qStrToPlatformStr(filePathQStr) == isoPath && std::filesystem::is_regular_file(isoPath))
	{
		static constexpr auto message
		{ 
			"Are you sure to save on your vanilla iso? it is highly "
			"recommended to make a copy before because tweaks can't be undone." 
		};

		if (QMessageBox::question(this, "Confirm", message, QMessageBox::Yes | QMessageBox::No) == QMessageBox::No)
		{
			return;
		}
	}

	SaveGameDialog saveGameDialog(this);

	auto saveGame = [&]()
	{
		try
		{
			emit saveGameDialog.onStateChanged("Apply tweaks...");
			m_tweaksWidget->write();

			const std::filesystem::path filesPath{ std::format("{}/{}", Path::dstTempDirectory, Path::filesDirectory) };

			emit saveGameDialog.progressBarChanged(50);
			emit saveGameDialog.onStateChanged("Repack iso...");

			const std::filesystem::path 
				configPath{ std::format("{}/{}", Path::dstTempDirectory, Path::configXmlFilename) },
				filePath{ QtUtility::qStrToPlatformStr(filePathQStr) };

			const auto makeArgs{ Path::makeIsoArgs(&filePath, &configPath) };
			if (mkpsxiso(static_cast<int>(makeArgs.size()), (Path::CStringPlatformPtr)makeArgs.data()) == EXIT_FAILURE)
			{
				throw DstException{ "Unable to repack iso" };
			}

			emit saveGameDialog.progressBarChanged(100);
			emit saveGameDialog.onStateChanged("Done");
			emit saveGameDialog.taskCompleted();
			return true;
		}
		catch (const std::exception& e)
		{
			emit saveGameDialog.onStateError(QString::fromStdString(std::format("An error occured, Reason: {}", e.what())));
			emit saveGameDialog.taskCompleted();
			return false;
		}
	};

	std::future<bool> future{ std::async(std::launch::async, saveGame) };
	saveGameDialog.exec();

	future.wait();

	// For some reasons mkpsxiso creates a file named mkpsxiso.cue
	if (std::filesystem::exists("mkpsxiso.cue"))
	{
		std::filesystem::remove("mkpsxiso.cue");
	}

	if (!future.get())
	{
		disableUI();
	}
	else
	{
		const std::filesystem::path filePath{ QtUtility::qStrToPlatformStr(filePathQStr) };
		m_game->setIsoPath(filePath);
		m_game->setNotVanilla();
		
		const QString filename
		{
			#ifdef _WIN32
				QString::fromStdWString(m_game->isoFilename().wstring())
			#else
				QString::fromStdString(m_game->isoFilename().string())
			#endif
		};

		m_topInfoWidget->setFilename(filename);
	}
}

void MainWindow::onThemeChanged()
{
	for (std::size_t i{}; i < m_themeActions.size(); ++i)
	{
		if (m_themeActions[i]->isChecked())
		{
			m_guiSettings.updateTheme(static_cast<Theme>(i));
			break;
		}
	}
}

void MainWindow::onHelpAbout()
{
	AboutDialog about(this);
	about.exec();
}

void MainWindow::dragEnterEvent(QDragEnterEvent* event)
{
	event->acceptProposedAction();
}

void MainWindow::dropEvent(QDropEvent* event)
{
	const auto* mimeData{ event->mimeData() };

	if (mimeData->hasUrls())
	{
		std::filesystem::path path
		{ 
			mimeData->urls().front().toLocalFile().
			#ifdef _WIN32
				toStdWString() 
			#else
				toStdString()
			#endif
		};
		
		if (m_game && path.extension() == ".json")
		{
			m_tweaksWidget->loadSettings(path);
		}
		else
		{
			enableUI(path);
		}
	}
}