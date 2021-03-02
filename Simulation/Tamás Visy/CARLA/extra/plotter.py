import matplotlib.pyplot as plt


def plot(data):
    fig = plt.figure()
    ax = fig.add_subplot(111, projection='3d')

    for d in data:
        ax.scatter(d[0], d[1], d[2])

    ax.set_xlabel('X Label')
    ax.set_ylabel('Y Label')
    ax.set_zlabel('Z Label')

    plt.show()
