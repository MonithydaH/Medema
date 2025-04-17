import asyncio  # Allow running multiple tasks
import csv  # Handles CSV file
import struct  # Convert binary data to float or int
from datetime import datetime  # Timestamps for data log
from bleak import BleakClient, BleakScanner  # For BLE connection
import matplotlib.pyplot as plt  # Real-time data visualization
import matplotlib.animation as animation
from collections import deque  # Store recent data points for plotting

# UUIDs for BLE Characteristics
DISTANCE_UUID = "38BB1816-10B3-B7D1-E378-EB423309BFEA"
GRADE_UUID = "38BB1816-10B3-B7D2-E378-EB423309BFEA"
MASS_UUID = "38BB1816-10B3-B7D3-E378-EB423309BFEA"

#store latest reading
latest_data = {"distance": None, "grade": None, "mass": None}

# CSV file setup
timestamp = datetime.now().strftime("%Y-%m-%d_%H-%M-%S")
filename = f"Medema_log_{timestamp}.csv"
csv_file = open(filename, mode='w', newline='')
csv_writer = csv.writer(csv_file)
csv_writer.writerow(["Timestamp", "Distance (mm)", "Grade", "Mass (g)"])

# Real-time plotting setup
window_size = None  # Number of recent data points to display
time_window = deque(maxlen=window_size)  # Store timestamps
distance_data = deque(maxlen=window_size)  # Store distance values
mass_data = deque(maxlen=window_size)  # Store mass values

fig, (ax1, ax2) = plt.subplots(2, 1, figsize=(7, 5))
fig.suptitle("Real-time Data Plot")

# Configure Distance Plot
ax1.set_title("Distance vs Time")
ax1.set_xlabel("Time")
ax1.set_ylabel("Distance (mm)")
ax1.set_ylim(0, 20)  # Fixed y-axis from 0 to 20
distance_line, = ax1.plot([], [], label="Distance", color='blue')
ax1.legend()

# Configure Mass Plot
ax2.set_title("Mass vs Time")
ax2.set_xlabel("Time")
ax2.set_ylabel("Mass (g)")
ax2.set_ylim(0, 2500)  # Fixed y-axis from 0 to 2500
mass_line, = ax2.plot([], [], label="Mass", color='red')
ax2.legend()

def update_plot(frame):
    if len(time_window) > 1:  # Ensure there is enough data to plot
        distance_line.set_data(range(len(time_window)), distance_data)
        mass_line.set_data(range(len(time_window)), mass_data)

        ax1.set_xlim(0, len(time_window))  # Keep x-axis scrolling
        ax2.set_xlim(0, len(time_window))

        plt.setp(ax1.xaxis.get_majorticklabels(), rotation=45, ha="right")
        plt.setp(ax2.xaxis.get_majorticklabels(), rotation=45, ha="right")

    return distance_line, mass_line

# Matplotlib animation for real time update (plot update every 500ms)
ani = animation.FuncAnimation(fig, update_plot, interval=500, cache_frame_data=False)

def log_data():
    if latest_data["distance"] is not None and latest_data["grade"] is not None:
        timestamp = datetime.now().strftime("%H:%M:%S")
        print(f"[{timestamp}] Distance: {latest_data['distance']:.2f} mm | Grade: {latest_data['grade']} | Mass: {latest_data['mass']:.2f} g")
        csv_writer.writerow([timestamp, latest_data["distance"], latest_data["grade"], latest_data["mass"]])
        csv_file.flush()
        time_window.append(timestamp)  # Store real-time timestamp
        distance_data.append(latest_data["distance"])
        mass_data.append(latest_data["mass"])

async def main():
    print("🔍 Scanning for BLE devices...")
    devices = await BleakScanner.discover()

    target = None
    for d in devices:
        if d.name and "Arduino" in d.name:
            target = d
            break

    if not target:
        print("❌ Could not find your ESP32C3.")
        return

    print(f" Connecting to {target.name} ({target.address})...")
    async with BleakClient(target.address) as client:
        print("🔗 Connected! Subscribing to data...")

        async def handle_distance(sender, data):
            try:
                latest_data["distance"] = struct.unpack('<f', data)[0]
                raw_grade = await client.read_gatt_char(GRADE_UUID)
                latest_data["grade"] = int.from_bytes(raw_grade, byteorder='little')
                log_data()
            except Exception as e:
                print("❌ Error in distance handler:", e)

        async def handle_mass(sender, data):
            try:
                latest_data["mass"] = struct.unpack('<f', data)[0]
                log_data()
            except Exception as p:
                print("❌ Error in mass handler:", p)

        await client.start_notify(DISTANCE_UUID, handle_distance)
        await client.start_notify(MASS_UUID, handle_mass)

        print("📡 Receiving data. Press Ctrl+C to stop.")
        try:
            while True:
                plt.pause(0.1)  # Keep Matplotlib responsive
                await asyncio.sleep(1)  # Adjust this delay for slower logging
        except asyncio.CancelledError:
            print("\n Stopped by user.")
        finally:
            await client.stop_notify(DISTANCE_UUID)
            await client.stop_notify(MASS_UUID)

try:
    asyncio.run(main())
except KeyboardInterrupt:
    print("\n Stopped by user.")
    csv_file.close()
