#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "tfrmtempctrl.h"
#include "thwdriver.h"
#include "tspectrumplotter.h"

#include <qfiledialog.h>
#include <qimagewriter.h>
#include <QSettings>
#include <QCloseEvent>
#include <QDebug>
#include <QFileDialog>
#include <QToolBar>

#include "log4qt/consoleappender.h"
#include "log4qt/logger.h"
#include "log4qt/ttcclayout.h"
#include "tspectrum.h"
#include "tfrmspectrconfig.h"
#include "tscanpath.h"
#define TESTMATRIX 1
#if TESTMATRIX
#include "vectorsolverservicetester.h"
#endif
class RasterData: public QwtMatrixRasterData
{
public:
    RasterData()
    {
        const uint size=32;

        QVector<double> values;
        for ( uint j = 0; j < size ; j++ )
            for ( uint i = 0; i < size ; i++ )
            values += sin((double)i/2)*cos((double)j/3);

        const int numColumns = size;
        setValueMatrix(values, numColumns);

        setInterval( Qt::XAxis,
            QwtInterval( 0, size, QwtInterval::ExcludeMaximum ) );
        setInterval( Qt::YAxis,
            QwtInterval( 0, size, QwtInterval::ExcludeMaximum ) );
        setInterval( Qt::ZAxis, QwtInterval(0, 1) );
    }
};






void MainWindow::setupLog4Qt()
{
    QSettings s;

    // Set logging level for Log4Qt to TRACE

    s.beginGroup("Log4Qt");
    if (!s.contains("Debug")){
        s.setValue("Debug", "ERROR");

        // Configure logging to log to the file C:/myapp.log using the level TRACE
        s.beginGroup("Properties");
        s.setValue("log4j.appender.A1", "org.apache.log4j.FileAppender");
        s.setValue("log4j.appender.A1.file", "log4qt.log");
        s.setValue("log4j.appender.A1.layout", "org.apache.log4j.TTCCLayout");
        s.setValue("log4j.appender.A1.layout.DateFormat", "ISO8601");
        s.setValue("log4j.rootLogger", "ERROR, A1");
    }
    // Settings will become active on next application startup
}



MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    closenow = false;
    HWThreadIsFinished = false;
    ui->setupUi(this);    
    lblComPortStatus = new QLabel("");
    statusBar()->addWidget(lblComPortStatus);
    HWDriver = new THWDriver();
    setupLog4Qt();    // Log first message, which initialises Log4Qt
    logger()->warn("test");
    thread()->setObjectName("Main");
    //Logger("MaxDoas")->name("Main");
    ms = TMaxdoasSettings::instance();

  //  connect(this,SIGNAL(finished()),this,SLOT(on_Finished()));
    //  HWDriver->hwdGetListSpectrometer();
    //ui->cbSpectrList->addItems(HWDriver->hwdGetListSpectrometer());

    TSpectrumPlotter* SpectrumPlotter = TSpectrumPlotter::instance(this);
    SpectrumPlotter->setParentLayout(ui->hbox);

    ScriptEngine = new QScriptEngine();
    ScriptEngine->setProcessEventsInterval(10);
    scriptWrapper = new TScriptWrapper(HWDriver);
    ScriptEngine = scriptWrapper->getScriptEngine();
    ScriptDebugger = new QScriptEngineDebugger;
    ScriptDebugger->attachTo(ScriptEngine);
    ScriptDebugger->setAutoShowStandardWindow(false);
    DebugOutputWidget = ScriptDebugger->widget(QScriptEngineDebugger::DebugOutputWidget);
    DebugOutputWidget->setMaximumHeight(150);
    ui->debugoutLayout->addWidget(DebugOutputWidget);

//    DebugLocalsWidget = ScriptDebugger->widget(QScriptEngineDebugger::LocalsWidget);
//    DebugLocalsWidget->setMaximumWidth(150);
//    ui->code_horizLayout->addWidget(DebugLocalsWidget);
    DebugCodeWidget = ScriptDebugger->widget(QScriptEngineDebugger::CodeWidget);
    ui->code_horizLayout->addWidget(DebugCodeWidget);

    DebugConsoleWidget = ScriptDebugger->widget(QScriptEngineDebugger::ConsoleWidget);
    DebugConsoleWidget->setMaximumWidth(250);
    ui->code_vertLayout->addWidget(DebugConsoleWidget);
    DebugBPWidget = ScriptDebugger->widget(QScriptEngineDebugger::BreakpointsWidget);
    DebugBPWidget->setMaximumWidth(250);

    ui->code_vertLayout->addWidget( DebugBPWidget);

    DebugToolbar =  ScriptDebugger->createStandardToolBar();
    DebugToolbar->setVisible(false);
    ui->horizontalLayout_2->insertWidget(ui->horizontalLayout_2->count()-1,DebugToolbar);

    ScriptEditor = new JSEdit();
    //editor->setWindowTitle(QFileInfo(fileName).fileName());
    ScriptEditor->setFrameShape(JSEdit::NoFrame);
    ScriptEditor->setWordWrapMode(QTextOption::NoWrap);
    ScriptEditor->setTabStopWidth(4);
  //  editor->resize(QApplication::desktop()->availableGeometry().size() / 2);

    // dark color scheme
//    ScriptEditor->setColor(JSEdit::Background,    QColor("#0C152B"));
//    ScriptEditor->setColor(JSEdit::Normal,        QColor("#FFFFFF"));
//    ScriptEditor->setColor(JSEdit::Comment,       QColor("#666666"));
//    ScriptEditor->setColor(JSEdit::Number,        QColor("#DBF76C"));
//    ScriptEditor->setColor(JSEdit::String,        QColor("#5ED363"));
//    ScriptEditor->setColor(JSEdit::Operator,      QColor("#FF7729"));
//    ScriptEditor->setColor(JSEdit::Identifier,    QColor("#FFFFFF"));
//    ScriptEditor->setColor(JSEdit::Keyword,       QColor("#FDE15D"));
//    ScriptEditor->setColor(JSEdit::BuiltIn,       QColor("#9CB6D4"));
//    ScriptEditor->setColor(JSEdit::Cursor,        QColor("#1E346B"));
//    ScriptEditor->setColor(JSEdit::Marker,        QColor("#DBF76C"));
//    ScriptEditor->setColor(JSEdit::BracketMatch,  QColor("#1AB0A6"));
//    ScriptEditor->setColor(JSEdit::BracketError,  QColor("#A82224"));
//    ScriptEditor->setColor(JSEdit::FoldIndicator, QColor("#555555"));

  //  editor->setPlainText(contents);
    ui->ScriptLayout->addWidget(ScriptEditor);
    #if TESTMATRIX
        runVecSolverTest();
    #endif
//     ui->hl->addWidget(mDebugger->widget(QScriptEngineDebugger::ScriptsWidget));



    connect(HWDriver,SIGNAL(hwdSigGotSpectrum()),this,SLOT(on_GotSpectrum()));
    connect(HWDriver,SIGNAL(hwdSigHWThreadFinished()),this,SLOT(HWThreadFinished()));
    connect(HWDriver,SIGNAL(hwdSigCOMPortChanged(QString,bool,bool)),this,SLOT(COMPortChanged(QString,bool,bool)));
    connect(HWDriver,SIGNAL(hwdSigCOMPortChanged(QString,bool,bool)),this,SLOT(COMPortChanged(QString,bool,bool)));

    QMetaObject::invokeMethod(this,
                              "StartMeasure",
                              Qt::QueuedConnection);
}

void MainWindow::StartMeasure(){
    TSPectrWLCoefficients wlcoef;
    TAutoIntegConf ac;
    HWDriver->hwdSetTiltOffset(ms->getTiltOffset());
    HWDriver->hwdSetComPort(ms->getComPortConfiguration());
    HWDriver->hwdSetTargetTemperature(ms->getTargetTemperature());
    HWDriver->hwdOpenSpectrometer(ms->getPreferredSpecSerial());
    wlcoef = ms->getWaveLengthCoefficients(ms->getPreferredSpecSerial());
    HWDriver->hwdOverwriteWLCoefficients(&wlcoef);
    ac = ms->getAutoIntegrationRetrievalConf();
    HWDriver->setIntegrationConfiguration(&ac);
    HWDriver->hwdMeasureSpectrum(1,0,scNone);
}

void MainWindow::on_GotSpectrum(){
    TAutoIntegConf ac = ms->getAutoIntegrationRetrievalConf();
   // HWDriver->hwdGetSpectrum(&spectrum);

//    HWDriver->hwdMeasureSpectrum(2,0,scNone);
}

void MainWindow::COMPortChanged(QString name, bool opened, bool error){
    QString s;
    if (opened)
        s = " opened";
    else
        s = " closed";
    if (error)
        s = s + " (error)";
    lblComPortStatus->setText("COM "+name+s);
}

void MainWindow::on_actionConfigSpectrometer_triggered(){
    TFrmSpectrConfig *tfrmspectrconfig = new TFrmSpectrConfig(HWDriver);
    tfrmspectrconfig->show();
}

void MainWindow::on_actionTempctrler_triggered(){
    TfrmTempctrl *frmtempctrler = new TfrmTempctrl(HWDriver);
    frmtempctrler->show();

}

void MainWindow::onScriptTerminated(){
    if(ScriptEngine->isEvaluating()){
        scriptWrapper->abort();
        QTimer::singleShot(100, this, SLOT(onScriptTerminated()));
    }else
        if (HWThreadIsFinished)
            close();

}

void MainWindow::on_actionClose_triggered(){
    //HWDriver->stop();
    if(ScriptEngine->isEvaluating())
        ScriptEngine->abortEvaluation();
}

void MainWindow::on_actionOpen_triggered(){
    QString fn;
    fn = QFileDialog::getOpenFileName(this,
        tr("Open Script"), ".", tr("Script Files (*.js)"));
    ScriptEditor->loadFromFile(fn);
    DebugToolbar->setVisible(true);
    ScriptEditor->setReadOnly(true);
    ui->BtnStart->setEnabled(false);
    ui->BtnStop->setEnabled(true);
    scriptWrapper->startScriptFile(fn);
    ScriptDebugger->action(QScriptEngineDebugger::InterruptAction)->trigger();
    ScriptDebugger->action(QScriptEngineDebugger::ContinueAction)->trigger();
}

void MainWindow::on_BtnStop_clicked()
{
#if 1
    scriptWrapper->abort();
    ScriptEditor->setReadOnly(false);
    ui->BtnStart->setEnabled(true);
    ui->BtnStop->setEnabled(false);
    DebugToolbar->setVisible(false);
#else
    QFile data("points.txt");
    if (data.open(QFile::WriteOnly | QFile::Truncate)) {
        TScanPath sp(0);
        sp.AddEllipseOutline(QPoint(0.5,0.7),0.8,0.9,45,100);
        sp.AddRect(QPointF(0.1,0.1),QPointF(0.9,0.6),QPointF(0.3,0.2),QPointF(0.5,0.6),QPoint(10,10));
        for (int i=0;i<sp.count();i++){
            TMirrorCoordinate* c;
            c = sp.getPoint(i);
            QTextStream out(&data);
            out << c->getAngleCoordinate().x() << ' '<< c->getAngleCoordinate().y()<<'\n';
                // writes "Result: 3.14      2.7       "
        }
       data.close();

    }

#endif
}

void MainWindow::on_BtnStart_clicked()
{

    ScriptEditor->setReadOnly(true);
    ui->BtnStart->setEnabled(false);
    ui->BtnStop->setEnabled(true);
    DebugToolbar->setVisible(true);
    scriptWrapper->startScript(ScriptEditor->toPlainText());
    ScriptDebugger->action(QScriptEngineDebugger::InterruptAction)->trigger();
    ScriptDebugger->action(QScriptEngineDebugger::ContinueAction)->trigger();

}

void MainWindow::HWThreadFinished(){
    HWThreadIsFinished = true;
    if(!ScriptEngine->isEvaluating())
        close();
}




void MainWindow::closeEvent(QCloseEvent *event){


    if (!closenow){
        HWDriver->stop();
        if(ScriptEngine->isEvaluating()){
            scriptWrapper->abort();
            ScriptDebugger->action(QScriptEngineDebugger::ContinueAction)->trigger();

        }
        event->ignore();
        QTimer::singleShot(100, this, SLOT(onScriptTerminated()));
        closenow = true;
       // QMetaObject::invokeMethod(this,
       //                                      "close",
       //                                      Qt::QueuedConnection);
                //QTimer::singleShot(1000, this, SLOT(close()));
    }else{
        event->accept();
    }


}


MainWindow::~MainWindow()
{
    delete scriptWrapper;
    delete ScriptDebugger;
//    delete marker_corr_top;
//    delete marker_corr_bot;
//    delete marker_target;
//    delete ImagePlot;
//    delete SpectrPlot;
    delete HWDriver;
    delete lblComPortStatus;
    delete ui;
}



