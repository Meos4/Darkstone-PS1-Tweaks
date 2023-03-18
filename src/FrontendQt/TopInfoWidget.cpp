#include "TopInfoWidget.hpp"

#include "FrontendQt/MainWindow.hpp"
#include "FrontendQt/QtUtility.hpp"

#include <QString>

TopInfoWidget::TopInfoWidget(QWidget* parent)
	: QWidget(parent)
{
	m_ui.setupUi(this);

	disableUI();

	connect(m_ui.loadSettings, &QAbstractButton::clicked, this, &TopInfoWidget::buttonLoadSettingsClicked);
	connect(m_ui.saveSettings, &QAbstractButton::clicked, this, &TopInfoWidget::buttonSaveSettingsClicked);
}

void TopInfoWidget::enableUI(const QString& version, const QString& filename)
{
	setVersion(version);
	setFilename(filename);

	m_ui.loadSettings->setEnabled(true);
	m_ui.saveSettings->setEnabled(true);
}

void TopInfoWidget::disableUI()
{
	setVersion("");
	setFilename("");

	m_ui.loadSettings->setEnabled(false);
	m_ui.saveSettings->setEnabled(false);
}

void TopInfoWidget::setVersion(const QString& version)
{
	m_ui.versionLabel2->setText(version);
}

void TopInfoWidget::setFilename(const QString& filename)
{
	static constexpr auto maxSize{ 50 };
	if (filename.size() > maxSize)
	{
		auto filenameReduced{ filename };
		filenameReduced.resize(maxSize);
		for (std::size_t i{}; i < 3; ++i)
		{
			filenameReduced[maxSize - 1 - i] = '.';
		}
		m_ui.fileLabel2->setText(filenameReduced);
	}
	else
	{
		m_ui.fileLabel2->setText(filename);
	}
}