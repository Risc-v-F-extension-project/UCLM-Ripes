#include <QFile>
#include <QSettings>
#include <QTemporaryDir>
#include <QtTest/QTest>

#include "processorhandler.h"
#include "processorregistry.h"
#include "ripessettings.h"
#include "rvisainfo_common.h"
#include "systemio.h"

using namespace Ripes;

class tst_RISCV_FP_Control : public QObject {
  Q_OBJECT

private:
  QTemporaryDir m_settings;

private slots:
  void initTestCase() {
    QVERIFY(m_settings.isValid());
    // Keep the user's simulator settings untouched, including FU configuration.
    QSettings::setDefaultFormat(QSettings::IniFormat);
    QSettings::setPath(QSettings::IniFormat, QSettings::UserScope,
                       m_settings.path());
    RipesSettings::setValue(RIPES_SETTING_REWINDSTACKSIZE, 0);
    RipesSettings::setValue(RIPES_SETTING_ASSEMBLER_TEXTSTART, 0);
    RipesSettings::setValue(RIPES_SETTING_ASSEMBLER_DATASTART, 0x10000000);
    RipesSettings::setValue(RIPES_SETTING_RV5S_FALU_ADDSUB_LATENCY, 4);
    RipesSettings::setValue(RIPES_SETTING_RV5S_FALU_MUL_LATENCY, 7);
    RipesSettings::setValue(RIPES_SETTING_RV5S_FALU_DIV_LATENCY, 25);
    for (const auto *key : {RIPES_SETTING_RV5S_FALU_ADDSUB_COUNT,
                            RIPES_SETTING_RV5S_FALU_MUL_COUNT,
                            RIPES_SETTING_RV5S_FALU_DIV_COUNT})
      RipesSettings::setValue(key, 1);
    for (const auto *key : {RIPES_SETTING_RV5S_FALU_ADDSUB_PIPELINED,
                            RIPES_SETTING_RV5S_FALU_MUL_PIPELINED,
                            RIPES_SETTING_RV5S_FALU_DIV_PIPELINED})
      RipesSettings::setValue(key, true);
  }

  void test_RV32I_F_control_1slot_no_tomado() {
    runControl(RV32_5S_1S, "control_1slot_not_taken.s", {"F"});
  }

  void test_RV32I_F_control_1slot_retardado() {
    runControl(RV32_5S_1S_DB, "control_1slot_delayed.s", {"F"});
  }

  void test_RV32I_F_control_2slots_no_tomado() {
    runControl(RV32_5S, "control_2slot_not_taken.s", {"F"});
  }

  void test_RV32I_F_control_2slots_retardado() {
    runControl(RV32_5S_2S_DB, "control_2slot_delayed.s", {"F"});
  }

  void test_RV32I_F_control_3slots_no_tomado() {
    runControl(RV32_5S_3S, "control_3slot_not_taken.s", {"F"});
  }

  void test_RV32I_F_control_3slots_retardado() {
    runControl(RV32_5S_3S_DB, "control_3slot_delayed.s", {"F"});
  }


private:
  void runControl(ProcessorID processorId, const QString &fileName,
                     const QStringList &extensions) {
    ProcessorHandler::selectProcessor(processorId, extensions);
    QFile file(QStringLiteral(RIPES_FP_CONTROL_TEST_DIR "/") + fileName);
    QVERIFY2(file.open(QIODevice::ReadOnly), qPrintable(file.errorString()));
    const auto assembled = ProcessorHandler::getAssembler()->assembleRaw(
        QString::fromUtf8(file.readAll()));
    QVERIFY2(assembled.errors.empty(),
             qPrintable(assembled.errors.toString()));
    auto program = std::make_shared<Program>(assembled.program);
    auto *processor = ProcessorHandler::getProcessorNonConst();
    ProcessorHandler::get()->loadProgram(program);
    RipesSettings::getObserver(RIPES_GLOBALSIGNAL_REQRESET)->trigger();

    bool stopped = false;
    uint64_t exitCode = 0;
    uint64_t failedCase = 0;
    QString trapError;
    processor->trapHandler = [&] {
      const auto service = processor->getRegister(RVISA::GPR, 17);
      if (service != 93)
        trapError = QString("Unexpected ECALL service: %1").arg(service);
      exitCode = processor->getRegister(RVISA::GPR, 10);
      failedCase = processor->getRegister(RVISA::GPR, 3);
      stopped = true;
    };

    constexpr unsigned maxCycles = 100000;
    unsigned cycles = 0;
    while (!stopped && cycles < maxCycles) {
      processor->clock();
      ++cycles;
    }
    // Do not retain references to this test's local variables in the processor.
    processor->trapHandler = [] {};
    SystemIO::reset();
    QVERIFY2(stopped, qPrintable(QString("Timeout in %1; gp=%2; cycles=%3")
                                    .arg(fileName)
                                    .arg(processor->getRegister(RVISA::GPR, 3))
                                    .arg(cycles)));
    QVERIFY2(trapError.isEmpty(), qPrintable(trapError));
    QVERIFY2(exitCode == 0,
             qPrintable(QString("%1: failed case %2 (exit=%3), cycle %4")
                            .arg(fileName)
                            .arg(failedCase)
                            .arg(exitCode)
                            .arg(cycles)));
    QCOMPARE(failedCase, uint64_t(5));
  }
};

QTEST_GUILESS_MAIN(tst_RISCV_FP_Control)
#include "tst_riscv_fp_control.moc"
