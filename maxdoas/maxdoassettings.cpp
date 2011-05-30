
#include <QSettings>
#include <QString>

QSettings settings;


void ConfInit(){
    //settings = QSettings();
}

QString ConfGetPreferredSpectrSerial(){
    return settings.value("Spectrometer/PreferredSerial","");
}
