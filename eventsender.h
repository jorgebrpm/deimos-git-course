#ifndef EVENTSENDER_H
#define EVENTSENDER_H
#include <QObject>
#include <QQueue>
#include <QtNetwork/QHostAddress>
#include <QtNetwork/QUdpSocket>
#include <QThread>
#include <QMutex>
#include <daemonmodule.h>
#include <mailslot.h>
#include <confreader.h>
#include <sharedmemory.h>
#include <QMap>
class CEventSender: public QObject
{
    Q_OBJECT
public:
    explicit CEventSender(QString sNode, QString sUser, QString sMachine, QString sProfile, QString sStation);
    ~CEventSender();
    bool SendRPSEvent(QString parameter, QString value, QString msg);
    bool SendRPSEvent(QString parameter, int value, QString msg);
    bool SendClusterRPSEvent(QString parameter, QString value, QString msg);
    void ChangeMulticast(QString multicast);
    void ChangePort(int port);

private:
    bool SendRPS1Event(QString parameter, QString value, QString msg);
    bool SendRPS2Event(QString parameter, QString value, QString msg);
    bool CreateSockets();
    void DeleteSockets();
    bool SendEvent(QString eventFrame);

private:
    QUdpSocket *m_pUdpSocket1;
    QUdpSocket *m_pUdpSocket2;
    QHostAddress m_groupAddress;
    QString m_sInterface1;
    QString m_sInterface2;
    QString m_sMCastEventOut;
    uint m_iPortEventOut;
    QString m_sUser;
    QString m_sMachine;
    QString m_sProfile;
    QString m_sStation;
    QString m_sNode;
    QMutex m_mutex;

};

#endif // EVENTSENDER_H
