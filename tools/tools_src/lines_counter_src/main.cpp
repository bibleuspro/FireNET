#include <QCoreApplication>
#include <QDebug>
#include <QFile>

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    qInfo() << "Project lines counter v.1.0";
    qInfo() << "Copyright (c) 2016 Ilya Chernetsov. All rights reserved";

    QString serverFolder = "..\\src\\server";

    QStringList fileList;
    fileList.push_back(serverFolder + "\\clientquerys.cpp");
    fileList.push_back(serverFolder + "\\clientquerys.h");

    fileList.push_back(serverFolder + "\\dbworker.cpp");
    fileList.push_back(serverFolder + "\\dbworker.h");

    fileList.push_back(serverFolder + "\\global.cpp");
    fileList.push_back(serverFolder + "\\global.h");

    fileList.push_back(serverFolder + "\\helper.cpp");
    fileList.push_back(serverFolder + "\\main.cpp");

    fileList.push_back(serverFolder + "\\httpconnector.cpp");
    fileList.push_back(serverFolder + "\\httpconnector.h");

    fileList.push_back(serverFolder + "\\redisconnector.cpp");
    fileList.push_back(serverFolder + "\\redisconnector.h");

    fileList.push_back(serverFolder + "\\mysqlconnector.cpp");
    fileList.push_back(serverFolder + "\\mysqlconnector.h");

    fileList.push_back(serverFolder + "\\remoteclientquerys.cpp");
    fileList.push_back(serverFolder + "\\remoteclientquerys.h");

    fileList.push_back(serverFolder + "\\tcpconnection.cpp");
    fileList.push_back(serverFolder + "\\tcpconnection.h");

    fileList.push_back(serverFolder + "\\tcpserver.cpp");
    fileList.push_back(serverFolder + "\\tcpserver.h");

    fileList.push_back(serverFolder + "\\tcpthread.cpp");
    fileList.push_back(serverFolder + "\\tcpthread.cpp");

    fileList.push_back(serverFolder + "\\remoteserver.cpp");
    fileList.push_back(serverFolder + "\\remoteserver.h");

    fileList.push_back(serverFolder + "\\remoteconnection.cpp");
    fileList.push_back(serverFolder + "\\remoteconnection.h");

    fileList.push_back(serverFolder + "\\settings.cpp");
    fileList.push_back(serverFolder + "\\settings.h");


    int linesCounter = 0;

    for(int i = 0; i < fileList.size(); ++i)
    {
        qDebug() << "File" << fileList [i] << "open for reading...";

        QFile file (fileList[i]);
        int fileCounter = 0;

        if(file.open(QIODevice::ReadOnly))
        {
            QTextStream in (&file);
            QString line;
            do
            {
                line = in.readLine();
                linesCounter++;
                fileCounter ++;

            } while (!line.isNull());

            qDebug() << "File" << fileList [i] << "have" << fileCounter << "lines";
        }
    }

    qDebug() << "Server have" << linesCounter << "lines";

    return a.exec();
}
