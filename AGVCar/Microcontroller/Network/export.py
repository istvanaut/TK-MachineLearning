import numpy as np
import torch.onnx
import subprocess
from Networks.FlatDense import FlatDense
from ReinforcementModel import ReinforcementModel
from torchsummary import summary

import ReinforcementModel


def export_to_ONNX(model):
    # Create model with feature dimension 10
    #model = ReinforcementModel.ReinforcementModel(dim_features=0, height=32, width=32, n_actions=4, model=FlatDense)
    #model.load_model(path = r"tensor.pt",dim_features=0, image_height=32, image_width=32, n_actions=4,model=FlatDense)

    D = r"MCU_NN.onnx"
    x =torch.randn(1, 32*32)
    print(x)
    print(x.shape)
    print("Output:")
    #summary(model.policy_net,(1,32*32))
    print(model.target_net(x,torch.tensor([0])))
    #torch.randn(1, 3, 64, 64, requires_grad=True)
    input_names = [ "image" ]
    output_names = [ "action" ]
    #print(x.size())
    torch.onnx.export(model.target_net,
                      (x, torch.tensor([0])),
                      D,
                      export_params=True,
                      opset_version=10,
                      do_constant_folding=True,
                      verbose=True,
                      input_names=input_names,
                      output_names=output_names)

    """Converting model using xcube.ai"""
    # This step requires xcube.ai to be installed
    xcubeai_outdir = "model"
    xcubeai_exepath = r"D:\\University\\AVG-Önlab\\stm32ai-windows-6.0.0\\windows\\stm32ai.exe"

    args = xcubeai_exepath + " generate " + "-m " + D + " -o " + xcubeai_outdir + " --type onnx" + " --compression 8" + " --name simplenn" + " --series stm32f7"
    subprocess.call(args, shell=True)

