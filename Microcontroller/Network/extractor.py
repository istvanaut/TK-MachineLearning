from binascii import unhexlify

fileToOpen = r"model\simplenn_data.c"

def extractWeights():

    arrayText = "static const ai_u8 s_simplenn_weights[157328] = {"

    f = open(fileToOpen,'r')
    text = f.read()
    index = text.find(arrayText) + len(arrayText)
    code = " ".join(text[index:].split())
    code = code[:6 * 157328-2]
    code = code.replace(", ","").replace("0x","")
    print(code)

    weightsBin = unhexlify(code)
    print(weightsBin)
    f.close()
    return weightsBin

