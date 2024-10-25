// This file is a part of "G-Pilot (formerly Candle)" application.
// Copyright 2015-2021 Hayrullin Denis Ravilevich
// Copyright 2024 BTS

#include "gcodeloader.h"
#include "parser/gcodeparser.h"
#include <QDebug>

GCodeLoader::GCodeLoader() {}

void GCodeLoader::loadFromFile(const QString &fileName)
{
    QFile file(fileName);

    if (!file.open(QIODevice::ReadOnly)) {
        //        QMessageBox::critical(this, this->windowTitle(), tr("Can't open file:\n") + fileName);
        return;
    }

    // Set filename
    // m_programFileName = fileName;

    // Prepare text stream
    // QTextStream textStream(&file);
    // textStream.setCodec("UTF-8");

    qDebug() << "Size: " << file.size();

    loadFromFileObject(file, file.size());

    file.close();

    // Read lines
    // QList<std::string> data;
    // while (!textStream.atEnd()) {
    //     data.append(textStream.readLine().toStdString());
    // }

    //qDebug() << "Lines: " << count();

    //loadFromString(data);
}

void GCodeLoader::loadFromFileObject(QFile &file, int size)
{
    GCode gCode;

    m_cancelling = false;

    // assert(m_communicator->isMachineConfigurationReady());
    // if (!m_communicator->isMachineConfigurationReady()) {
    //     return;
    // }

    // Reset tables
    gCode.clear();
    // clearTable();
    // m_probeModel.clear();
    // m_programHeightmapModel.clear();
    // updateCurrentModel(&m_programModel);

    // Reset parsers
    // m_viewParser.reset();
    // m_probeParser.reset();

    // Reset code drawer
    // m_currentDrawer = m_codeDrawer;
    // m_codeDrawer->update();
    // ui->glwVisualizer->fitDrawable(m_codeDrawer);
    // updateProgramEstimatedTime(QList<LineSegment*>());

    // Update interface
    // ui->chkHeightMapUse->setChecked(false);
    // ui->grpHeightMap->setProperty("overrided", false);
    // style()->unpolish(ui->grpHeightMap);
    // ui->grpHeightMap->ensurePolished();

    // Reset tableview
    // QByteArray headerState = ui->tblProgram->horizontalHeader()->saveState();
    // ui->tblProgram->setModel(NULL);

    // Prepare parser
    GcodeParser parser;
    // parser.setTraverseSpeed(m_communicator->machineConfiguration().maxRate().x()); // uses only x axis speed
    // if (m_codeDrawer->getIgnoreZ()) parser.reset(QVector3D(qQNaN(), qQNaN(), 0));

    // Block parser updates on table changes
    // m_programLoading = true;

    // Prepare model
    // m_programModel.data().clear();
    // m_programModel.data().reserve(data.count());

    // QProgressDialog progress(tr("Opening file..."), tr("Abort"), 0, data.count(), this);
    // progress.setWindowModality(Qt::WindowModal);
    // progress.setFixedSize(progress.sizeHint());
    // if (data.count() > PROGRESSMINLINES) {
    //     progress.show();
    //     progress.setStyleSheet("QProgressBar {text-align: center; qproperty-format: \"\"}");
    // }

    emit loadingStarted();

    std::string command;
    std::string stripped;
    std::string trimmed;
    QList<QString> args;
    GCodeLine item;
    int remaining = size;

    while (!file.atEnd())
    {
        command = file.readLine().toStdString();

        trimmed = GcodePreprocessorUtils::trimCommand(command);

        if (!trimmed.empty()) {
            // Split command
            stripped = GcodePreprocessorUtils::removeComment(command);
            args = GcodePreprocessorUtils::splitCommand(stripped);

            parser.addCommand(args);

            item.command = QString::fromStdString(trimmed);
            item.state = GCodeLine::InQueue;
            item.lineNumber = parser.getCommandNumber();
            item.args = args;

            gCode.append(item);
        }

        remaining = size - file.pos();

        int percentage = 100 - (remaining * 100 / size);
        static int lastPercentage = 0;
        if (percentage != lastPercentage) {
            lastPercentage = percentage;
            emit progress(percentage);
            qDebug() << "Progress: " + QString::number(percentage) + "%";
        }

        // if (progress.isVisible() && (remaining % PROGRESSSTEP == 0)) {
        //     progress.setValue(progress.maximum() - remaining);
        //     qApp->processEvents();
        //     if (progress.wasCanceled()) break;
        // }

        if (m_cancelling) {
            break;
        }
    }

    qDebug() << "end of file";
    // progress.close();
    // qApp->processEvents();

    // m_programModel.insertRow(m_programModel.rowCount());

    // updateProgramEstimatedTime(
    //     m_viewParser.getLinesFromParser(
    //         &gp,
    //         m_configuration.parserModule().arcApproximationValue(),
    //         m_configuration.parserModule().arcApproximationMode() == ConfigurationParser::ParserArcApproximationMode::ByAngle
    //         )
    //     );

    // m_programLoading = false;

    // Set table model
    // ui->tblProgram->setModel(&m_programModel);
    // ui->tblProgram->horizontalHeader()->restoreState(headerState);

    // Update tableview
    // connect(ui->tblProgram->selectionModel(), SIGNAL(currentChanged(QModelIndex,QModelIndex)), this, SLOT(onTableCurrentChanged(QModelIndex,QModelIndex)));
    // ui->tblProgram->selectRow(0);

    //  Update code drawer
    // m_codeDrawer->update();
    // ui->glwVisualizer->fitDrawable(m_codeDrawer);

    // resetHeightmap();
    // updateControlsState();

    if (m_cancelling) {
        emit loadingCancelled();
    } else {
        emit loadingFinished(gCode);
    }

    qDebug() << "Loaded: " << size;
}


void GCodeLoader::cancelLoading()
{

}
