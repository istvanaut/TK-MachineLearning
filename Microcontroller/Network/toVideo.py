import numpy as np
from PIL import Image
import pandas as pd
import cv2
import json
testOutput = pd.read_csv('Data/Outputs/output.csv')

import subprocess
from PIL import Image
for i in range(testOutput.shape[0]):
    l=np.array(json.loads(testOutput.loc[i]['state_image'])[0])
    im=np.array(l*255, dtype=np.uint8)
    img = Image.fromarray(im)
    img.save("Data/Images/%07d.jpg" % i)
subprocess.call(["ffmpeg","-y","-r",str(4),"-i", "Data/Images/%07d.jpg","-vcodec","mpeg4", "-qscale","5", "-r", str(4), "Data/Videos/video_prev.avi"])