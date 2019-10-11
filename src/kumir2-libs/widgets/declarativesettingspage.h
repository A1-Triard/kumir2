#ifndef DECLARATIVE_SETTINGS_PAGE
#define DECLARATIVE_SETTINGS_PAGE

#ifdef WIDGETS_LIBRARY
#define WIDGETS_EXPORT Q_DECL_EXPORT
#else
#define WIDGETS_EXPORT Q_DECL_IMPORT
#endif

#include <kumir2-libs/extensionsystem/settings.h>

#include <QtCore>
#if QT_VERSION >= 0x050000
#include <QtWidgets>
#else
#include <QtGui>
#endif

namespace Widgets {

class WIDGETS_EXPORT DeclarativeSettingsPage : public QWidget
{  
    friend class DeclarativeSettingsPageImpl;
    Q_OBJECT
public:
    enum Type { Integer, Double, String, Char, Bool, Color, Font, Choice };

    struct Entry {
        Type type;
        QString title;
        QVariant defaultValue;
        QVariant minimumValue;
        QVariant maximumValue;
        QStringList items;
        qreal displayOrder;
        QString key;

        inline bool operator < (const struct Entry & other) const {
            return this->displayOrder < other.displayOrder;
        }
    };

    explicit DeclarativeSettingsPage(
            const QString &title,
            ExtensionSystem::SettingsPtr settings,
            const QMap<QString,Entry> entries
            );

    void setSettingsObject(ExtensionSystem::SettingsPtr settings);
    ExtensionSystem::SettingsPtr settingsObject() const;

public slots:
    void init();
    void accept();
    void resetToDefaults();

signals:
    void settingsChanged(const QStringList & keys);

private:
    class DeclarativeSettingsPageImpl* pImpl_;
};

}

#endif
