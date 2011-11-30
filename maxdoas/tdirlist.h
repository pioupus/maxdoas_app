#ifndef TDIRLIST_H
#define TDIRLIST_H

#include <QObject>
#include <QDateTime>
#include <QStringList>
#include <QHash>

class TFileentry
{
public:
    TFileentry();
    bool loadFileName(QString fn,QString format);
    QString FileName;
    QString Basename;
    int SequenceNr;
    int groupindex;
    QDateTime DateTime;

    QString getHashString();
};

class tdirlist: public QObject
{
Q_OBJECT
public:
    tdirlist(QString dir,int startindex,QString format="CCA");
    ~tdirlist();

public slots:
    QString getFileName(QString BaseName, int groupindex, int sequencenr);
    bool gotoNextCompleteGroup(int BaseNameCounts);
    bool gotoNextCompleteSequence(int BaseNameCounts);
    bool gotoNextSequence();
    void gotoStart();
    QString getFileName(QString BaseName);
private:
    QHash<QString,TFileentry*> FileTable;
    int SequenceNr;
    int StartSequenceNr;
    int groupindex;
    QString Directory;
};

#endif // TDIRLIST_H
