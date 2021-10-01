import torch
import subprocess

from binascii import unhexlify

from Microcontroller.Network.Networks.FlatDense import FlatDense
from Microcontroller.Network.ReinforcementModel import ReinforcementModel


class NetworkExporter:
    def __init__(self):
        self.filename = r"ExportedModel/MCU_NN.onnx"
        self.output_directory = "ExportedModel/model"
        self.exe_path = r"D:\\University\\AVG-Önlab\\stm32ai-windows-6.0.0\\windows\\stm32ai.exe"

    def export_to_ONNX(self, model):
        torch.onnx.export(model.target_net,
                          model.target_net.proxy_input(),
                          self.filename,
                          export_params=True,
                          opset_version=10,
                          do_constant_folding=True,
                          verbose=True,
                          input_names=['image'],
                          output_names=['action'])

        """Converting model using xcube.ai"""
        # This step requires xcube.ai to be installed
        args = self.exe_path + " generate " + "-m " + self.filename + " -o " + self.output_directory + " --type onnx" + " --compression 8" + " --name simplenn" + " --series stm32f7"
        subprocess.call(args, shell=True)

    def extract_weights(self):
        array_text = "static const ai_u8 s_simplenn_weights[157328] = {"

        with open(self.output_directory+'\simplenn_data.c', 'r') as weights:
            text = weights.read()
        index = text.find(array_text) + len(array_text)
        code = " ".join(text[index:].split())
        code = code[:6 * 157328 - 2]
        code = code.replace(", ", "").replace("0x", "")
        weights_bin = unhexlify(code)
        return weights_bin


def main():
    exporter = NetworkExporter()
    model = ReinforcementModel(dim_features=12, height=32, width=32, n_actions=4, model=FlatDense)
    exporter.export_to_ONNX(model)


if __name__ == '__main__':
    main()
