from binascii import unhexlify

fileToOpen = r"model\mcu_nn_data.c"

def extractWeights():

    arrayText = "static const ai_u8 s_mcu_nn_weights[4848] = {"

    f = open(fileToOpen,'r')
    text = f.read()
    index = text.find(arrayText) + len(arrayText)
    code = " ".join(text[index:].split())
    code = code[:6 * 4848-2]
    code = code.replace(", ","").replace("0x","")
    print(code)

    weightsBin = unhexlify(code)
    print(weightsBin)
    f.close()
    return weightsBin

