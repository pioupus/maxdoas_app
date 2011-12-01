#include "tdirlist.h"
#include <QDir>
#include <QMap>

QString getHash(int gi,int seq){
    return QString::number(gi).rightJustified(4, '0')+QString::number(seq).rightJustified(4, '0');
}

TFileentry::TFileentry(){
    FileName = "";
    Basename = "";
    SequenceNr =-1;
    groupindex = -1;
}

bool TFileentry::loadFileName(QString fn,QString format){
    FileName = fn;
    bool result=false;
    if (format == "CCA"){
        Basename = fn.left(fn.indexOf("_"));
        QString s = fn.mid(fn.indexOf("_")+1,20);
        DateTime =  QDateTime::fromString(s,"yyyy_MM_dd__hh_mm_ss");
        s = fn.mid(fn.indexOf("_seq")+4);
        s = s.left(s.indexOf("s"));
        SequenceNr = s.toInt();
        result = true;
    }
    if (format == "SIGIS"){
                                                                        //Haz_(2006_03_17_12_16_16_968)_step10_image003.bmp_ppm.txt
        fn = fn.section("(",1,1);                               //2006_03_17_12_16_16_968)_step10_image003.bmp_ppm.txt
        QString s = fn.left(fn.indexOf(")"));                             //2006_03_17_12_16_16_968

        DateTime =  QDateTime::fromString(s,"yyyy_MM_dd_hh_mm_ss_zzz");
        s = fn.mid(fn.indexOf("image")+5);                  //003.bmp_ppm.txt
        Basename = s;
        s = s.left(s.indexOf("."));                                     //003
        SequenceNr = s.toInt();
        Basename = Basename.mid(Basename.indexOf("_")+1);
        Basename = Basename.left(Basename.indexOf("."));
        result = true;
    }
    return result;
}

QString TFileentry::getHashString(){
    return getHash(groupindex,SequenceNr);
}

QString getDateString(QString s,QString format){
    if (format == "CCA"){
        return s.mid(s.indexOf("_")+1,20);
    }
    if (format == "SIGIS"){
        s = s.section("(",1,1);
        return s.left(s.indexOf(")"));
    }
    return "";
}

tdirlist::tdirlist(QString dir,int startindex,QString format)
{
    this->StartSequenceNr = startindex;
    QString datfmt = "yyyy_MM_dd__hh_mm_ss";
    QString filtermask = "*s.spe";

    gotoStart();
    QDir dr(dir);
    Directory = dr.absolutePath();
    if (format == "CCA"){
        datfmt = "yyyy_MM_dd__hh_mm_ss";
        filtermask = "*s.spe";
    }
    if (format == "SIGIS"){
        datfmt = "yyyy_MM_dd_hh_mm_ss_zzz";
        filtermask = "*.txt";
    }
    QStringList filter;
    filter.append(filtermask);
    QStringList dl = dr.entryList(filter);
    QMap<QDateTime,QString> filelist;
    for (int i=0;i<dl.count();i++){
        QString s = dl[i];
        s = getDateString(s,format);
        QDateTime dt =  QDateTime::fromString(s,datfmt);
        filelist.insertMulti(dt,dl[i]);
    }
    QMapIterator<QDateTime,QString> mi(filelist);
    int groupindex = 0;
    int oldseq = startindex;
    QStringList BaseNames;
    while (mi.hasNext()) {
        mi.next();
        TFileentry* fe = new TFileentry();
        fe->loadFileName(mi.value(),format);
        bool samegroup = (oldseq == fe->SequenceNr) && (BaseNames.indexOf(fe->Basename) == -1);
        if (oldseq+1 == fe->SequenceNr){
            BaseNames.clear();
            samegroup = true;
        }
        if (!samegroup){
            groupindex++;
            BaseNames.clear();
        }
        BaseNames.append(fe->Basename);
        oldseq = fe->SequenceNr;
        fe->groupindex = groupindex;
        FileTable.insertMulti(fe->getHashString(),fe);
    }
}

tdirlist::~tdirlist(){
    QHashIterator<QString, TFileentry*> i(FileTable);
    while (i.hasNext()) {
        TFileentry* fe;
        i.next();
        fe = i.value();
        delete fe;
    }
    FileTable.clear();
}

bool tdirlist::gotoNextCompleteGroup(int BaseNameCounts){
    bool ok = true;
    while (ok){
        groupindex++;
        QString hash = getHash(groupindex,SequenceNr);
        if (!FileTable.contains(hash))
            ok = false;
        int c = FileTable.count(hash);
        if (c==BaseNameCounts){
            break;
        }
    }
    return ok;
}

bool tdirlist::gotoNextCompleteSequence(int BaseNameCounts){
    bool ok = true;
    while (ok){
        ok = gotoNextSequence();
        if (ok){
            QString hash = getHash(groupindex,SequenceNr);
            int c = FileTable.count(hash);
            if (c==BaseNameCounts){
                break;
            }
        }
    }
    return ok;
}

bool tdirlist::gotoNextSequence(){
    SequenceNr++;
    QString hash;
    hash = getHash(groupindex,SequenceNr);
    if (FileTable.contains(hash))
        return true;
    else{
        groupindex++;
        SequenceNr = StartSequenceNr;
        hash = getHash(groupindex,SequenceNr);
        if (FileTable.contains(hash))
            return true;
        else
            return false;
    }
}

void tdirlist::gotoStart(){
    this->SequenceNr=StartSequenceNr;
    this->groupindex=-1;
}

QString tdirlist::getFileName(QString BaseName){
    return getFileName(BaseName,groupindex,SequenceNr);
}

QString tdirlist::getFileName(QString BaseName, int groupindex, int sequencenr){
    QString hash = getHash(groupindex,sequencenr);
    QList<TFileentry*> subgroup = FileTable.values(hash);
    QString result;
    for(int i = 0;i<subgroup.count();i++){
        if (subgroup[i]->Basename == BaseName){
            result = Directory+"/"+subgroup[i]->FileName;
            break;
        }
    }

    return result;

}
