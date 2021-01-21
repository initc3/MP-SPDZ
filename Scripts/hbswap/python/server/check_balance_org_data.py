import sys

from utils import from_hex, sz

if __name__=='__main__':
    server_id = sys.argv[1]

    file = f"Player-Data/Private-Output-{server_id}"
    agree = 0
    with open(file, 'rb') as f:
        agree = f.read(sz)
    print(from_hex(agree))