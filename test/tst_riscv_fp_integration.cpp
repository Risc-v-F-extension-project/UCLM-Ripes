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

class tst_RISCV_FP_Integration : public QObject {
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

  }

  void test_RV32I_F_parte1_1slot_no_tomado_segmentadas() {
    runIntegration(RV32_5S_1S, "prueba3.1_F.s",
                   {"F"}, true, 100);
  }

  void test_RV32I_F_parte1_1slot_no_tomado_no_segmentadas() {
    runIntegration(RV32_5S_1S, "prueba3.1_F.s",
                   {"F"}, false, 100);
  }

  void test_RV32I_F_parte1_1slot_retardado_segmentadas() {
    runIntegration(RV32_5S_1S_DB, "prueba3.1_F.s",
                   {"F"}, true, 100);
  }

  void test_RV32I_F_parte1_1slot_retardado_no_segmentadas() {
    runIntegration(RV32_5S_1S_DB, "prueba3.1_F.s",
                   {"F"}, false, 100);
  }

  void test_RV32I_F_parte1_2slots_no_tomado_segmentadas() {
    runIntegration(RV32_5S, "prueba3.1_F.s",
                   {"F"}, true, 100);
  }

  void test_RV32I_F_parte1_2slots_no_tomado_no_segmentadas() {
    runIntegration(RV32_5S, "prueba3.1_F.s",
                   {"F"}, false, 100);
  }

  void test_RV32I_F_parte1_2slots_retardado_segmentadas() {
    runIntegration(RV32_5S_2S_DB, "prueba3.1_F.s",
                   {"F"}, true, 100);
  }

  void test_RV32I_F_parte1_2slots_retardado_no_segmentadas() {
    runIntegration(RV32_5S_2S_DB, "prueba3.1_F.s",
                   {"F"}, false, 100);
  }

  void test_RV32I_F_parte1_3slots_no_tomado_segmentadas() {
    runIntegration(RV32_5S_3S, "prueba3.1_F.s",
                   {"F"}, true, 100);
  }

  void test_RV32I_F_parte1_3slots_no_tomado_no_segmentadas() {
    runIntegration(RV32_5S_3S, "prueba3.1_F.s",
                   {"F"}, false, 100);
  }

  void test_RV32I_F_parte1_3slots_retardado_segmentadas() {
    runIntegration(RV32_5S_3S_DB, "prueba3.1_F.s",
                   {"F"}, true, 100);
  }

  void test_RV32I_F_parte1_3slots_retardado_no_segmentadas() {
    runIntegration(RV32_5S_3S_DB, "prueba3.1_F.s",
                   {"F"}, false, 100);
  }

  void test_RV32I_F_parte2_1slot_no_tomado_segmentadas() {
    runIntegration(RV32_5S_1S, "prueba3.2_F.s",
                   {"F"}, true, 10);
  }

  void test_RV32I_F_parte2_1slot_no_tomado_no_segmentadas() {
    runIntegration(RV32_5S_1S, "prueba3.2_F.s",
                   {"F"}, false, 10);
  }

  void test_RV32I_F_parte2_1slot_retardado_segmentadas() {
    runIntegration(RV32_5S_1S_DB, "prueba3.2_F.s",
                   {"F"}, true, 10);
  }

  void test_RV32I_F_parte2_1slot_retardado_no_segmentadas() {
    runIntegration(RV32_5S_1S_DB, "prueba3.2_F.s",
                   {"F"}, false, 10);
  }

  void test_RV32I_F_parte2_2slots_no_tomado_segmentadas() {
    runIntegration(RV32_5S, "prueba3.2_F.s",
                   {"F"}, true, 10);
  }

  void test_RV32I_F_parte2_2slots_no_tomado_no_segmentadas() {
    runIntegration(RV32_5S, "prueba3.2_F.s",
                   {"F"}, false, 10);
  }

  void test_RV32I_F_parte2_2slots_retardado_segmentadas() {
    runIntegration(RV32_5S_2S_DB, "prueba3.2_F.s",
                   {"F"}, true, 10);
  }

  void test_RV32I_F_parte2_2slots_retardado_no_segmentadas() {
    runIntegration(RV32_5S_2S_DB, "prueba3.2_F.s",
                   {"F"}, false, 10);
  }

  void test_RV32I_F_parte2_3slots_no_tomado_segmentadas() {
    runIntegration(RV32_5S_3S, "prueba3.2_F.s",
                   {"F"}, true, 10);
  }

  void test_RV32I_F_parte2_3slots_no_tomado_no_segmentadas() {
    runIntegration(RV32_5S_3S, "prueba3.2_F.s",
                   {"F"}, false, 10);
  }

  void test_RV32I_F_parte2_3slots_retardado_segmentadas() {
    runIntegration(RV32_5S_3S_DB, "prueba3.2_F.s",
                   {"F"}, true, 10);
  }

  void test_RV32I_F_parte2_3slots_retardado_no_segmentadas() {
    runIntegration(RV32_5S_3S_DB, "prueba3.2_F.s",
                   {"F"}, false, 10);
  }

  void test_RV64I_FD_parte1_1slot_no_tomado_segmentadas() {
    runIntegration(RV64_5S_1S, "prueba3.1_FD.s",
                   {"F", "D"}, true, 100);
  }

  void test_RV64I_FD_parte1_1slot_no_tomado_no_segmentadas() {
    runIntegration(RV64_5S_1S, "prueba3.1_FD.s",
                   {"F", "D"}, false, 100);
  }

  void test_RV64I_FD_parte1_1slot_retardado_segmentadas() {
    runIntegration(RV64_5S_1S_DB, "prueba3.1_FD.s",
                   {"F", "D"}, true, 100);
  }

  void test_RV64I_FD_parte1_1slot_retardado_no_segmentadas() {
    runIntegration(RV64_5S_1S_DB, "prueba3.1_FD.s",
                   {"F", "D"}, false, 100);
  }

  void test_RV64I_FD_parte1_2slots_no_tomado_segmentadas() {
    runIntegration(RV64_5S, "prueba3.1_FD.s",
                   {"F", "D"}, true, 100);
  }

  void test_RV64I_FD_parte1_2slots_no_tomado_no_segmentadas() {
    runIntegration(RV64_5S, "prueba3.1_FD.s",
                   {"F", "D"}, false, 100);
  }

  void test_RV64I_FD_parte1_2slots_retardado_segmentadas() {
    runIntegration(RV64_5S_2S_DB, "prueba3.1_FD.s",
                   {"F", "D"}, true, 100);
  }

  void test_RV64I_FD_parte1_2slots_retardado_no_segmentadas() {
    runIntegration(RV64_5S_2S_DB, "prueba3.1_FD.s",
                   {"F", "D"}, false, 100);
  }

  void test_RV64I_FD_parte1_3slots_no_tomado_segmentadas() {
    runIntegration(RV64_5S_3S, "prueba3.1_FD.s",
                   {"F", "D"}, true, 100);
  }

  void test_RV64I_FD_parte1_3slots_no_tomado_no_segmentadas() {
    runIntegration(RV64_5S_3S, "prueba3.1_FD.s",
                   {"F", "D"}, false, 100);
  }

  void test_RV64I_FD_parte1_3slots_retardado_segmentadas() {
    runIntegration(RV64_5S_3S_DB, "prueba3.1_FD.s",
                   {"F", "D"}, true, 100);
  }

  void test_RV64I_FD_parte1_3slots_retardado_no_segmentadas() {
    runIntegration(RV64_5S_3S_DB, "prueba3.1_FD.s",
                   {"F", "D"}, false, 100);
  }

  void test_RV64I_FD_parte2_1slot_no_tomado_segmentadas() {
    runIntegration(RV64_5S_1S, "prueba3.2_FD.s",
                   {"F", "D"}, true, 10);
  }

  void test_RV64I_FD_parte2_1slot_no_tomado_no_segmentadas() {
    runIntegration(RV64_5S_1S, "prueba3.2_FD.s",
                   {"F", "D"}, false, 10);
  }

  void test_RV64I_FD_parte2_1slot_retardado_segmentadas() {
    runIntegration(RV64_5S_1S_DB, "prueba3.2_FD.s",
                   {"F", "D"}, true, 10);
  }

  void test_RV64I_FD_parte2_1slot_retardado_no_segmentadas() {
    runIntegration(RV64_5S_1S_DB, "prueba3.2_FD.s",
                   {"F", "D"}, false, 10);
  }

  void test_RV64I_FD_parte2_2slots_no_tomado_segmentadas() {
    runIntegration(RV64_5S, "prueba3.2_FD.s",
                   {"F", "D"}, true, 10);
  }

  void test_RV64I_FD_parte2_2slots_no_tomado_no_segmentadas() {
    runIntegration(RV64_5S, "prueba3.2_FD.s",
                   {"F", "D"}, false, 10);
  }

  void test_RV64I_FD_parte2_2slots_retardado_segmentadas() {
    runIntegration(RV64_5S_2S_DB, "prueba3.2_FD.s",
                   {"F", "D"}, true, 10);
  }

  void test_RV64I_FD_parte2_2slots_retardado_no_segmentadas() {
    runIntegration(RV64_5S_2S_DB, "prueba3.2_FD.s",
                   {"F", "D"}, false, 10);
  }

  void test_RV64I_FD_parte2_3slots_no_tomado_segmentadas() {
    runIntegration(RV64_5S_3S, "prueba3.2_FD.s",
                   {"F", "D"}, true, 10);
  }

  void test_RV64I_FD_parte2_3slots_no_tomado_no_segmentadas() {
    runIntegration(RV64_5S_3S, "prueba3.2_FD.s",
                   {"F", "D"}, false, 10);
  }

  void test_RV64I_FD_parte2_3slots_retardado_segmentadas() {
    runIntegration(RV64_5S_3S_DB, "prueba3.2_FD.s",
                   {"F", "D"}, true, 10);
  }

  void test_RV64I_FD_parte2_3slots_retardado_no_segmentadas() {
    runIntegration(RV64_5S_3S_DB, "prueba3.2_FD.s",
                   {"F", "D"}, false, 10);
  }


private:
  void runIntegration(ProcessorID processorId, const QString &fileName,
                     const QStringList &extensions, bool pipelined,
                      unsigned expectedElements) {
    for (const auto *key : {RIPES_SETTING_RV5S_FALU_ADDSUB_COUNT,
                            RIPES_SETTING_RV5S_FALU_MUL_COUNT,
                            RIPES_SETTING_RV5S_FALU_DIV_COUNT})
      RipesSettings::setValue(key, pipelined ? 1 : 2);
    for (const auto *key : {RIPES_SETTING_RV5S_FALU_ADDSUB_PIPELINED,
                            RIPES_SETTING_RV5S_FALU_MUL_PIPELINED,
                            RIPES_SETTING_RV5S_FALU_DIV_PIPELINED})
      RipesSettings::setValue(key, pipelined);
    ProcessorHandler::selectProcessor(processorId, extensions);
    QFile file(QStringLiteral(RIPES_FP_INTEGRATION_TEST_DIR "/") + fileName);
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

    processor->trapHandler = [] {};
    SystemIO::reset();
    QVERIFY2(stopped, qPrintable(QString("Timeout in %1; gp=%2; cycles=%3")
                                    .arg(fileName)
                                    .arg(processor->getRegister(RVISA::GPR, 3))
                                    .arg(cycles)));
    QVERIFY2(trapError.isEmpty(), qPrintable(trapError));
    QVERIFY2(exitCode == 0,
             qPrintable(QString("%1: incorrect result position %2 (exit=%3), cycle %4")
                            .arg(fileName)
                            .arg(failedCase)
                            .arg(exitCode)
                            .arg(cycles)));
    QCOMPARE(failedCase, uint64_t(expectedElements));
  }
};

QTEST_GUILESS_MAIN(tst_RISCV_FP_Integration)
#include "tst_riscv_fp_integration.moc"
