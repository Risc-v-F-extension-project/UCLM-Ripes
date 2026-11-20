#pragma once

#include "processors/interface/ripesprocessor.h"
#include "rv5s_forwardingunit.h"
#include "rv5s_hazardunit.h"

namespace vsrtl {
namespace core {

// Both forwarding policies use IdStage, MemStage and WbStage. Keep the
// observation separate from the logic that controls the multiplexers.
template <typename Source>
void appendOperandForwarding(std::vector<Ripes::OperandForwardingInfo> &infos,
                             const Ripes::RipesProcessor &processor,
                             Source source, Ripes::StageIndex consumerStage,
                             unsigned operand, bool floatingPoint) {
  if (source == Source::IdStage)
    return;

  const Ripes::StageIndex producerStage{
      0, source == Source::MemStage ? 3u : 4u};
  const auto producer = processor.stageInfo(producerStage);
  const auto consumer = processor.stageInfo(consumerStage);
  if (!producer.stage_valid || !consumer.stage_valid ||
      producer.state != Ripes::StageInfo::State::None ||
      consumer.state != Ripes::StageInfo::State::None)
    return;

  infos.push_back({producer.pc, consumer.pc, producerStage, consumerStage,
                   operand, floatingPoint});
}

inline std::vector<Ripes::OperandForwardingInfo> executionForwardingInfos(
    const Ripes::RipesProcessor &processor, const ForwardingUnit &forwarding,
    Ripes::RVInstr opcode, bool executionEnabled, bool controlFlowInID) {
  std::vector<Ripes::OperandForwardingInfo> infos;
  const auto execution = processor.stageInfo({0, 2});
  if (!executionEnabled || !execution.stage_valid ||
      execution.state != Ripes::StageInfo::State::None)
    return infos;

  // In a one-slot model, branches consume their operands in ID.
  if (controlFlowInID &&
      (Control::do_branch_ctrl(opcode) || Control::do_jump_ctrl(opcode)))
    return infos;

  const auto stages = processor.fpUnicicleStageInfos();
  const auto cycle = static_cast<uint64_t>(processor.getCycleCount());
  if (!stages.empty() &&
      std::none_of(stages.begin(), stages.end(), [&](const auto &stage) {
        return stage.valid && stage.pc == execution.pc &&
               stage.inputCycle == cycle;
      }))
    return infos;

  const auto usage = HazardUnit::sourceUsageFor(opcode);
  const auto integerSources = forwarding.operandSources(false);
  const auto fpSources = forwarding.operandSources(true);
  if (usage.usesRs1) {
    appendOperandForwarding(infos, processor,
                            usage.rs1IsFP ? fpSources[0] : integerSources[0],
                            {0, 2}, 1, usage.rs1IsFP);
  }
  if (usage.usesRs2) {
    appendOperandForwarding(infos, processor,
                            usage.rs2IsFP ? fpSources[1] : integerSources[1],
                            {0, 2}, 2, usage.rs2IsFP);
  }
  return infos;
}

} // namespace core
} // namespace vsrtl
