#ifndef MESSAGER_H
#define MESSAGER_H

#include <QtGlobal>
#include <QScopedPointer>
class QString;

#ifdef Q_OS_WIN32
#include <windows.h>
typedef DWORD Pid;
#else
#include <sys/types.h>
typedef pid_t Pid;
#endif


class Messager
{
public:
	static Messager &get();
	void sendMessage(Pid receiver, const QString &message);

	class ImplInterface
	{
	public:
		virtual void sendMessage(Pid receiver, const QString &message) = 0;
		virtual ~ImplInterface() {}
	};

private:
	explicit Messager();
	QScopedPointer<ImplInterface> pImpl_;
};

#endif
