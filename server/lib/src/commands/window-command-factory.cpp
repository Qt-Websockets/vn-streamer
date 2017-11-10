#include "window-command-factory.h"
#include <QMap>
#include "logger.h"
#include "composite-window-command.h"
#include "sleep-window-command.h"
#include "mouse-window-command.h"


WindowCommand *WindowCommandFactory::create(const QString &text)
{
	QStringList commands = text.split(",");

	QList<WindowCommand*> list;
	for (const QString &command : commands)
	{
		WindowCommand *cmd = makeSingle(command);
		if (cmd != Q_NULLPTR)
			list.append(cmd);
	}

	if (list.isEmpty())
		return Q_NULLPTR;
	if (list.count() > 1)
		return new CompositeWindowCommand(list);
	return list.first();
}

WindowCommand *WindowCommandFactory::makeSingle(const QString &command)
{
	if (command.isEmpty())
		return Q_NULLPTR;

	QStringList data = command.split(' ', QString::SkipEmptyParts);
	QString type = data.takeFirst();

	// Sleep commands
	if (type == "sleep" && data.count() == 1)
	{
		int duration = data[0].toInt();
		return new SleepWindowCommand(duration);
	}

	// Keyboard commands
	if (type == "key")
	{
		// TODO
		return Q_NULLPTR;
	}

	// Mouse commands
	if (type == "mouse" && data.count() == 3)
	{
		static QMap<QString, Qt::MouseButton> buttons = {
			{ "left", Qt::LeftButton },
			{ "right", Qt::RightButton },
		};

		if (!buttons.contains(data[0]))
		{
			LOG(QString("Invalid button '%1'").arg(data[0]), Logger::Error);
			return Q_NULLPTR;
		}

		Qt::MouseButton button = buttons[data[0]];
		QPoint pos(data[1].toInt(), data[2].toInt());
		return new MouseWindowCommand(button, pos);
	}

	LOG(QString("Invalid command '%1'").arg(command), Logger::Error);
	return Q_NULLPTR;
}
