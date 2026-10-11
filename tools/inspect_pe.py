"""Read-only PE metadata and ABI audit. Never writes or executes the input image."""
import argparse
import hashlib
import json
from pathlib import Path
import pefile


def inspect(path):
    data = path.read_bytes()
    image = pefile.PE(data=data)
    return {
        'file_size': len(data), 'sha256': hashlib.sha256(data).hexdigest(),
        'machine': hex(image.FILE_HEADER.Machine),
        'timestamp': hex(image.FILE_HEADER.TimeDateStamp),
        'image_base': hex(image.OPTIONAL_HEADER.ImageBase),
        'entry_point': hex(image.OPTIONAL_HEADER.AddressOfEntryPoint),
        'size_of_image': hex(image.OPTIONAL_HEADER.SizeOfImage),
        'imports': {entry.dll.decode(): [item.name.decode() if item.name else '#' + str(item.ordinal)
            for item in entry.imports] for entry in getattr(image, 'DIRECTORY_ENTRY_IMPORT', [])},
        'sections': [{'name': section.Name.rstrip(b'\0').decode(), 'rva': hex(section.VirtualAddress),
            'virtual_size': hex(section.Misc_VirtualSize),
            'sha256': hashlib.sha256(section.get_data()[:section.Misc_VirtualSize]).hexdigest()}
            for section in image.sections]
    }


def audit(proxy_path, original_path):
    proxy = pefile.PE(str(proxy_path))
    original = pefile.PE(str(original_path))
    a = {entry.name: entry for entry in proxy.DIRECTORY_ENTRY_EXPORT.symbols}
    b = {entry.name: entry for entry in original.DIRECTORY_ENTRY_EXPORT.symbols}
    if a.keys() != b.keys():
        raise ValueError('Export name sets differ')
    if proxy.FILE_HEADER.Machine != 0x14c:
        raise ValueError('Proxy must be x86')
    for name, entry in b.items():
        expected = None if name == b'_BinkSetSoundSystem@8' else b'binkw32o.' + name
        if a[name].ordinal != entry.ordinal or a[name].forwarder != expected:
            raise ValueError('ABI mismatch: ' + name.decode())
    return {'exports': len(b), 'result': 'PASS: names, ordinals, forwarders and x86 architecture'}


if __name__ == '__main__':
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('image', type=Path)
    parser.add_argument('--original-bink', type=Path)
    args = parser.parse_args()
    print(json.dumps(audit(args.image, args.original_bink) if args.original_bink else inspect(args.image), indent=2))
