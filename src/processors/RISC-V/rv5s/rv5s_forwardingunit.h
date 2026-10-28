#pragma once

#include "processors/RISC-V/riscv.h"

#include "VSRTL/core/vsrtl_component.h"

namespace Ripes {
enum class ForwardingSrc { IdStage, MemStage, WbStage };
enum class FPForwardingSrc { IdStage, WbStage };
}

namespace vsrtl {
namespace core {
using namespace Ripes;

class ForwardingUnit : public Component {
public:
  ForwardingUnit(const std::string &name, SimComponent *parent)
      : Component(name, parent) {
    alu_reg1_forwarding_ctrl << [this] {
      const auto idx = id_reg1_idx.uValue();
      if (idx == 0) {
        return ForwardingSrc::IdStage;
      } else if (idx == mem_reg_wr_idx.uValue() && mem_reg_wr_en.uValue()) {
        return ForwardingSrc::MemStage;
      } else if (idx == wb_reg_wr_idx.uValue() && wb_reg_wr_en.uValue()) {
        return ForwardingSrc::WbStage;
      } else {
        return ForwardingSrc::IdStage;
      }
    };

    alu_reg2_forwarding_ctrl << [this] {
      const auto idx = id_reg2_idx.uValue();
      if (idx == 0) {
        return ForwardingSrc::IdStage;
      } else if (idx == mem_reg_wr_idx.uValue() && mem_reg_wr_en.uValue()) {
        return ForwardingSrc::MemStage;
      } else if (idx == wb_reg_wr_idx.uValue() && wb_reg_wr_en.uValue()) {
        return ForwardingSrc::WbStage;
      } else {
        return ForwardingSrc::IdStage;
      }
    };

    falu_reg1_forwarding_ctrl << [this] {
      return selectFPSource(id_reg1_idx.uValue());
    };

    falu_reg2_forwarding_ctrl << [this] {
      return selectFPSource(id_reg2_idx.uValue());
    };
  }

  INPUTPORT(id_reg1_idx, c_RVRegsBits);
  INPUTPORT(id_reg2_idx, c_RVRegsBits);

  INPUTPORT(mem_reg_wr_idx, c_RVRegsBits);
  INPUTPORT(mem_reg_wr_en, 1);

  INPUTPORT(wb_reg_wr_idx, c_RVRegsBits);
  INPUTPORT(wb_reg_wr_en, 1);
  INPUTPORT(wb_mem_op, enumBitWidth<MemOp>());

  OUTPUTPORT_ENUM(alu_reg1_forwarding_ctrl, ForwardingSrc);
  OUTPUTPORT_ENUM(alu_reg2_forwarding_ctrl, ForwardingSrc);
  OUTPUTPORT_ENUM(falu_reg1_forwarding_ctrl, FPForwardingSrc);
  OUTPUTPORT_ENUM(falu_reg2_forwarding_ctrl, FPForwardingSrc);

protected:
  virtual FPForwardingSrc selectFPSource(unsigned index) const {
    const auto memOp = wb_mem_op.eValue<MemOp>();
    return index == wb_reg_wr_idx.uValue() &&
                   (memOp == MemOp::FLW || memOp == MemOp::FLD)
               ? FPForwardingSrc::WbStage
               : FPForwardingSrc::IdStage;
  }
};

// Only the standard two-slot predict-not-taken processor uses this policy.
// Other processors retain ForwardingUnit and its existing FP behavior.
class RV5SForwardingUnit : public ForwardingUnit {
public:
  RV5SForwardingUnit(const std::string &name, SimComponent *parent)
      : ForwardingUnit(name, parent) {}

  void setFPExtensionEnabled(bool enabled) { fpEnabled = enabled; }

  INPUTPORT(mem_fp_reg_wr_en, 1);
  INPUTPORT(wb_fp_reg_wr_en, 1);

private:
  FPForwardingSrc selectFPSource(unsigned index) const override {
    if (!fpEnabled)
      return ForwardingUnit::selectFPSource(index);
    const bool newerProducer = mem_fp_reg_wr_en.uValue() && index == mem_reg_wr_idx.uValue();
    const bool writebackProducer = wb_fp_reg_wr_en.uValue() && index == wb_reg_wr_idx.uValue();
    return writebackProducer && !newerProducer
               ? FPForwardingSrc::WbStage
               : FPForwardingSrc::IdStage;
  }

  bool fpEnabled = false;
};
} // namespace core
} // namespace vsrtl
