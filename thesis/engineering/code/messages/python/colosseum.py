import struct
import os
import sys
import numpy as np

from time import time
from select import select

u8, u16, u32, u64, i8, i16, i32, i64, f32, f64, _char, _bool = range(12)

type_size   = [
    1, 2, 4, 8,
    1, 2, 4, 8,
    4, 8, 1, 1,
]

struct_type = [
    '<B', '<H', '<L', '<Q',
    '<b', '<h', '<l', '<q',
    '<f', '<d', '<c', '<c',
]

numpy_type  = [
    '<u1', '<u2', '<u4', '<u8',
    '<i1', '<i2', '<i4', '<i8',
    '<f4', '<f8', '<u1', '<u1',
]

colo_type   = {
    np.uint8: u8, np.uint16: u16, np.uint32: u32, np.uint64: u64,
    np.int8:  i8, np.int16:  i16, np.int32:  i32, np.int64:  i64,
    np.float32: f32, np.float64: f64, np.bool: _bool,
}

def hexdump(bytes):
    return ' '.join(f'{x:02X}' for x in bytes)

def panic(*args, file=sys.stderr, **kwargs):
    print(*args, file=file, **kwargs)
    sys.exit(1)

def open(f, mode):
    if mode == 'r': return os.open(f, os.O_RDONLY)
    if mode == 'w': return os.open(f, os.O_WRONLY)
    panic(f'unknown mode {mode}')

def close(f):
    os.close(f)

def send(f, tag, *args):
    t1 = time()
    type = bytearray()
    data = bytearray()
    
    for arg in args:
        if isinstance(arg, tuple):
            data += struct.pack(struct_type[arg[1]], arg[0])
            type += struct.pack('<B', arg[1])
        elif isinstance(arg, int):
            data += struct.pack('<L', arg)
            type += struct.pack('<B', i32)
        elif isinstance(arg, str):
            data += np.fromstring(arg, '<u1').tobytes()
            type += struct.pack('<BL', 1 << 4 | _char, len(arg))
        else:
            if isinstance(arg, list):
                arg = np.array(arg)
            if isinstance(arg, np.ndarray):
                dims = len(arg.shape)
                base = colo_type[arg.dtype.type]
                data += arg.tobytes()
                type += struct.pack('<B', dims << 4 | base)
                for dim in arg.shape:
                    type += struct.pack('<L', dim)

    head = struct.pack('<bLL', tag, len(type), len(data))
    pack = head + type + data
    return os.write(f, pack)

def recv(f):
    hlen = 9
    head = os.read(f, hlen)
    while len(head) < hlen:
        select([f], [], [f])
        head += os.read(f, hlen - len(head))
    tag, tlen, dlen = struct.unpack('<bLL', head)
    plen = tlen + dlen
    body = os.read(f, plen)
    while len(body) < plen:
        select([f], [], [f])
        body += os.read(f, plen - len(body))

    type, data = body[:tlen], body[tlen:tlen+dlen]
    args = []
    while type:
        argtype, type  = type[0], type[1:]
        ndim  = argtype >> 4 & 0x07
        base  = argtype & 0x0F
        size  = type_size[base]
        if ndim > 0:
            dims = []
            for x in range(ndim):
                argdim, type  = type[:4], type[4:]
                dim = struct.unpack('<L', argdim)[0]
                size *= dim
                dims.append(dim)
            arg, data = data[:size], data[size:]
            arg = np.frombuffer(arg, dtype=numpy_type[base])
            arg = arg.reshape(dims)
            if base == _char:
                arg = str(arg, encoding='utf-8')
            args.append(arg)

        else:
            arg, data = data[:size], data[size:]
            args.append(struct.unpack(struct_type[base], arg)[0]) 

    return (tag, args)

