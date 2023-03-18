#pragma once

#include "ui_TopInfoWidget.h"

#include "Common/Types.hpp"

#include <QWidget>

class QString;

class TopInfoWidget final : public QWidget
{
	Q_OBJECT
public:
	TopInfoWidget(QWidget* parent = nullptr);

	void enableUI(const QString& version, const QString& filename);
	void disableUI();

	void setVersion(const QString& version);
	void setFilename(const QString& filename);
Q_SIGNALS:
	void buttonLoadSettingsClicked();
	void buttonSaveSettingsClicked();
private:
	Ui::TopInfoWidget m_ui;
};