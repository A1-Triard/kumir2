#ifndef ROBOTMODULEBASE_H
#define ROBOTMODULEBASE_H

#include <kumir2-libs/extensionsystem/kplugin.h>

class QWidget;
class QMenu;
class QAction;

namespace ActorRobot {
class RobotPlugin;

class RobotModuleBase : public QObject
{
	Q_OBJECT
public /* methods */:
	RobotModuleBase(ExtensionSystem::KPlugin* parent);
	QList<QMenu*> moduleMenus() const;
	QDir myResourcesDir() const;
	ExtensionSystem::SettingsPtr mySettings() const;
	virtual QWidget* mainWidget() const = 0;
	virtual QWidget* pultWidget() const = 0;
	virtual void handleGuiReady();
	virtual QString initialize(const QStringList &configurationParameters, const ExtensionSystem::CommandLine & runtimeParameters);
	virtual bool isSafeToQuit();
	virtual QVariantList templateParameters() const;
public Q_SLOTS:
	virtual void reset() = 0;
	virtual void reloadSettings(ExtensionSystem::SettingsPtr settings, const QStringList & keys) = 0;
	virtual void changeGlobalState(ExtensionSystem::GlobalState old, ExtensionSystem::GlobalState current) = 0;
	virtual void terminateEvaluation() = 0;
	virtual void setAnimationEnabled(bool enabled) = 0;
	virtual void runGoUp() = 0;
	virtual void runGoDown() = 0;
	virtual void runGoLeft() = 0;
	virtual void runGoRight() = 0;
	virtual void runDoPaint() = 0;
	virtual bool runIsWallAtTop() = 0;
	virtual bool runIsWallAtBottom() = 0;
	virtual bool runIsWallAtLeft() = 0;
	virtual bool runIsWallAtRight() = 0;
	virtual bool runIsFreeAtTop() = 0;
	virtual bool runIsFreeAtBottom() = 0;
	virtual bool runIsFreeAtLeft() = 0;
	virtual bool runIsFreeAtRight() = 0;
	virtual bool runIsColor() = 0;
	virtual bool runIsClear() = 0;
	virtual qreal runRadiation() = 0;
	virtual int runTemperature() = 0;
	virtual void runFieldSize(int& rows, int& cols) = 0;
	virtual bool runMark(const int row, const int col) = 0;
	virtual bool runColored(const int row, const int col) = 0;
	virtual void runRobotPos(int& row, int& col) = 0;
	virtual QChar runUpChar(const int row, const int col) = 0;
	virtual int runCellTemp(const int row, const int col) = 0;
	virtual qreal runCellRad(const int row, const int col) = 0;
	virtual QChar runDownChar(const int row, const int col) = 0;
	virtual void loadActorData(QIODevice * source);

Q_SIGNALS:
	void notifyOnTemplateParametersChanged();

protected: /* methods */
	const ExtensionSystem::CommandLine& commandLineParameters() const;
	void msleep(unsigned long msecs);
	void setError(const QString & errorText);
	void sleep(unsigned long secs);
	void usleep(unsigned long usecs);

protected: /* fields */
	QMenu* m_menuRobot;
	QAction* m_actionRobotLoadEnvironment;
	QAction* m_actionRobotLoadRescent;
	QAction* m_actionRobotRevertEnvironment;
	QAction* m_actionRobotSaveEnvironment;
	QAction* m_actionRobotSave2Png;
	QAction* m_actionRobotEditEnvironment;
	QAction* m_actionRobotAutoWindowSize;
	QAction* m_actionRobotNewEnvironment;

private:
	RobotPlugin *plugin;
};

} // namespace ActorRobot

#endif // ROBOTMODULEBASE_H
