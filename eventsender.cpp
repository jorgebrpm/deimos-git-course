
#include "eventsender.h"
#include <QNetworkInterface>
#include "mscsframesdefinition.h"
#include <CLogFile.h>
#include "controller.h"

/* -------------------------------------------------------------------- */
/*! @class  :  CEventSender.
*   @brief  :
*   @par    :
*   @return :
*   @note   :
*//* -------------------------------------------------------------------- */
CEventSender::CEventSender(QString sNode,QString sUser, QString sMachine, QString sProfile, QString sStation)
{

    m_sInterface1=Controller().GetInterface1();
    m_sInterface2=Controller().GetInterface2();
    m_sMCastEventOut=Controller().GetMCastEventOut();
    m_iPortEventOut=Controller().GetPortEventOut();
    m_sNode=sNode;
    m_sUser=sUser;
    m_sMachine=sMachine;
    m_sProfile=sProfile;
    m_sStation=sStation;

    if (!CreateSockets()) {
        _LogEE << "Error to create sockets";
    }

}

/* -------------------------------------------------------------------- */
/*! @class  :  CEventSender.
*   @brief  :
*   @par    :
*   @return :
*   @note   :
*//* -------------------------------------------------------------------- */
void CEventSender::ChangeMulticast(QString multicast)
{
    QMutexLocker locker (&m_mutex);
    m_sMCastEventOut=multicast;
    DeleteSockets();
    if (!CreateSockets()) {
        _LogEE << "Error to create sockets";
    }
}

/* -------------------------------------------------------------------- */
/*! @class  :  CEventSender.
*   @brief  :
*   @par    :
*   @return :
*   @note   :
*//* -------------------------------------------------------------------- */
void CEventSender::ChangePort(int port)
{
    QMutexLocker locker (&m_mutex);
    m_iPortEventOut=port;
}

/* -------------------------------------------------------------------- */
/*! @class  :  CEventSender.
*   @brief  :
*   @par    :
*   @return :
*   @note   :
*//* -------------------------------------------------------------------- */
void CEventSender::DeleteSockets()
{
    if (m_pUdpSocket1) {
        m_pUdpSocket1->close();
        delete m_pUdpSocket1;
        m_pUdpSocket1 = NULL;
    }

    if (m_pUdpSocket2) {
        m_pUdpSocket2->close();
        delete m_pUdpSocket2;
        m_pUdpSocket2 = NULL;
    }

}

/* -------------------------------------------------------------------- */
/*! @class  :  CEventSender.
*   @brief  :
*   @par    :
*   @return :
*   @note   :
*//* -------------------------------------------------------------------- */
CEventSender::~CEventSender()
{
    DeleteSockets();
}


/* -------------------------------------------------------------------- */
/*! @class  :  CEventSender.
*   @brief  :
*   @par    :
*   @return :
*   @note   :
*//* -------------------------------------------------------------------- */
bool CEventSender::SendRPSEvent(QString parameter, int value, QString msg) {

    bool res=false;
    QString valueStr=QString("%1").arg(value);
    if (m_sNode=="rps1")  res=SendRPS1Event(parameter, valueStr, msg);
    if (m_sNode=="rps2")  res=SendRPS2Event(parameter, valueStr, msg);
    return res;
}


/* -------------------------------------------------------------------- */
/*! @class  :  CEventSender.
*   @brief  :
*   @par    :
*   @return :
*   @note   :
*//* -------------------------------------------------------------------- */
bool CEventSender::SendRPSEvent(QString parameter, QString value, QString msg) {

    bool res=false;
    if (m_sNode=="rps1")  res=SendRPS1Event(parameter, value, msg);
    if (m_sNode=="rps2")  res=SendRPS2Event(parameter, value, msg);
    return res;
}


/* -------------------------------------------------------------------- */
/*! @class  :  CEventSender.
*   @brief  :
*   @par    :
*   @return :
*   @note   :
*//* -------------------------------------------------------------------- */
bool CEventSender::SendClusterRPSEvent(QString parameter, QString value, QString msg) {
     FrameEv frame;
     QString event = frame.GetEventRPSFrame(m_sUser, m_sMachine, m_sProfile, m_sStation, parameter, value, msg);
     return this->SendEvent(event);
}

/* -------------------------------------------------------------------- */
/*! @class  :  CEventSender.
*   @brief  :
*   @par    :
*   @return :
*   @note   :
*//* -------------------------------------------------------------------- */
bool CEventSender::SendRPS1Event(QString parameter, QString value, QString msg) {
     FrameEv frame;
     QString event = frame.GetEventRPS1Frame(m_sUser, m_sMachine, m_sProfile, m_sStation, parameter, value, msg);
     return this->SendEvent(event);
}

/* -------------------------------------------------------------------- */
/*! @class  :  CSpxManager.
*   @brief  :
*   @par    :
*   @return :
*   @note   :
*//* -------------------------------------------------------------------- */
bool CEventSender::SendRPS2Event(QString parameter, QString value, QString msg) {
     FrameEv frame;
     QString event = frame.GetEventRPS2Frame(m_sUser, m_sMachine, m_sProfile, m_sStation, parameter, value, msg);
     return this->SendEvent(event);
}



/* -------------------------------------------------------------------- */
/*! @class  :  CEventSender.
*   @brief  :
*   @par    :
*   @return :
*   @note   :
*//* -------------------------------------------------------------------- */
bool CEventSender::SendEvent(QString eventFrame) {

    bool res=false;
    int resL1=SGeneralStatus_t::UnKnown;
    int resL2=SGeneralStatus_t::UnKnown;

    QMutexLocker locker (&m_mutex);

    if (m_pUdpSocket1) {
        qDebug() << eventFrame;
        qint64 iWrites1 = m_pUdpSocket1->writeDatagram(eventFrame.toAscii(), eventFrame.length(), m_groupAddress, m_iPortEventOut);
        resL1=SGeneralStatus_t::Enable;
        if (iWrites1 <= 0) {
            QString ss = QString("Error to send data track from Lan 1: %1").arg(m_pUdpSocket1->errorString());
            qDebug() << ss;
            _LogEE << ss;
            resL1=SGeneralStatus_t::Disable;
        }
    }

    if (m_pUdpSocket2) {
        qDebug() << eventFrame;
        resL2=SGeneralStatus_t::Enable;
        qint64 iWrites2 = m_pUdpSocket2->writeDatagram(eventFrame.toAscii(), eventFrame.length(), m_groupAddress, m_iPortEventOut);
        if (iWrites2 <= 0) {
            QString ss = QString("Error to send data track from Lan 2: %1").arg(m_pUdpSocket2->errorString());
            qDebug() << ss;
            _LogEE << ss;
            resL2=SGeneralStatus_t::Disable;
        }
    }
    if ((resL1==SGeneralStatus_t::Enable) || (resL2==SGeneralStatus_t::Enable)) res=true;
    else
        res=false;

    return res;
}

/* -------------------------------------------------------------------- */
/*! @class  :  CEventSender.
*   @brief  :
*   @par    :
*   @return :
*   @note   :
*//* -------------------------------------------------------------------- */
bool CEventSender::CreateSockets() {
    bool ret(false);

    if (!m_sMCastEventOut.isEmpty()) {
        m_groupAddress = QHostAddress(m_sMCastEventOut);

        m_pUdpSocket1 = new QUdpSocket();
        m_pUdpSocket1->setSocketOption(QAbstractSocket::MulticastTtlOption, 33);

        m_pUdpSocket2 = new QUdpSocket();
        m_pUdpSocket2->setSocketOption(QAbstractSocket::MulticastTtlOption, 33);

        if (m_iPortEventOut != 0) {
            m_pUdpSocket1->bind(m_iPortEventOut, QUdpSocket::ShareAddress | QUdpSocket::ReuseAddressHint);
            m_pUdpSocket2->bind(m_iPortEventOut, QUdpSocket::ShareAddress | QUdpSocket::ReuseAddressHint);

            QList<QNetworkInterface> il(QNetworkInterface::allInterfaces());

            bool bInterface1 = false, bInterface2 = false;

            for (int i = 0; i < il.size(); i++) {

                QList<QNetworkAddressEntry> ade(il[i].addressEntries());
                for (int j = 0; j < ade.size(); j++) {
                    QString ifNet = ade[j].ip().toString();

                    QNetworkInterface ifObj = il[i];

                    if (ifNet == m_sInterface1) {
                        m_pUdpSocket1->setMulticastInterface(ifObj);
                        m_pUdpSocket1->joinMulticastGroup(m_groupAddress, ifObj);
                        bInterface1 = !bInterface1;
                        break;
                    }
                    else if (ifNet == m_sInterface2) {
                        m_pUdpSocket2->setMulticastInterface(ifObj);
                        m_pUdpSocket2->joinMulticastGroup(m_groupAddress, ifObj);
                        bInterface2 = !bInterface2;
                        break;
                    }
                }
            }
            ret = (bInterface1 && bInterface2);
            if (ret) {
                _LogII << "Socket output created successfully";
            }
            else {
                _LogEE << "Sockets outputs error to configure interfaces";
            }
        }
        else {
            _LogEE << "Error to create socket. Port output is 0";
        }
    }
    else {
        _LogEE << "Error to create socket. Multicast output is empty";
    }


    return ret;
}
