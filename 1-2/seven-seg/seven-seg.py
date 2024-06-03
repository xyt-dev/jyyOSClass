import fileinput

TEMPLATE = '''
\033[2J\033[1;1H
      AAAAAAAA
    FF        BB
    FF        BB
    FF        BB
    FF        BB
      GGGGGGGG
    EE        CC
    EE        CC
    EE        CC
    EE        CC
      DDDDDDDD

'''

BLOCK = {
    0: '\033[37m\u2593\033[0m',
    1: '\033[31m\u2588\033[0m',
}

VARS = 'ABCDEFG'

for v in VARS:
    globals()[v] = 0

stdin = fileinput.input()

while True:
    pic = TEMPLATE
    for v in VARS:
        pic = pic.replace(v, BLOCK[globals()[v]])
    print(pic)
    exec(stdin.readline())