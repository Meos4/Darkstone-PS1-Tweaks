#include "AboutDialog.hpp"

#include "Common/Software.hpp"

#include <format>

AboutDialog::AboutDialog(QWidget* parent)
	: QDialog(parent)
{
	m_ui.setupUi(this);

	m_ui.nameLabel->setText(QString::fromStdString(
		std::format("<b><font size=\"+2\">Darkstone PS1 Tweaks v{} by Meos</b></font>", Software::version)));

	m_ui.linkLabel->setOpenExternalLinks(true);
	m_ui.linkLabel->setText("<a href=\"https://github.com/Meos4\">GitHub</a>");
}