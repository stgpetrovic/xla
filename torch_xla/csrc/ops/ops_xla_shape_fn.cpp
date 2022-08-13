#include "torch_xla/csrc/ops/ops_xla_shape_fn.h"

#include "tensorflow/compiler/xla/client/lib/logdet.h"
#include "tensorflow/compiler/xla/shape_util.h"
#include "tensorflow/compiler/xla/xla_client/util.h"
#include "torch_xla/csrc/elementwise.h"
#include "torch_xla/csrc/helpers.h"
#include "torch_xla/csrc/pooling.h"
#include "torch_xla/csrc/reduction.h"

namespace torch_xla {
namespace {
template <typename T>
std::vector<T> GetValuesVectorWithOptional(
    absl::Span<const T> values,
    absl::Span<const c10::optional<T>* const> opt_values) {
  std::vector<T> result(values.begin(), values.end());
  for (auto opt : opt_values) {
    if (*opt) {
      result.push_back(*(*opt));
    }
  }
  return result;
}
}  // namespace

xla::Shape AbsOutputShape(const torch::lazy::Value& input) {
  return GetXlaShape(input);
}

xla::Shape AcosOutputShape(const torch::lazy::Value& input) {
  return GetXlaShape(input);
}

xla::Shape AcoshOutputShape(const torch::lazy::Value& input) {
  return GetXlaShape(input);
}

xla::Shape AdaptiveAvgPool2dOutputShape(const torch::lazy::Value& input,
                                        absl::Span<const int64_t> output_size) {
  auto lower_for_shape_fn =
      [output_size](absl::Span<const xla::XlaOp> operands) -> xla::XlaOp {
    XLA_CHECK_EQ(operands.size(), 1);
    return BuildAdaptiveAvgPool2d(operands[0], output_size);
  };
  return InferOutputShape({GetXlaShape(input)}, lower_for_shape_fn);
}

xla::Shape AdaptiveAvgPool2dBackwardOutputShape(
    const torch::lazy::Value& grad_output, const torch::lazy::Value& input) {
  auto lower_for_shape_fn =
      [](absl::Span<const xla::XlaOp> operands) -> xla::XlaOp {
    XLA_CHECK_EQ(operands.size(), 2);
    return BuildAdaptiveAvgPool2dBackward(/*out_backprop=*/operands[0],
                                          /*input=*/operands[1]);
  };
  return InferOutputShape({GetXlaShape(grad_output), GetXlaShape(input)},
                          lower_for_shape_fn);
}

xla::Shape AdaptiveAvgPool3dOutputShape(const torch::lazy::Value& input,
                                        absl::Span<const int64_t> output_size) {
  auto lower_for_shape_fn =
      [output_size](absl::Span<const xla::XlaOp> operands) -> xla::XlaOp {
    XLA_CHECK_EQ(operands.size(), 1);
    return BuildAdaptiveAvgPool3d(operands[0], output_size);
  };
  return InferOutputShape({GetXlaShape(input)}, lower_for_shape_fn);
}

xla::Shape AmaxOutputShape(const torch::lazy::Value& input,
                           absl::Span<const int64_t> dim, bool keepdim) {
  auto lower_for_shape_fn =
      [&](absl::Span<const xla::XlaOp> operands) -> xla::XlaOp {
    return BuildMaxInDims(operands[0], dim, keepdim);
  };
  return InferOutputShape({GetXlaShape(input)}, lower_for_shape_fn);
}

xla::Shape AminOutputShape(const torch::lazy::Value& input,
                           absl::Span<const int64_t> dim, bool keepdim) {
  auto lower_for_shape_fn =
      [&](absl::Span<const xla::XlaOp> operands) -> xla::XlaOp {
    return BuildMinInDims(operands[0], dim, keepdim);
  };
  return InferOutputShape({GetXlaShape(input)}, lower_for_shape_fn);
}

xla::Shape AdaptiveAvgPool3dBackwardOutputShape(
    const torch::lazy::Value& grad_output, const torch::lazy::Value& input) {
  auto lower_for_shape_fn =
      [](absl::Span<const xla::XlaOp> operands) -> xla::XlaOp {
    XLA_CHECK_EQ(operands.size(), 2);
    return BuildAdaptiveAvgPool3dBackward(/*out_backprop=*/operands[0],
                                          /*input=*/operands[1]);
  };
  return InferOutputShape({GetXlaShape(grad_output), GetXlaShape(input)},
                          lower_for_shape_fn);
}

xla::Shape AllOutputShape(const torch::lazy::Value& input) {
  std::vector<int64_t> dimensions =
      torch::lazy::Iota<int64_t>(GetXlaShape(input).rank());
  auto lower_for_shape_fn =
      [dimensions](absl::Span<const xla::XlaOp> operands) -> xla::XlaOp {
    return BuildAll(operands[0], dimensions, false);
  };

  return InferOutputShape({GetXlaShape(input)}, lower_for_shape_fn);
}

xla::Shape AsinOutputShape(const torch::lazy::Value& input) {
  return GetXlaShape(input);
}

xla::Shape AsinhOutputShape(const torch::lazy::Value& input) {
  return GetXlaShape(input);
}

xla::Shape AtanOutputShape(const torch::lazy::Value& input) {
  return GetXlaShape(input);
}

xla::Shape AtanhOutputShape(const torch::lazy::Value& input) {
  return GetXlaShape(input);
}

xla::Shape BinaryCrossEntropyOutputShape(
    const torch::lazy::Value& input, const torch::lazy::Value& target,
    const c10::optional<torch::lazy::Value>& weight, int64_t reduction) {
  auto lower_for_shape_fn =
      [&](absl::Span<const xla::XlaOp> operands) -> xla::XlaOp {
    absl::optional<xla::XlaOp> weight;
    if (operands.size() > 2) {
      weight = operands[2];
    }
    return BuildBinaryCrossEntropy(operands[0], operands[1], weight,
                                   GetXlaReductionMode(reduction));
  };
  std::vector<xla::Shape> shapes;
  for (auto& i : GetValuesVectorWithOptional<torch::lazy::Value>(
           {input, target}, {&weight})) {
    shapes.push_back(GetXlaShape(i));
  }
  return InferOutputShape(shapes, lower_for_shape_fn);
}

xla::Shape BinaryCrossEntropyBackwardOutputShape(
    const torch::lazy::Value& grad_output, const torch::lazy::Value& input,
    const torch::lazy::Value& target,
    const c10::optional<torch::lazy::Value>& weight, int64_t reduction) {
  auto lower_for_shape_fn =
      [&](absl::Span<const xla::XlaOp> operands) -> xla::XlaOp {
    absl::optional<xla::XlaOp> weight;
    if (operands.size() > 3) {
      weight = operands[3];
    }
    return BuildBinaryCrossEntropyBackward(operands[0], operands[1],
                                           operands[2], weight,
                                           GetXlaReductionMode(reduction));
  };
  std::vector<xla::Shape> shapes;
  for (auto& i : GetValuesVectorWithOptional<torch::lazy::Value>(
           {grad_output, input, target}, {&weight})) {
    shapes.push_back(GetXlaShape(i));
  }
  return InferOutputShape(shapes, lower_for_shape_fn);
}

xla::Shape CeilOutputShape(const torch::lazy::Value& input) {
  return GetXlaShape(input);
}

xla::Shape ClampMaxTensorOutputShape(const torch::lazy::Value& input,
                                     const torch::lazy::Value& other) {
  auto lower_for_shape_fn =
      [](absl::Span<const xla::XlaOp> operands) -> xla::XlaOp {
    return xla::Min(operands[0], operands[1]);
  };
  return InferOutputShape({GetXlaShape(input), GetXlaShape(other)},
                          lower_for_shape_fn);
}

xla::Shape ClampMinTensorOutputShape(const torch::lazy::Value& input,
                                     const torch::lazy::Value& other) {
  auto lower_for_shape_fn =
      [](absl::Span<const xla::XlaOp> operands) -> xla::XlaOp {
    return xla::Max(operands[0], operands[1]);
  };
  return InferOutputShape({GetXlaShape(input), GetXlaShape(other)},
                          lower_for_shape_fn);
}

xla::Shape CosOutputShape(const torch::lazy::Value& input) {
  return GetXlaShape(input);
}

xla::Shape CoshOutputShape(const torch::lazy::Value& input) {
  return GetXlaShape(input);
}

xla::Shape ErfOutputShape(const torch::lazy::Value& input) {
  return GetXlaShape(input);
}

xla::Shape ErfcOutputShape(const torch::lazy::Value& input) {
  return GetXlaShape(input);
}

xla::Shape ErfinvOutputShape(const torch::lazy::Value& input) {
  return GetXlaShape(input);
}

xla::Shape ExpOutputShape(const torch::lazy::Value& input) {
  return GetXlaShape(input);
}

xla::Shape Expm1OutputShape(const torch::lazy::Value& input) {
  return GetXlaShape(input);
}

xla::Shape FloorOutputShape(const torch::lazy::Value& input) {
  return GetXlaShape(input);
}

xla::Shape GeScalarOutputShape(const torch::lazy::Value& self,
                               const torch::lazy::Value& other) {
  auto lower_for_shape_fn =
      [&](absl::Span<const xla::XlaOp> operands) -> xla::XlaOp {
    return BuildComparisonOp(at::aten::ge, operands[0], operands[1]);
  };
  return InferOutputShape({GetXlaShape(self), GetXlaShape(other)},
                          lower_for_shape_fn);
}

xla::Shape GeTensorOutputShape(const torch::lazy::Value& self,
                               const torch::lazy::Value& other) {
  return GtScalarOutputShape(self, other);
}

xla::Shape GtScalarOutputShape(const torch::lazy::Value& self,
                               const torch::lazy::Value& other) {
  auto lower_for_shape_fn =
      [&](absl::Span<const xla::XlaOp> operands) -> xla::XlaOp {
    return BuildComparisonOp(at::aten::gt, operands[0], operands[1]);
  };
  return InferOutputShape({GetXlaShape(self), GetXlaShape(other)},
                          lower_for_shape_fn);
}

xla::Shape GtTensorOutputShape(const torch::lazy::Value& self,
                               const torch::lazy::Value& other) {
  return GtScalarOutputShape(self, other);
}

xla::Shape HardsigmoidOutputShape(const torch::lazy::Value& input) {
  return GetXlaShape(input);
}

xla::Shape HardsigmoidBackwardOutputShape(const torch::lazy::Value& grad_output,
                                          const torch::lazy::Value& input) {
  return GetXlaShape(input);
}

xla::Shape HardswishOutputShape(const torch::lazy::Value& input) {
  return GetXlaShape(input);
}

xla::Shape HardswishBackwardOutputShape(const torch::lazy::Value& grad_output,
                                        const torch::lazy::Value& input) {
  return GetXlaShape(input);
}

xla::Shape InverseOutputShape(const torch::lazy::Value& input) {
  return GetXlaShape(input);
}

xla::Shape IsnanOutputShape(const torch::lazy::Value& input) {
  xla::Shape isnan_shape(GetXlaShape(input));
  isnan_shape.set_element_type(xla::PRED);
  return isnan_shape;
}

xla::Shape LeScalarOutputShape(const torch::lazy::Value& self,
                               const torch::lazy::Value& other) {
  auto lower_for_shape_fn =
      [&](absl::Span<const xla::XlaOp> operands) -> xla::XlaOp {
    return BuildComparisonOp(at::aten::le, operands[0], operands[1]);
  };
  return InferOutputShape({GetXlaShape(self), GetXlaShape(other)},
                          lower_for_shape_fn);
}

xla::Shape LeTensorOutputShape(const torch::lazy::Value& self,
                               const torch::lazy::Value& other) {
  return LeScalarOutputShape(self, other);
}

xla::Shape LtScalarOutputShape(const torch::lazy::Value& self,
                               const torch::lazy::Value& other) {
  auto lower_for_shape_fn =
      [&](absl::Span<const xla::XlaOp> operands) -> xla::XlaOp {
    return BuildComparisonOp(at::aten::lt, operands[0], operands[1]);
  };
  return InferOutputShape({GetXlaShape(self), GetXlaShape(other)},
                          lower_for_shape_fn);
}

xla::Shape LtTensorOutputShape(const torch::lazy::Value& self,
                               const torch::lazy::Value& other) {
  return LtScalarOutputShape(self, other);
}

xla::Shape LogdetOutputShape(const torch::lazy::Value& input) {
  const xla::Shape& input_shape = GetXlaShape(input);
  XLA_CHECK_GE(input_shape.rank(), 2) << input_shape;
  // The input tensor is ...,N,N
  xla::Shape logdet_shape(input_shape);
  logdet_shape.DeleteDimension(input_shape.rank() - 1);
  logdet_shape.DeleteDimension(input_shape.rank() - 2);
  return logdet_shape;
}

xla::Shape LogicalAndOutputShape(const torch::lazy::Value& input,
                                 const torch::lazy::Value& other) {
  auto shape_fn = [](absl::Span<const xla::XlaOp> operands) -> xla::XlaOp {
    return XlaHelpers::PromotedLogicalBinaryOp(
        operands[0], operands[1],
        [](xla::XlaOp lhs, xla::XlaOp rhs) { return xla::And(lhs, rhs); });
  };
  return InferOutputShape({GetXlaShape(input), GetXlaShape(other)}, shape_fn);
}

xla::Shape LogicalNotOutputShape(const torch::lazy::Value& input) {
  auto shape_fn = [](absl::Span<const xla::XlaOp> operands) -> xla::XlaOp {
    return XlaHelpers::PromotedLogicalUnaryOp(
        operands[0], [](xla::XlaOp lhs) { return xla::Not(lhs); });
  };
  return InferOutputShape({GetXlaShape(input)}, shape_fn);
}

xla::Shape LogicalOrOutputShape(const torch::lazy::Value& input,
                                const torch::lazy::Value& other) {
  auto shape_fn = [](absl::Span<const xla::XlaOp> operands) -> xla::XlaOp {
    return XlaHelpers::PromotedLogicalBinaryOp(
        operands[0], operands[1],
        [](xla::XlaOp lhs, xla::XlaOp rhs) { return xla::Or(lhs, rhs); });
  };
  return InferOutputShape({GetXlaShape(input), GetXlaShape(other)}, shape_fn);
}

xla::Shape LogicalXorOutputShape(const torch::lazy::Value& input,
                                 const torch::lazy::Value& other) {
  auto shape_fn = [](absl::Span<const xla::XlaOp> operands) -> xla::XlaOp {
    return XlaHelpers::PromotedLogicalBinaryOp(
        operands[0], operands[1],
        [](xla::XlaOp lhs, xla::XlaOp rhs) { return xla::Xor(lhs, rhs); });
  };
  return InferOutputShape({GetXlaShape(input), GetXlaShape(other)}, shape_fn);
}

xla::Shape LogSigmoidForwardOutputShape(const torch::lazy::Value& input) {
  return xla::ShapeUtil::MakeTupleShape(
      {GetXlaShape(input), GetXlaShape(input)});
}

xla::Shape LogSigmoidBackwardOutputShape(const torch::lazy::Value& grad_output,
                                         const torch::lazy::Value& input,
                                         const torch::lazy::Value& buffer) {
  return GetXlaShape(grad_output);
}

xla::Shape MaximumOutputShape(const torch::lazy::Value& input,
                              const torch::lazy::Value& other) {
  auto lower_for_shape_fn =
      [&](absl::Span<const xla::XlaOp> operands) -> xla::XlaOp {
    auto promoted = XlaHelpers::Promote(operands[0], operands[1]);
    return xla::Max(promoted.first, promoted.second);
  };
  return InferOutputShape({GetXlaShape(input), GetXlaShape(other)},
                          lower_for_shape_fn);
}

xla::Shape MinimumOutputShape(const torch::lazy::Value& input,
                              const torch::lazy::Value& other) {
  auto lower_for_shape_fn =
      [&](absl::Span<const xla::XlaOp> operands) -> xla::XlaOp {
    auto promoted = XlaHelpers::Promote(operands[0], operands[1]);
    return xla::Max(promoted.first, promoted.second);
  };
  return InferOutputShape({GetXlaShape(input), GetXlaShape(other)},
                          lower_for_shape_fn);
}

xla::Shape ReciprocalOutputShape(const torch::lazy::Value& input) {
  return GetXlaShape(input);
}

xla::Shape ReluOutputShape(const torch::lazy::Value& input) {
  auto lower_for_shape_fn =
      [](absl::Span<const xla::XlaOp> operands) -> xla::XlaOp {
    XLA_CHECK_EQ(operands.size(), 1) << "Unexpected number of operands";
    return BuildRelu(operands[0]);
  };
  return InferOutputShape({GetXlaShape(input)}, lower_for_shape_fn);
}

xla::Shape RoundOutputShape(const torch::lazy::Value& input) {
  return GetXlaShape(input);
}

xla::Shape RsqrtOutputShape(const torch::lazy::Value& input) {
  return GetXlaShape(input);
}

xla::Shape SeluOutputShape(const torch::lazy::Value& input) {
  return GetXlaShape(input);
}

xla::Shape SgnOutputShape(const torch::lazy::Value& input) {
  return GetXlaShape(input);
}

xla::Shape SignOutputShape(const torch::lazy::Value& input) {
  return GetXlaShape(input);
}

xla::Shape SiluOutputShape(const torch::lazy::Value& input) {
  return GetXlaShape(input);
}

xla::Shape SiluBackwardOutputShape(const torch::lazy::Value& grad_output,
                                   const torch::lazy::Value& input) {
  auto lower_for_shape_fn =
      [](absl::Span<const xla::XlaOp> operands) -> xla::XlaOp {
    return BuildSiLUBackward(operands[0], operands[1]);
  };
  return InferOutputShape({GetXlaShape(grad_output), GetXlaShape(input)},
                          lower_for_shape_fn);
}

xla::Shape SinOutputShape(const torch::lazy::Value& input) {
  return GetXlaShape(input);
}

xla::Shape SinhOutputShape(const torch::lazy::Value& input) {
  return GetXlaShape(input);
}

/* Blocked on https://github.com/pytorch/xla/issues/3596 */
// xla::Shape SlogdetOutputShape(const torch::lazy::Value& input) {
//   auto lower_for_shape_fn =
//       [](absl::Span<const xla::XlaOp> operands) -> xla::XlaOp {
//     xla::SignAndLogDet result = xla::SLogDet(operands[0]);
//     return xla::Tuple(operands[0].builder(), {result.sign, result.logdet});
//   };
//   return InferOutputShape({GetXlaShape(input)}, lower_for_shape_fn);
// }

xla::Shape TanOutputShape(const torch::lazy::Value& input) {
  return GetXlaShape(input);
}

xla::Shape TanhOutputShape(const torch::lazy::Value& input) {
  return GetXlaShape(input);
}

xla::Shape TrilOutputShape(const torch::lazy::Value& input) {
  return GetXlaShape(input);
}

xla::Shape TriuOutputShape(const torch::lazy::Value& input) {
  return GetXlaShape(input);
}

xla::Shape TruncOutputShape(const torch::lazy::Value& input) {
  return GetXlaShape(input);
}

}  // namespace torch_xla
