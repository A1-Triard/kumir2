#ifndef KPLUGIN_H
#define KPLUGIN_H

#include <QtCore>
#include "pluginspec.h"
#include "settings.h"
#include "commandlineparameter.h"
#include <kumir2/plugininterface.h>

#ifdef EXTENSIONSYSTEM_LIBRARY
#define EXTENSIONSYSTEM_EXPORT Q_DECL_EXPORT
#else
#define EXTENSIONSYSTEM_EXPORT Q_DECL_IMPORT
#endif

class QWidget;
class QIcon;

namespace ExtensionSystem {

typedef Shared::GlobalState GlobalState;

class EXTENSIONSYSTEM_EXPORT KPlugin
        : public QObject
        , public Shared::PluginInterface
{
    friend class PluginManager;
    friend struct PluginManagerImpl;
    Q_OBJECT
    Q_INTERFACES(Shared::PluginInterface)
public:
    virtual QByteArray pluginName() const;
    enum State { Disabled = 0x00, Loaded=0x01, Initialized=0x02, Started=0x03, Stopped=0x04 };
    KPlugin();
    const PluginSpec & pluginSpec() const;
    State state() const;
    inline virtual bool isGuiRequired() const { return pluginSpec().gui; } // Can be overridden in special cases

    /** DEPRECATED
     * Use
     *   QList<QWidget*> settingsEditorPages()
     * instead
     */
    inline virtual QWidget* settingsEditorPage() { return 0; }

    inline virtual QList<QWidget*> settingsEditorPages() { return QList<QWidget*>(); }

    SettingsPtr mySettings() const;

    QDir myResourcesDir() const;

    virtual ~KPlugin();


    inline virtual void saveSession() const { }
    inline virtual void restoreSession() { }
    inline virtual void changeCurrentDirectory(const QString & path) { Q_UNUSED(path); }
    inline virtual void changeGlobalState(GlobalState old, GlobalState current) { Q_UNUSED(old); Q_UNUSED(current); }
    inline virtual QList<CommandLineParameter> acceptableCommandLineParameters() const
    {
        return QList<CommandLineParameter>();
    }
    void initialize(const QString & pluginResourcesRootPath);
    virtual QString initialize(
            const QStringList & /*configurationArguments*/,
            const CommandLine & /*runtimeArguments*/
            ) = 0;

    inline virtual bool isSafeToQuit() { return true; }
    inline virtual void start() {}
    inline virtual void stop() {}
    virtual void updateSettings(const QStringList & keys) = 0;
    KPlugin * myDependency(const QByteArray & name) const;

    QList<KPlugin*> loadedPlugins(const QByteArray &pattern = "*");
    QList<const KPlugin*> loadedPlugins(const QByteArray &pattern = "*") const;

protected:
    virtual void createPluginSpec() = 0;
    PluginSpec _pluginSpec;

private:
    SettingsPtr _settings;
    QString _resourcesDir;
    State _state;
};

} // namespace ExtensionSystem

#endif // KPLUGIN_H
