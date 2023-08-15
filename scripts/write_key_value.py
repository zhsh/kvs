import asyncio
import sys
from bleak import BleakClient, BleakScanner

KEY_CHARACTERISTIC = "3dadb224-692c-4e3b-b02f-b5d1c2c43b13"
VALUE_CHARACTERISTIC = "7775c5de-9143-4d69-9619-c4c537e926f7"
READY_CHARACTERISTIC = "9eaa2c33-1f21-441a-924c-8399e5ee9b90"

async def main(key, value):
    device = await BleakScanner.find_device_by_name("Key-Value Storage", timeout = 10)
    if not device:
        print('Device not found.')
        exit(0)
    print('Found the device')

    async with BleakClient(device) as client:
        print(f"Connected: {client.is_connected}")
        await client.write_gatt_char(KEY_CHARACTERISTIC, key.encode(), response=True)
        print("Key sent")
        await client.write_gatt_char(VALUE_CHARACTERISTIC, value.encode(), response=True)
        print("Value sent")
        await client.write_gatt_char(READY_CHARACTERISTIC, b'', response=True)
        print("Ready sent")


if __name__ == "__main__":
    if len(sys.argv) < 3:
        print(f"Usage: python3 {sys.argv[0]} KEY VALUE")
        exit(0)
    asyncio.run(main(sys.argv[1], sys.argv[2]))
