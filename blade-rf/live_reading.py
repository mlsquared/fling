from bladerf import _bladerf
import numpy as np
import time

sdr = _bladerf.BladeRF()
rx_ch = sdr.Channel(_bladerf.CHANNEL_RX(0))


sample_rate = 10e6  #10 mhz
center_freq = 100e6  #100 mhz
gain = 50  #50 db
update_interval = 0.5  # .5 seconds
buffer_size = int(sample_rate * update_interval)  

rx_ch.frequency = center_freq
rx_ch.sample_rate = sample_rate
rx_ch.bandwidth = sample_rate / 2
rx_ch.gain_mode = _bladerf.GainMode.Manual
rx_ch.gain = gain

sdr.sync_config(layout=_bladerf.ChannelLayout.RX_X1,
                fmt=_bladerf.Format.SC16_Q11,
                num_buffers=16,
                buffer_size=8192,
                num_transfers=8,
                stream_timeout=3500)

rx_ch.enable = True  

try:
    while True:
        buf = bytearray(buffer_size * 4)  
        sdr.sync_rx(buf, buffer_size)

        #converting from raw format
        samples = np.frombuffer(buf, dtype=np.int16)
        samples = samples[0::2] + 1j * samples[1::2]
        samples = samples / 2048.0  #scaling from -1 to 1

        #outputting max signal strength
        signal_strength = np.max(np.abs(samples))
        print(f"Signal strength: {signal_strength:.4f}")

        time.sleep(update_interval)

except KeyboardInterrupt:
    print("Canceled")

rx_ch.enable = False

