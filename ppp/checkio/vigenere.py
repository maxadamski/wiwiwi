
def generate_key(base, length):
    return (base * (length // len(base) + 1))[:length]

def vigenere(m, k, op):
    return ''.join(chr(op(ord(m[i]), ord(k[i]) - 2*ord('A')) % 26 + ord('A')) for i, _ in enumerate(m))

def encrypted(m, k):
    return vigenere(m, k, lambda a, b: a + b)

def decrypted(m, k):
    return vigenere(m, k, lambda a, b: a - b)

def used_key(d, e):
    return [chr(k) for k in range(ord('A'), ord('Z')) if d == decrypted(e, chr(k))][0]

def decode_vigenere(d, e, new_encr):
    key = ''.join(used_key(d[i], e[i]) for i in range(len(d)))
    print(key)
    return decrypted(new_encr, key)


#These "asserts" using only for self-checking and not necessary for auto-testing
if __name__ == '__main__':

    assert decode_vigenere('DONTWORRYBEHAPPY',
                           'FVRVGWFTFFGRIDRF',
                           'DLLCZXMFVRVGWFTF') == "BEHAPPYDONTWORRY", "CHECKIO"
    assert decode_vigenere('HELLO', 'OIWWC', 'ICP') == "BYE", "HELLO"
    assert decode_vigenere('LOREMIPSUM',
                           'OCCSDQJEXA',
                           'OCCSDQJEXA') == "LOREMIPSUM", "DOLORIUM"

