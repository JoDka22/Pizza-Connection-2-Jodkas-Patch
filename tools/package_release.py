"""Package only explicitly allowed patch files; never include original game files."""
import hashlib
import json
import re
import struct
import subprocess
import sys
import zipfile
from pathlib import Path

ROOT = Path(__file__).resolve().parent.parent


def digest(data):
    return hashlib.sha256(data).hexdigest()


def patch_dll(name):
    data = (ROOT / 'out' / 'Release' / name).read_bytes()
    if len(data) < 64 or data[:2] != b'MZ':
        raise ValueError('Missing PE image: ' + name)
    pe = struct.unpack_from('<I', data, 60)[0]
    if pe > len(data) - 24 or data[pe:pe + 4] != b'PE\0\0':
        raise ValueError('Invalid PE image: ' + name)
    if struct.unpack_from('<H', data, pe + 4)[0] != 0x14c:
        raise ValueError('Release DLL must be x86: ' + name)
    if not struct.unpack_from('<H', data, pe + 22)[0] & 0x2000:
        raise ValueError('Release image must be a DLL: ' + name)
    return data


def package(version):
    if not re.fullmatch(r'v[0-9]+\.[0-9]+(?:\.[0-9]+)?', version):
        raise ValueError('Expected a version such as v1.0')
    state = subprocess.check_output(['git', 'status', '--porcelain'], cwd=ROOT, text=True)
    if state.strip():
        raise ValueError('Commit tracked/untracked source changes before packaging')
    commit = subprocess.check_output(['git', 'rev-parse', 'HEAD'], cwd=ROOT, text=True).strip()
    # Explicit input paths, never a recursive glob of a build/game directory.
    files = {
        'ddraw.dll': patch_dll('ddraw.dll'),
        'pc2fix.ini': (ROOT / 'pc2fix.ini.example').read_bytes(),
        'README.md': (ROOT / 'README.md').read_bytes(),
        'LICENSE': (ROOT / 'LICENSE').read_bytes(),
        'vendors/minhook-master/LICENSE.txt':
            (ROOT / 'vendors/minhook-master/LICENSE.txt').read_bytes(),
        'tests/README.md': (ROOT / 'tests/README.md').read_bytes(),
    }
    for path in sorted((ROOT / 'docs').glob('*.md')):
        files['docs/' + path.name] = path.read_bytes()
    # Reject local directory paths, including embedded CodeView/PDB paths.
    local_path = re.compile(rb'(?<![A-Za-z0-9])[A-Za-z]:[\\/][A-Za-z0-9_ .()-]{3,}[\\/]')
    for name, data in files.items():
        if local_path.search(data) or local_path.search(data.replace(b'\0', b'')):
            raise ValueError('Local directory path in package input: ' + name)
    hashes = {name: digest(data) for name, data in files.items()}
    files['BUILD_INFO.json'] = (json.dumps({
        'version': version, 'commit': commit, 'architecture': 'x86',
        'configuration': 'Release', 'files_sha256': hashes,
    }, indent=2) + '\n').encode()
    destination = ROOT / 'out' / ('Pizza _Connection 2_Jodkas_Patch_' + version + '.zip')
    with zipfile.ZipFile(destination, 'w', compression=zipfile.ZIP_DEFLATED) as archive:
        for name, data in sorted(files.items()):
            info = zipfile.ZipInfo(name, date_time=(2026, 1, 1, 0, 0, 0))
            info.compress_type = zipfile.ZIP_DEFLATED
            archive.writestr(info, data)
    with zipfile.ZipFile(destination) as archive:
        if archive.testzip() is not None or set(archive.namelist()) != set(files):
            raise ValueError('Archive verification failed')
        for name, data in files.items():
            if archive.read(name) != data:
                raise ValueError('Archive content mismatch: ' + name)
    print(json.dumps({'archive': str(destination),
                      'commit': commit, 'files': len(files), 'sha256': digest(destination.read_bytes())}))


if __name__ == '__main__':
    package(sys.argv[1])
