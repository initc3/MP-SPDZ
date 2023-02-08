import sys


def int_to_hex(x):
    x = mpz(x)
    x = (x * R) % prime
    x = binary(int(x))
    x += b'\x00' * (32 - len(x))
    return x

if __name__ == '__main__':
    file = sys.argv[1]
    with open(file, "rb") as f:
        i = 0
        for line in f.readlines():
            print(line, i)
            i += 1
