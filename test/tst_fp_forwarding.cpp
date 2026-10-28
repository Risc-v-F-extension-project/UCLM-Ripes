#include <QtTest/QTest>
#include <QSettings>
#include <QTemporaryDir>

#include "processorhandler.h"
#include "programloader.h"
#include "processors/RISC-V/rv5s/rv5s.h"

using namespace Ripes;
using namespace vsrtl::core;

class ForwardingDesign : public Design {
public:
  ForwardingDesign() : Design("forwarding") {
    unit->setFPExtensionEnabled(true);
    3 >> unit->id_reg1_idx;
    0 >> unit->id_reg2_idx;
    0 >> unit->mem_reg_wr_en;
    0 >> unit->wb_reg_wr_en;
    0 >> unit->wb_mem_op;
    memIndexReg->out >> unit->mem_reg_wr_idx;
    memWriteReg->out >> unit->mem_fp_reg_wr_en;
    wbIndexReg->out >> unit->wb_reg_wr_idx;
    wbWriteReg->out >> unit->wb_fp_reg_wr_en;
    0 >> memIndexReg->in;
    0 >> memWriteReg->in;
    0 >> wbIndexReg->in;
    0 >> wbWriteReg->in;
    verifyAndInitialize();
  }
  void update() {
    setSynchronousValue(memIndexReg, 0, memIndex);
    setSynchronousValue(memWriteReg, 0, memWrite);
    setSynchronousValue(wbIndexReg, 0, wbIndex);
    setSynchronousValue(wbWriteReg, 0, wbWrite);
  }
  unsigned memIndex = 3, wbIndex = 3;
  bool memWrite = false, wbWrite = false;
  SUBCOMPONENT(unit, RV5SForwardingUnit);
  SUBCOMPONENT(memIndexReg, TYPE(Register<5>));
  SUBCOMPONENT(memWriteReg, TYPE(Register<1>));
  SUBCOMPONENT(wbIndexReg, TYPE(Register<5>));
  SUBCOMPONENT(wbWriteReg, TYPE(Register<1>));
};

class tst_fp_forwarding : public QObject {
  Q_OBJECT
private slots:
  void priority() {
    ForwardingDesign design;
    for (unsigned bits = 0; bits < 16; ++bits) {
      design.memWrite = bits & 1;
      design.wbWrite = bits & 2;
      design.memIndex = bits & 4 ? 3 : 0;
      design.wbIndex = bits & 8 ? 3 : 0;
      design.update();
      for (unsigned index : {3u, 0u}) {
        const bool mem = design.memWrite && design.memIndex == index;
        const bool wb = design.wbWrite && design.wbIndex == index;
        const auto expected = wb && !mem ? FPForwardingSrc::WbStage : FPForwardingSrc::IdStage;
        const auto actual = index == 3
            ? design.unit->falu_reg1_forwarding_ctrl.eValue<FPForwardingSrc>()
            : design.unit->falu_reg2_forwarding_ctrl.eValue<FPForwardingSrc>();
        QCOMPARE(actual, expected);
      }
    }
  }

  void consumers_data() {
    QTest::addColumn<ProcessorID>("processor");
    QTest::addColumn<unsigned>("latency");
    QTest::addColumn<bool>("pipelined");
    for (auto id : {ProcessorID::RV32_5S, ProcessorID::RV64_5S})
      for (unsigned latency : {2u, 4u, 7u})
        for (bool pipelined : {false, true})
          QTest::addRow("%d-%u-%d", int(id), latency, int(pipelined))
              << id << latency << pipelined;
  }

  void configurations_data() {
    QTest::addColumn<ProcessorID>("processor");
    QTest::addColumn<bool>("floating");
    for (auto id : {ProcessorID::RV32_5S_1S, ProcessorID::RV32_5S_1S_DB,
                    ProcessorID::RV32_5S_2S_DB, ProcessorID::RV32_5S_3S,
                    ProcessorID::RV32_5S_3S_DB, ProcessorID::RV64_5S_1S,
                    ProcessorID::RV64_5S_1S_DB, ProcessorID::RV64_5S_2S_DB,
                    ProcessorID::RV64_5S_3S, ProcessorID::RV64_5S_3S_DB})
      QTest::addRow("legacy-F-%d", int(id)) << id << true;
    for (auto id : {ProcessorID::RV32_5S, ProcessorID::RV64_5S})
      QTest::addRow("standard-no-F-%d", int(id)) << id << false;
  }

  void configurations() {
    QFETCH(ProcessorID, processor);
    QFETCH(bool, floating);
    ProcessorHandler::get()->selectProcessor(
        processor, floating ? QStringList{"F"} : QStringList{});
    ProgramLoader loader;
    loader.loadTest(floating ? ".text\nfadd.s f3, f1, f2\nnop\nnop\nnop\nnop\nnop\n"
                             : ".text\naddi x3, x0, 7\naddi x4, x3, 5\n");
    auto *proc = ProcessorHandler::get()->getProcessorNonConst();
    if (floating) {
      const uint64_t boxing = proc->implementsISA()->bits() == 64
                                  ? 0xffffffff00000000ULL : 0;
      proc->setRegister(RVISA::FPR, 1, boxing | 0x3f800000u);
      proc->setRegister(RVISA::FPR, 2, boxing | 0x40000000u);
    }
    for (unsigned cycle = 0; cycle < 80 && !proc->finished(); ++cycle)
      proc->clock();
    QVERIFY(proc->finished());
    if (!floating) {
      QCOMPARE(proc->getRegister(RVISA::GPR, 4), 12);
      return;
    }
    QCOMPARE(uint32_t(proc->getRegister(RVISA::FPR, 3)), 0x40400000u);
    auto *design = dynamic_cast<Design *>(proc);
    QVERIFY(design);
    bool checked = false;
    for (auto *component : design->getSubComponents()) {
      QVERIFY(!dynamic_cast<RV5SForwardingUnit *>(component));
      if (component->getName() != "idex_reg") continue;
      for (auto *child : component->getSubComponents()) {
        const auto checkLegacy = [&](const auto *state) {
          QVERIFY(state->registers()[3].anticipated);
          QCOMPARE(uint32_t(state->resolvedValue(3, 0x12345678)), 0x40400000u);
          checked = true;
        };
        if (auto *state = dynamic_cast<TemporaryFPRegisterState<32> *>(child))
          checkLegacy(state);
        if (auto *state = dynamic_cast<TemporaryFPRegisterState<64> *>(child))
          checkLegacy(state);
      }
    }
    QVERIFY(checked);
  }

  void consumers() {
    QFETCH(ProcessorID, processor);
    QFETCH(unsigned, latency);
    QFETCH(bool, pipelined);
    ProcessorHandler::get()->selectProcessor(processor, {"F"});
    const auto configure = [latency, pipelined](auto *cpu) {
      cpu->hzunit->setConfiguration(latency, 2, 2, 1, 1, 1,
                                   pipelined, pipelined, pipelined);
    };
    auto *selected = ProcessorHandler::get()->getProcessorNonConst();
    if (auto *cpu = dynamic_cast<RV5S<uint32_t> *>(selected)) configure(cpu);
    else configure(dynamic_cast<RV5S<uint64_t> *>(selected));
    RipesSettings::getObserver(RIPES_GLOBALSIGNAL_REQRESET)->trigger();
    auto proc = ProcessorHandler::get()->getProcessorNonConst();
    proc->setMaxReverseCycles(128);
    proc->trapHandler = [] {};
    ProgramLoader loader;
    loader.loadTest(QStringLiteral(
        ".data\nvalues: .float 1.0 2.0\n.text\n"
        "la x1, values\nflw f1, 0(x1)\nflw f2, 4(x1)\n"
        "fadd.s f3, f1, f2\n"
        "fadd.s f4, f3, f1\nfmul.s f5, f3, f2\n"
        "fadd.s f6, f1, f3\nfadd.s f7, f2, f3\n"
        // Consecutive writers: WB must not override the newer MEM result.
        "fadd.s f8, f1, f1\nfadd.s f8, f2, f2\n"
        "fadd.s f9, f8, f1\n"
        // A load must override an earlier arithmetic temporary.
        "flw f3, 0(x1)\nfadd.s f10, f3, f2\n"
        "nop\nnop\nnop\nnop\nnop\n"));
    auto *design = dynamic_cast<Design *>(proc);
    QVERIFY(design);
    TemporaryFPRegisterState<32> *state32 = nullptr;
    TemporaryFPRegisterState<64> *state64 = nullptr;
    // Find the temporal state through the simulator's public component tree.
    for (auto *component : design->getSubComponents()) {
      if (component->getName() != "idex_reg") continue;
      for (auto *child : component->getSubComponents()) {
        if (child->getName() != "temporaryFPRegisters") continue;
        state32 = dynamic_cast<TemporaryFPRegisterState<32> *>(child);
        state64 = dynamic_cast<TemporaryFPRegisterState<64> *>(child);
      }
    }
    QVERIFY(state32 || state64);
    auto *cpu32 = dynamic_cast<RV5S<uint32_t> *>(proc);
    auto *cpu64 = dynamic_cast<RV5S<uint64_t> *>(proc);
    auto snapshot = [&] {
      QByteArray result;
      const auto append = [&](uint64_t value) { result.append(reinterpret_cast<const char *>(&value), sizeof(value)); };
      append(proc->getCycleCount());
      for (unsigned i = 0; i < 32; ++i) append(proc->getRegister(RVISA::FPR, i));
      const auto appendState = [&](const auto &state) {
        for (const auto &reg : state.registers()) {
          append(reg.anticipated); append(reg.value); append(reg.emissionCycle);
        }
        // The physical operand must win outside the single capture window.
        for (unsigned i = 0; i < 32; ++i) {
          const auto &reg = state.registers()[i];
          const auto resolved = state.resolvedValue(i, 0x12345678);
          if (!reg.anticipated || reg.emissionCycle != proc->getCycleCount() + 1)
            Q_ASSERT(resolved == 0x12345678);
          append(resolved);
        }
      };
      if (state32) appendState(*state32); else appendState(*state64);
      const auto appendPipeline = [&](const auto *cpu) {
        append(cpu->hzunit->currentCycle());
        Q_ASSERT(cpu->exmem_reg->valid_out.uValue() ||
                 !cpu->exmem_reg->fp_reg_do_write_out.uValue());
        Q_ASSERT(cpu->memwb_reg->valid_out.uValue() ||
                 !cpu->memwb_reg->fp_reg_do_write_out.uValue());
        append(cpu->idex_reg->f_r1_out.uValue());
        append(cpu->idex_reg->f_r2_out.uValue());
        append(cpu->funit->falu_reg1_forwarding_ctrl.uValue());
        append(cpu->funit->falu_reg2_forwarding_ctrl.uValue());
        append(cpu->falu->op1.uValue());
        append(cpu->falu->op2.uValue());
      };
      if (cpu32) appendPipeline(cpu32); else appendPipeline(cpu64);
      return result;
    };
    bool sawTemporary = false, sawWriteback = false, sawPhysical = false;
    const auto observe = [&](const auto *cpu) {
      if (!cpu->idex_reg->valid_out.uValue() ||
          !cpu->idex_reg->fp_reg_do_write_out.uValue()) return;
      const auto destination = cpu->idex_reg->wr_reg_idx_out.uValue();
      const auto source = cpu->funit->falu_reg1_forwarding_ctrl.uValue();
      if (destination == 4) {
        QCOMPARE(source, unsigned(FPForwardingSrc::IdStage));
        QCOMPARE(uint32_t(cpu->idex_reg->f_r1_out.uValue()), 0x40400000u);
        sawTemporary = true;
      } else if (destination == 5) {
        QCOMPARE(source, unsigned(FPForwardingSrc::WbStage));
        QCOMPARE(uint32_t(cpu->falu->op1.uValue()), 0x40400000u);
        sawWriteback = true;
      } else if (destination == 6) {
        QCOMPARE(source, unsigned(FPForwardingSrc::IdStage));
        QCOMPARE(uint32_t(cpu->idex_reg->f_r2_out.uValue()), 0x40400000u);
        sawPhysical = true;
      }
    };
    QList<QByteArray> snapshots{snapshot()};
    for (unsigned cycle = 0; cycle < 100 && !proc->finished(); ++cycle) {
      proc->clock();
      snapshots.append(snapshot());
      if (cpu32) observe(cpu32); else observe(cpu64);
    }
    QVERIFY(proc->finished());
    QVERIFY(sawTemporary && sawWriteback && sawPhysical);
    const auto fp = [&](unsigned index) { return uint32_t(proc->getRegister(RVISA::FPR, index)); };
    QCOMPARE(fp(4), 0x40800000u); // 4
    QCOMPARE(fp(5), 0x40c00000u); // 6
    QCOMPARE(fp(6), 0x40800000u);
    QCOMPARE(fp(7), 0x40a00000u);
    QCOMPARE(fp(9), 0x40a00000u); // newest f8 = 4
    QCOMPARE(fp(10), 0x40400000u); // loaded f3 = 1
    for (int cycle = int(snapshots.size()) - 2; cycle >= 0; --cycle) {
      proc->reverseProcessor();
      const auto restored = snapshot();
      if (restored != snapshots[cycle]) {
        for (int offset = 0; offset < restored.size(); offset += 8)
          if (restored.mid(offset, 8) != snapshots[cycle].mid(offset, 8))
            qWarning() << "Reverse difference" << cycle << offset
                       << restored.mid(offset, 8).toHex()
                       << snapshots[cycle].mid(offset, 8).toHex();
      }
      QCOMPARE(restored, snapshots[cycle]);
    }
    for (int cycle = 1; cycle < snapshots.size(); ++cycle) {
      proc->clock();
      const auto replayed = snapshot();
      if (replayed != snapshots[cycle]) {
        for (int offset = 0; offset < replayed.size(); offset += 8)
          if (replayed.mid(offset, 8) != snapshots[cycle].mid(offset, 8))
            qWarning() << "Replay difference" << cycle << offset
                       << replayed.mid(offset, 8).toHex()
                       << snapshots[cycle].mid(offset, 8).toHex();
      }
      QCOMPARE(replayed, snapshots[cycle]);
    }
  }
};

int main(int argc, char **argv) {
  QApplication app(argc, argv);
  QTemporaryDir settingsDirectory;
  QSettings::setDefaultFormat(QSettings::IniFormat);
  QSettings::setPath(QSettings::IniFormat, QSettings::UserScope,
                     settingsDirectory.path());
  tst_fp_forwarding test;
  return QTest::qExec(&test, argc, argv);
}
#include "tst_fp_forwarding.moc"
