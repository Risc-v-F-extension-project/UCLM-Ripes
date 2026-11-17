#include "rv_d_ext.h"

namespace Ripes {
namespace RVISA {
namespace ExtD {

void enableExt(
    const ISAInfoBase *isa,
    InstrVec & instructions,
    PseudoInstrVec &)
{
  if (isa->bits() != 64) {
    return;
  }

  enableInstructions<
      Fld_d, Fsd_d,
      Fadd_d, Fsub_d, Fmul_d, Fdiv_d, Fsqrt_d, Fmin_d, Fmax_d,
      Fsgnj_d, Fsgnjn_d, Fsgnjx_d, Fcvt_s_d, Fcvt_d_s
      >(instructions);
}

} //namespace ExtD
} // namespace RVISA
} // namespace Ripes
