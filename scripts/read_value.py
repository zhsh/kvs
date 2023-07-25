import asyncio
import sys
from bleak import BleakClient, BleakScanner

INPUT_CHARACTERISTIC = "a30b08c5-94a3-4678-a026-47dcf3ebec1f"
OUTPUT_CHARACTERISTIC = "4c599c12-9cff-4d7b-aea6-b811907f367e"


async def main(key):
    device = await BleakScanner.find_device_by_name("Key-Value Storage", timeout = 10)
    if not device:
        print('Device not found.')
        exit(0)
    print('Found the device')

    done = asyncio.Event()

    def callback(sender, data):
        print(f"response: {data.decode()}")
        done.set()

    async with BleakClient(device) as client:
        print(f"Connected: {client.is_connected}")
        await client.start_notify(OUTPUT_CHARACTERISTIC, callback)
        print("Sending data...")
        await client.write_gatt_char(INPUT_CHARACTERISTIC, key.encode(), response=True)
        print("Data sent")
        await asyncio.wait_for(done.wait(), 10)


if __name__ == "__main__":
    if len(sys.argv) < 2:
        print(f"Usage: python3 {sys.argv[0]} VALUE")
        exit(0)
    asyncio.run(main(sys.argv[1]))
