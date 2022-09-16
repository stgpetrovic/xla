from absl.testing import absltest, parameterized
from absl import logging
import copy
import torch
import torch.distributed as dist
import torch.nn as nn
import torch.optim as optim
from torch.nn.parallel import DistributedDataParallel as DDP
import torch_xla.core.xla_model as xm
import torch_xla.distributed.xla_backend
from torch_xla.experimental import pjrt
import distributed_util as util


class TestPjRtDistributedDataParallel(parameterized.TestCase):

  @staticmethod
  def _ddp_init(init_file: str):
    util.init_xla_backend(init_file)

    device = xm.xla_device()
    model = nn.Linear(10, 10).to(device)
    ddp_model = DDP(model)

  def test_ddp_init(self):
    pjrt.run_multiprocess(self._ddp_init, self.create_tempfile().full_path)

  def test_ddp_correctness(self):
    pjrt.run_multiprocess(util.ddp_correctness,
                          self.create_tempfile().full_path)


if __name__ == "__main__":
  absltest.main()
