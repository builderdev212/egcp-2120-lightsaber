import wave
import struct

def parse_wav(filename: str) -> list[int]:
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
        
        # Resample to framerate of microcontroller
        r = 8000 / framerate
        msamples = []
        i = 0.0
        while int(i) < len(samples):
            msamples.append(samples[int(i)])
            i += 1.0/r

        dacsamples = []
        for s in msamples:
            s = max(-1.0, min(1.0, s))
            dv = int(((s*.5)+.5)*4095)
            dacsamples.append(dv)


if __name__ == "__main__":
    parse_wav("audio/sample-3s.wav")
