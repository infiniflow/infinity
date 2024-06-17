from urllib.request import urlopen
import tarfile
import subprocess

funcs: "set[str]" = set()

files = [
    ('https://downloads.python.org/pypy/pypy3.9-v7.3.11-macos_arm64.tar.bz2', 'pypy3.9-v7.3.11-macos_arm64/bin/libpypy3.9-c.dylib')
]

for f in files:
    fs = urlopen(f[0])
    ft = tarfile.open(fileobj=fs, mode="r|bz2")
    success = False
    for member in ft: # move to the next file each loop
        if member.name == f[1]:
            ft.extract(member, path='tmp')
            success = True
    assert success

    out = subprocess.check_output(['nm', '-gjU', 'tmp/' + f[1]])
    for line in out.decode().split('\n'):
        if line.startswith('_Py') or line.startswith('__Py'):
            funcs.add(line)

with open("darwin-ld-pypy.sym", "w") as f:
    for func in sorted(list(funcs)):
        f.write(f'-U _{func}\n')
