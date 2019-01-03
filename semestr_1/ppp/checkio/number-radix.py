def num(c):
    return int(c) if ord(c) in range(48, 58) else ord(c) - 55

def checkio(strnum, radix):
    if any(num(v) > radix - 1 for v in strnum):
        return -1
    return sum(num(v) * pow(radix, i) for i, v in enumerate(reversed(strnum)))