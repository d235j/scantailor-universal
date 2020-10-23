/*
    Scan Tailor - Interactive post-processing tool for scanned pages.
    Copyright (C) 2020 Alexander Trufanov <trufanovan@gmail.com>

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/


#ifndef DJVUPAGEGENERATOR_H
#define DJVUPAGEGENERATOR_H

#include <QThread>
#include <QSet>
class QSingleShotExec: public QThread
{
    Q_OBJECT
public:
    QSingleShotExec(const QStringList &commands): m_execCommands(commands) {}

    void run() override {
        for (const QString& cmd: m_execCommands) {

            if (isInterruptionRequested()) {
                break;
            }

  //          qDebug() << cmd.toStdString().c_str() << "\n";
            system(cmd.toStdString().c_str());
//            qDebug() << QThread::currentThreadId();
        }
    }

private:
    QStringList m_execCommands;
};


class DjVuPageGenerator : public QObject
{
    Q_OBJECT
public:
    explicit DjVuPageGenerator(QObject *parent = nullptr);
    ~DjVuPageGenerator();

    void setInputImageFile(const QString &file, const QByteArray& hash);
    void setOutputFile(const QString& file) { m_outputFile = file; }
    void setComands(const QStringList &commands) { m_commands = commands; }

    QString inputFileName() const { return m_inputFile; }
    const QByteArray& inputFileHash() const { return m_inputFileHash; }
    QString outputFileName() const { return m_outputFile; }
    QString executedCommands() const { return m_executedCommands; }

    bool execute();
    void stop();

public slots:
    void executed();

private:
    QString updatedCommands() const;
signals:
    void executionComplete(bool success);
private:
    QString m_inputFile;
    QByteArray m_inputFileHash;
    QStringList m_commands;
    QString m_tempFile;
    QString m_outputFile;
    QString m_executedCommands;
    typedef QPair<QSingleShotExec*, QString> ExecuterHndl;
    QSet< ExecuterHndl > m_executers;
};


#endif // DJVUPAGEGENERATOR_H
