import wave
import struct
import os
import sys
from pathlib import Path

def parse_wav(filename: str) -> list[int]:
    channels = 0
    frames = 0
    framerate = 0
    width = 0
    raw = b''

    with wave.open(filename, mode='rb') as f:
        # Get file info
        channels = f.getnchannels()
        frames = f.getnframes()
        framerate = f.getframerate()
        width = f.getsampwidth()

        # Get raw data
        raw = f.readframes(frames)

    # Parse out into 16 bit samples
    samples = []
    for i in range(0, len(raw), width*channels):
        frame = []
        for channel in range(channels):
            s = i + channel*width
            d = struct.unpack('<h', raw[s:s+2])[0]
            frame.append(d)
        mono = sum(frame)/len(frame)
        samples.append(mono/32768)

    # Trim whitespace from wav file
    start = 0
    end = len(samples)
    while start < end and abs(samples[start]) < 0.02:
        start += 1
    while end > start and abs(samples[end-1]) < 0.02:
        end -= 1
    samples = samples[start:end]
    
    # Resample to framerate of microcontroller
    r = 4000 / framerate
    msamples = []
    i = 0.0
    while int(i) < len(samples):
        msamples.append(samples[int(i)])
        i += 1.0/r

    # Convert to 12-bit values to send to the DAC
    dacsamples = []
    for s in msamples:
        s = max(-1.0, min(1.0, s))
        dv = (int(((s*.5)+.5)*4095) & 0xFF0) >> 4
        dacsamples.append(dv)

    sfilename = os.path.splitext(filename)[0]
    hname = Path(filename).stem
    hname = ''.join(filter(str.isalnum, hname)).upper()
    with open(sfilename + ".h", "w", encoding="utf-8") as f:
        f.write("#ifndef " + hname + "_H\n")
        f.write("#define " + hname + "_H\n\n")
        f.write("#include <avr/pgmspace.h>\n\n")
        f.write("const uint8_t " + hname.lower() + "[" + str(len(dacsamples)) + "] PROGMEM = {" + ', '.join(list(map(str, dacsamples))) + "};\n")
        f.write("\n#endif\n")

if __name__ == "__main__":
    parse_wav(sys.argv[1])
