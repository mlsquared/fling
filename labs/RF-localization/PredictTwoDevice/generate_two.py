# Step 2
# Fixed size room, two devices with / without noise (True / False), with interference
import pandas as pd
import numpy as np
import random
import math

# Configurable Constants
room_size = (5.0, 5.0, 2.5)  # width, depth, height
speed_of_light = 3e8  # m/s

device_types = ['WiFi', 'LTE', '5G']
path_loss_exponents = {'WiFi': 2.2, 'LTE': 2.0, '5G': 2.6}
freq_ranges = {'WiFi': (2.4e9, 5e9), 'LTE': (700e6, 2.6e9), '5G': (3.3e9, 6e9)}

# Fixed SDR positions on TV
sdr_positions = [
    (1.0, 5.0, 1.0),             # Bottom-left
    (2.55, 5.0, 1.0),            # Bottom-right
    (2.55, 5.0, 1.87)            # Top-right
]

def random_position(margin=0.5):
    return (
        random.uniform(margin, room_size[0] - margin),
        random.uniform(margin, room_size[1] - margin),
        random.uniform(0.5, room_size[2] - 0.5)
    )

def simulate_signal(device_pos, sdrs, freq, device_type, packet_len, timestamp, device_id=0, noise=True):
    X = {"device_id": device_id, "timestamp": timestamp}
    for i, sdr_pos in enumerate(sdrs):
        dx, dy, dz = np.array(device_pos) - np.array(sdr_pos)
        distance = math.sqrt(dx**2 + dy**2 + dz**2)
        time_ns = (distance / speed_of_light) * 1e9
        path_loss = path_loss_exponents[device_type]
        rssi = -20 * path_loss * np.log10(distance + 1e-3)
        if noise:
            rssi += np.random.normal(0, 1.5)
            time_ns += np.random.normal(0, 0.5)
        X[f"rssi_sdr_{i+1}"] = rssi
        X[f"arrival_time_sdr_{i+1}"] = time_ns
    X['center_freq_hz'] = freq
    X['packet_length_bytes'] = packet_len
    X['device_type'] = device_type
    X['device_x'], X['device_y'], X['device_z'] = device_pos
    return X

def generate_dataset(samples_per_device=300, interference_count=200, noise=True):
    rows = []
    for device_id in [0, 1]:
        device_pos = random_position()
        device_type = random.choice(device_types)
        for i in range(samples_per_device):
            freq = random.uniform(*freq_ranges[device_type])
            packet_len = random.randint(50, 1500)
            timestamp = round(i * 0.1, 2)
            sample = simulate_signal(
                device_pos, sdr_positions, freq, device_type, packet_len,
                timestamp, device_id=device_id, noise=noise
            )
            rows.append(sample)
        
        ### add interference
        for i in range(interference_count):
            pos = random_position()
            type_ = random.choice(device_types)
            freq = random.uniform(*freq_ranges[type_])
            packet_len = random.randint(50, 1500)
            timestamp = round((samples_per_device * 2 + i) * 0.1, 2)
            sample = simulate_signal(pos, sdr_positions, freq, type_, packet_len,
                                    timestamp, device_id=-1, noise=noise)
            rows.append(sample)
    return pd.DataFrame(rows)

# Generate the Stage 1 dataset
samples_per_device=10000
interference_count=20000
stage_1_df = generate_dataset(samples_per_device, noise=True)
stage_1_df.to_csv(f"data/two_{samples_per_device}_{interference_count}.csv", index=False)