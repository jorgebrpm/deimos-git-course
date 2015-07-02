#include <QtCore/QCoreApplication>
#include <QDebug>
#include <controller.h>
#include <CLogFile.h>
#include <QNetworkInterface>

#ifdef _LINUX
 #include <signal.h>
#endif

/*********************************************************************
*
* (c) Copyright 2012, Elecnor Deimos Spac L.T.U.
*
* File: $RCSfile: ,v $
* ID: $Id: rew Exp $

*
**********************************************************************/

#ifdef _LINUX
    static void  sigIntHandler(int sig);
#endif

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    QString version=QString("%1.%2").arg(SVersion_t::VERSION).arg(SVersion_t::BUILD);
    QCoreApplication::setApplicationVersion(version);
    QCoreApplication::setOrganizationName(SVersion_t::COMPANY);
    QCoreApplication::setApplicationName("DriverNCRPS");

    if( MainWindow::isAlreadyRunning() )
    {
        _LogFatal << "Another instance is already running";
        return 0;
    }

    CLogFile::getInstance().setComponent(QCoreApplication::applicationName().toStdString());



#ifdef _LINUX
    // Install our tidy-up function
    signal(SIGUSR1, sigIntHandler);
    signal(SIGINT, sigIntHandler);
    signal(SIGTERM, sigIntHandler);
    signal(SIGHUP, sigIntHandler);
    signal(SIGKILL, sigIntHandler);
    signal(SIGUSR1, sigIntHandler);
    signal(SIGSTOP, sigIntHandler);

#endif

    bool bDialog = false;
    if (argc > 1) {
        QString p1 = argv[1];
        if (p1 == "-dlg")
            bDialog = true;
    }

    CLogFile::getInstance().setComponent("DriverNCRPS");
    Controller().Init(bDialog);

    qDebug() << "DriverNCRPS ...";
    
    return a.exec();
    /*
    while (getwchar() != 'x');
    Controller().FinishModule();
    CLogFile::getInstance().releaseInstance();
    */
}

#ifdef _LINUX
    static void sigIntHandler(int sig) {
        printf("SIGINT received ... %d.\n", sig);

        if (sig == SIGUSR1) {
            Controller().ShowControlDialog(true);
            return;
        }

        Controller().FinishModule();
        CLogFile::getInstance().releaseInstance();
        exit(EXIT_SUCCESS);
    } // sigIntHandler

#endif
