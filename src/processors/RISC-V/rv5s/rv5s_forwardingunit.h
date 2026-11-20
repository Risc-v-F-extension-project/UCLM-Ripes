#pragma once

#include "processors/RISC-V/riscv.h"

#include "VSRTL/core/vsrtl_component.h"
#include <array>

namespace Ripes {
enum class ForwardingSrc { IdStage, MemStage, WbStage };
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
      const auto idx = id_reg1_idx.uValue();
      if (idx == mem_reg_wr_idx.uValue() && mem_fp_reg_wr_en.uValue()) {
        return ForwardingSrc::MemStage;
      } else if (idx == wb_reg_wr_idx.uValue() && wb_fp_reg_wr_en.uValue()) {
        return ForwardingSrc::WbStage;
      } else {
        return ForwardingSrc::IdStage;
      }
    };

    falu_reg2_forwarding_ctrl << [this] {
      const auto idx = id_reg2_idx.uValue();
      if (idx == mem_reg_wr_idx.uValue() && mem_fp_reg_wr_en.uValue()) {
        return ForwardingSrc::MemStage;
      } else if (idx == wb_reg_wr_idx.uValue() && wb_fp_reg_wr_en.uValue()) {
        return ForwardingSrc::WbStage;
      } else {
        return ForwardingSrc::IdStage;
      }
    };
  }

  std::array<ForwardingSrc, 2> operandSources(bool floatingPoint) const {
    if (floatingPoint) {
      return {falu_reg1_forwarding_ctrl.eValue<ForwardingSrc>(),
              falu_reg2_forwarding_ctrl.eValue<ForwardingSrc>()};
    }
    return {alu_reg1_forwarding_ctrl.eValue<ForwardingSrc>(),
            alu_reg2_forwarding_ctrl.eValue<ForwardingSrc>()};
  }

  INPUTPORT(id_reg1_idx, c_RVRegsBits);
  INPUTPORT(id_reg2_idx, c_RVRegsBits);

  INPUTPORT(mem_reg_wr_idx, c_RVRegsBits);
  INPUTPORT(mem_reg_wr_en, 1);

  INPUTPORT(wb_reg_wr_idx, c_RVRegsBits);
  INPUTPORT(wb_reg_wr_en, 1);

  INPUTPORT(mem_fp_reg_wr_en, 1);

  INPUTPORT(wb_fp_reg_wr_en, 1);

  OUTPUTPORT_ENUM(alu_reg1_forwarding_ctrl, ForwardingSrc);
  OUTPUTPORT_ENUM(alu_reg2_forwarding_ctrl, ForwardingSrc);
  OUTPUTPORT_ENUM(falu_reg1_forwarding_ctrl, ForwardingSrc);
  OUTPUTPORT_ENUM(falu_reg2_forwarding_ctrl, ForwardingSrc);
};
} // namespace core
} // namespace vsrtl
