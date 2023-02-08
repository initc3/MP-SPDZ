import sys

BUFFER_SIZE = 100
N = 4
k = 253
bit_size = 32
batch_size = bit_size * BUFFER_SIZE

sig = b'/\x00\x00\x00\x00\x00\x00\x00Shamir gfp\x00 \x00\x00\x00\x10\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x14\xde\xf9\xde\xa2\xf7\x9c\xd6X\x12c\x1a\\\xf5\xd3\xed'

if __name__ == '__main__':
    element_type = sys.argv[1]

    for server_id in range(N):
        file = f'offline/buffer/{N}-MSp-{k}/{element_type}-MSp-P{server_id}'
        with open(file, "rb") as f:
            data = f.read()
            # print(len(data))
            # print(data[batch_size : 2 * batch_size])
        with open(file, "wb") as f:
            f.write(sig + data)
