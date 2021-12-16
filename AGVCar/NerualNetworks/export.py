import numpy as np
import torch.onnx
import subprocess



def export_to_ONNX(model):
    # Create model with feature dimension 10

    D = r"MCU_NN.onnx"
    x =torch.randn(1, 32*32)
    print(x)
    print(x.shape)
    print("Output:")
    print(model.target_net(x,torch.tensor([0])))
    input_names = [ "image" ]
    output_names = [ "action" ]

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

