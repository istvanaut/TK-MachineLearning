import matplotlib.pyplot as plt

# TODO (5) add more things it can do
# Important: set working directory to CARLA instead of CARLA/support

# ~~~ for distance from start
LINES_CONTAINING = '~~~'

with open('files/carla.log', 'r') as f:
    content = f.readlines()
content = [x.strip() for x in content]

important = []

for line in content:
    if LINES_CONTAINING in line:
        i = line.index('-:-')
        important.append(float(str(line[i+4:i+12])))

print(important)

plt.plot(important)
plt.show()
