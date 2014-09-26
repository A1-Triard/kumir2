#ifndef GDB_RUN_PLUGIN
#define GDB_RUN_PLUGIN

#include "extensionsystem/kplugin.h"
#include "interfaces/runinterface.h"

#include "pascalvariablesmodel.h"

#include <QProcess>
#include <QFile>
#include <QMutex>
#include <QMutexLocker>
#include <QThread>


namespace GdbRun {

using namespace ExtensionSystem;
using namespace Shared;



class GdbRunPlugin
        : public KPlugin
        , public RunInterface
{
    Q_OBJECT
#if QT_VERSION >= 0x050000
    Q_PLUGIN_METADATA(IID "kumir2.GDBRun" FILE "")
#endif
    Q_INTERFACES(Shared::RunInterface)
public:
    explicit GdbRunPlugin();
    QString initialize(const QStringList &conf, const CommandLine &);
    void updateSettings(const QStringList &);
    bool loadProgram(const RunnableProgram &sourceInfo);
    QDateTime loadedProgramVersion() const;
    bool canStepOut() const;
    void runBlind();
    void runContinuous();
    void runStepInto();
    void runStepOver();
    void runToEnd();
    void runTesting();
    bool isTestingRun() const;
    void terminate();
    void terminateAndWaitForStopped();
    bool hasMoreInstructions() const;
    bool hasTestingEntryPoint() const;
    int currentLineNo() const;
    QPair<quint32,quint32> currentColumn() const;
    QString error() const;
    QVariant valueStackTopItem() const;
    unsigned long int stepsCounted() const;
    inline QAbstractItemModel * debuggerVariablesViewModel() const { return variablesModel_; }
    inline void setStdInTextStream(QTextStream *) {}
    inline void setStdOutTextStream(QTextStream *) {}

signals:
    void stopped(int reason);
    void outputRequest(const QString & output);
    void errorOutputRequest(const QString &);
    void inputRequest(const QString & format);
    void finishInput(const QVariantList & data);
    void lineChanged(int lineNo, quint32 colStart, quint32 colEnd);
    void marginText(int lineNo, const QString & text);

protected slots:
    void handleInputDone(const QVariantList &data);

    void handleGdbClientReadStdOut();
    void handleGdbClientReadStdErr();

    void handleGdbServerReadStdOut();
    void handleGdbServerReadStdErr();

    void handleGdbStatusStream(const QByteArray & resultStream);
    void handleGdbInteractionStream(const QByteArray & resultStream);
    void handleGdbLogOutputStream(const QByteArray & resultStream);
    void handleGdbAsyncMessageStream(const QByteArray & resultStream);
    void handleGdbAsyncLogStream(const QByteArray & resultStream);

protected:
    virtual void extractInputFormat();
    virtual void processGdbQueryResponse(const QMap<QString,QVariant>& response);
    virtual void loadGlobalSymbols();
    virtual void processInfoVariablesResponse(const QStringList & rawLines);
    virtual void processInfoFuncResponse(const QStringList & rawLines);
    virtual void setBreak1();


private:    
    enum GdbState {
        NotStarted,
        StartedServer,
        StartedBoth,
        Querying,
        Paused,
        Running,
        Terminating
    };
    enum InteractionQuery {
        NoInteractionQuery,
        GetGlobalSymbolsTable,
        GetFunctionsTable
    };


    void sendGdbCommand(const QByteArray& command);
    void queueGdbCommand(const QByteArray& command, GdbState condition, InteractionQuery query = NoInteractionQuery);
    void queueInteractionCommand(InteractionQuery query);
    void queueGdbCommands(const QList<QByteArray>& command, GdbState condition);
    void flushGdbCommands();
    static QMap<QString,QVariant> parseGdbMiCommandOutput(const QString & out);
    static QStringList splitByTopLevelComas(const QString &in);
    static QString gdbCommand();
    static QString gdbServerCommand();


    QProcess* gdbClient_;
    QProcess* gdbServer_;
    QString programFileName_;    
    PascalVariablesModel* variablesModel_;
    Q_PID inferiorPid_;
    QTextCodec* ioCodec_;
    GdbState gdbState_;
    QMutex* gdbStateLocker_;
    struct StateWaiter: public QThread {
        inline StateWaiter(GdbRunPlugin* parent): QThread(parent), p(parent) {}
        GdbRunPlugin* p;
        GdbState t;
        void run();
        void waitForState(GdbState targetState);
    }* stateWaiter_;
    struct ConditionalCommand {
        QByteArray command;
        GdbState cond;
        InteractionQuery query;
    };
    QLinkedList<ConditionalCommand> gdbCommandsQueue_;

    QMutex* gdbCommandsQueueLocker_;
    bool symbolsLoaded_;
    bool breakLine1Inserted_;
    QByteArray bkptNoQuery_;
    QString mainProgramSourceFileName_;
    InteractionQuery interactionQuery_;
    QByteArray interactionBuffer_;
    bool showVariablesMode_;
    bool queryFpcInput_;
    QByteArray lastRunCommand_;
    int currentFrameLevel_;
};

}


#endif