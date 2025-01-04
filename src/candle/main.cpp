// This file is a part of "Candle" application.
// Copyright 2015-2021 Hayrullin Denis Ravilevich
// Copyright 2024 BTS

#include <QApplication>
#include <QDebug>
#include <QOpenGLWidget>
#include <QLocale>
#include <QTranslator>
#include <QFile>
#include <QStyleFactory>
#include <QStyleHints>
#include <QFontDatabase>
#include <QCommandLineParser>

#include "frmmain.h"

void messageHandler(QtMsgType type, const QMessageLogContext &, const QString & msg)
{
    QString txt;
    switch (type) {
        case QtDebugMsg:
            txt = QString("Debug: %1").arg(msg);
            break;
        case QtWarningMsg:
            txt = QString("Warning: %1").arg(msg);
            break;
        case QtCriticalMsg:
            txt = QString("Critical: %1").arg(msg);
            break;
        case QtInfoMsg:
            txt = QString("Info: %1").arg(msg);
            break;
        case QtFatalMsg:
            txt = QString("Fatal: %1").arg(msg);
            abort();
    }

    QFile outFile("GPilot.log");
    outFile.open(QIODevice::WriteOnly | QIODevice::Append);
    QTextStream ts(&outFile);
    ts << txt << Qt::endl;
    QTextStream(stdout) << txt << Qt::endl;
}

void initAppInfo()
{
    QCoreApplication::setApplicationName("G-Pilot");
    QCoreApplication::setOrganizationName("BTS");
}

int main(int argc, char *argv[])
{
#ifdef UNIX
    bool styleOverrided = false;
    for (int i = 0; i < argc; i++) if (QString(argv[i]).toUpper() == "-STYLE") {
        styleOverrided = true;
        break;
    }
#endif

    QApplication app(argc, argv);
    app.setApplicationDisplayName("G-Pilot");
    app.setOrganizationName("G-Pilot");

    QCommandLineParser parser;
    parser.setApplicationDescription("Test helper");
    parser.addHelpOption();
    parser.addVersionOption();

    QCommandLineOption logToFileOption(QStringList{"l", "log-to-file"}, "Log debug info to `GPilot.log`.");
    parser.addOption(logToFileOption);

    parser.process(app);

    if (parser.isSet(logToFileOption)) {
        qInstallMessageHandler(messageHandler);
    }

#ifdef GLES
    QFontDatabase::addApplicationFont(":/fonts/Ubuntu-R.ttf");
#endif
    QSettings set(app.applicationDirPath() + "/settings.ini", QSettings::IniFormat);
    QString loc = set.value("language", "en").toString();

    QString translationsFolder = qApp->applicationDirPath() + "/translations/";
    QString translationFileName = translationsFolder + "candle_" + loc + ".qm";

    if (QFile::exists(translationFileName)) {
        QTranslator* translator = new QTranslator();
        if (translator->load(translationFileName)) app.installTranslator(translator); else delete translator;
    }

    QString baseTranslationFileName = translationsFolder + "qt_" + loc + ".qm";

    if (QFile::exists(translationFileName)) {
        QTranslator* baseTranslator = new QTranslator();
        if (baseTranslator->load(baseTranslationFileName)) app.installTranslator(baseTranslator); else delete baseTranslator;
    }

#ifdef UNIX
    if (!styleOverrided) {
        foreach (QString str, QStyleFactory::keys()) {
            qDebug() << "style" << str;
            if (str.contains("GTK+")) {
                app.setStyle(QStyleFactory::create(str));
                break;
            }
        }
    }
#endif

#ifdef GLES
    app.styleHints()->setColorScheme(Qt::ColorScheme::Light);
    app.setStyle(QStyleFactory::create("Fusion"));

    QPalette palette;
    palette.setColor(QPalette::Highlight, QColor(204, 204, 254));
    palette.setColor(QPalette::HighlightedText, QColor(0, 0, 0));
    app.setPalette(palette);

    app.setStyleSheet("QWidget {font-family: \"Ubuntu\";}\
                    QMenuBar {padding-top: 2px; padding-bottom: 2px;}\
                    QMenuBar::item {spacing: 3px; padding: 2px 8px; background: transparent;}\
                    QMenuBar::item:pressed {border: 1px solid #505050; border-bottom: 1px; border-top-left-radius: 3px; border-top-right-radius: 3px; background: #404040; color: white;}\
                    QToolTip { color: #ffffff; background-color: #2a82da; border: 1px solid white;}\
                    QDialog {border: 1px solid palette(mid);}");
#endif

    app.setStyleSheet(app.styleSheet() + "QWidget {font-size: 8pt}");

    frmMain form;
    form.show();

    return app.exec();
}
