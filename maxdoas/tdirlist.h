#ifndef TDIRLIST_H
#define TDIRLIST_H

#include <QObject>
#include <QDateTime>
#include <QStringList>
#include <QHash>

class TScriptStringList: public QObject
{
Q_OBJECT
public:
    TScriptStringList();
    ~TScriptStringList();

public slots:
    void add(QString t);
    QString at(int index);
    void clear();
    int count();
    bool saveToFile(QString filename);
    bool loadFromFile(QString filename);
private:
    QStringList text;
};

class TFileentry
{
public:
    TFileentry();
    bool loadFileName(QString fn,QString format);
    QString FileName;
    QString Basename;
    QString getGroupName();
    int SequenceNr;
    int groupindex;
    QDateTime DateTime;

    QString directory;
    QString getHashString();
    QString getDirectory();
};

class tdirlist: public QObject
{
Q_OBJECT
public:
    tdirlist(QString dir,int startindex,QString format="CCA");
    tdirlist(TScriptStringList *directories,int startindex,QString format="CCA");
    ~tdirlist();

public slots:
    QString getFileName(QString BaseName, int groupindex, int sequencenr);
    bool gotoNextCompleteGroup(int BaseNameCounts);
    bool gotoNextCompleteSequence(int BaseNameCounts);
    bool gotoNextSequence();
    void gotoStart();
    QString getDir();
    QString getFileName(QString BaseName);
    QString getGroupName();
private:
    void ini(TScriptStringList *dir,int startindex,QString format);
    QHash<QString,TFileentry*> FileTable;
    int SequenceNr;
    int StartSequenceNr;
    int groupindex;
    QString Directory;
};

#endif // TDIRLIST_H
