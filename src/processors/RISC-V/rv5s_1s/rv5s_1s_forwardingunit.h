#pragma once

#include "processors/RISC-V/riscv.h"
#include "../rv5s/rv5s_forwardingunit.h"

#include "VSRTL/core/vsrtl_component.h"

namespace Ripes {
enum class ForwardingSrc_1S { IdStage, MemStage, WbStage };
}

namespace vsrtl {
namespace core {
using namespace Ripes;

class ForwardingUnit_1S : public RV5SForwardingUnit {
public:
  ForwardingUnit_1S(const std::string &name, SimComponent *parent)
      : RV5SForwardingUnit(name, parent) {
    branch_op1_fwctrl << [this] { return getFwSrc(if_reg1_idx.uValue()); };
    branch_op2_fwctrl << [this] { return getFwSrc(if_reg2_idx.uValue()); };
  }

  // used to determine what registers are needed for branches
  INPUTPORT(if_reg1_idx, c_RVRegsBits);
  INPUTPORT(if_reg2_idx, c_RVRegsBits);

  // target register of ALU result
  INPUTPORT(ex_reg_wr_idx, c_RVRegsBits);
  INPUTPORT(ex_reg_wr_en, 1);

  // extra control lines for branch operand forwarding
  OUTPUTPORT_ENUM(branch_op1_fwctrl, ForwardingSrc_1S);
  OUTPUTPORT_ENUM(branch_op2_fwctrl, ForwardingSrc_1S);

private:
  ForwardingSrc_1S getFwSrc(VSRTL_VT_U idx) const {
    if (idx == 0) {
      return ForwardingSrc_1S::IdStage;
    } else if (idx == mem_reg_wr_idx.uValue() && mem_reg_wr_en.uValue()) {
      return ForwardingSrc_1S::MemStage;
    } else if (idx == wb_reg_wr_idx.uValue() && wb_reg_wr_en.uValue()) {
      return ForwardingSrc_1S::WbStage;
    } else {
      return ForwardingSrc_1S::IdStage;
    }
  }
};
} // namespace core
} // namespace vsrtl
