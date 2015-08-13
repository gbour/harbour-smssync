/*
/*
  Copyright (C) 2015 Guillaume Bour.
  Contact: Guillaume Bour <buillaume@bour.cc>
  All rights reserved.

*/

#include <signal.h>
#include <QCoreApplication>
#include <QLoggingCategory>

int main(int argc, char *argv[])
{
    QCoreApplication app(argc, argv);
    app.setApplicationName("smssync");
    app.setOrganizationName("gbour");

    QLoggingCategory l("main");
    qCDebug(l) << argv[0] << "0.1";

    return app.exec();
}

