import wave

def parse_wav(filename: str) -> list[int]:
    with wave.open(filename, mode='rb') as f:
        channels = f.getnchannels()
        frames = f.getnframes()
        
        print(f.getnchannels(), f.getframerate(), f.getnframes())

if __name__ == "__main__":
    parse_wav("audio/sample-3s.wav")
