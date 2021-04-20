import matplotlib.pyplot as plt

# Important setup: set working directory to CARLA instead of CARLA/support

# ~~~ are the lines summarizing a run
from support.names import nameof

LINES_CONTAINING = '~~~'

with open('files/carla.log', 'r') as f:
    content = f.readlines()
content = [x.strip() for x in content]

distances = []
successes = []

for line in content:
    if LINES_CONTAINING in line:
        i = line.index('-:-')
        distances.append(float(str(line[i + 4:i + 12])))
        if 'success: True' in line:
            successes.append(10.0)
        else:
            successes.append(0.0)

print(distances)
print(successes)  # success or fail

plt.plot(distances, label=nameof(distances))
plt.plot(successes, label=nameof(distances))
plt.show()
