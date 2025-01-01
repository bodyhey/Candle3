#include "gcodethreadedloader.h"
#include "gcodeloader.h"
#include <QThread>
#include <QObject>

// class GCodeLoaderWorker : public QObject
// {
//     Q_OBJECT

// public:
//     void abc() {
//         emit progress(1);
//     }
// //     enum class Source {
// //         File,
// //         Lines
// //     };

// //    bject *parent = nullptr) : QObject(parent) {
// //     }

// //     ~GCodeLoaderWorker() {
// //     }

GCodeThreadedLoader::GCodeThreadedLoader(QObject *parent) : AbstractGCodeLoader(parent)
{
}

void GCodeThreadedLoader::loadFromFile(const QString &fileName, ConfigurationParser &configuration)
{
    GCodeLoaderWorker *m_thread = new GCodeLoaderWorker(
        GCodeLoaderWorker::Source::File,
        fileName,
        QStringList()
    );

    connect(m_thread, &GCodeLoaderWorker::progress, this, [this](int value){
        emit progress(value);
    });
    connect(m_thread, &GCodeLoaderWorker::finished, this, [this](GCodeLoaderData* result){
        emit finished(result);
    });
    connect(m_thread, &GCodeLoaderWorker::cancelled, this, [this](){
        emit cancelled();
    });

    // // connect(thread, &GCodeLoaderThread::finished, this, [thread](){
    // //     thread->deleteLater();
    // // }))

    m_thread->run();

    emit started();
}

void GCodeThreadedLoader::loadFromLines(const QStringList &lines, ConfigurationParser &configuration)
{

}

void GCodeThreadedLoader::cancel()
{
    m_thread->requestInterruption();
}

GCodeLoaderWorker::GCodeLoaderWorker(Source source, const QString &fileName, const QStringList &lines, QObject *parent) : QThread(parent)
{
    this->m_source = source;
    this->m_fileName = fileName;
    //this->m_lines = lines;
}

void GCodeLoaderWorker::run() {
    GCodeLoader loader;
    ConfigurationParser configuration(nullptr);

    connect(
        &loader, &GCodeLoader::progress, this,
        [this](int value) { emit progress(value); }, Qt::QueuedConnection);
    connect(
        &loader, &GCodeLoader::finished, this,
        [this](GCodeLoaderData *result) { emit finished(result); },
        Qt::QueuedConnection);
    connect(
        &loader, &GCodeLoader::cancelled, this,
        [this]() { emit cancelled(); }, Qt::QueuedConnection);

    loader.loadFromFile(this->m_fileName, configuration);
}
