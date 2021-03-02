spec = None


def set_spec(s):
    global spec
    spec = s


def report():
    if spec is not None:
        print(spec.get_location())
