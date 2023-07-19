import asyncio
from bleak import BleakClient

INPUT_CHARACTERISTIC = "a30b08c5-94a3-4678-a026-47dcf3ebec1f"
OUTPUT_CHARACTERISTIC = "4c599c12-9cff-4d7b-aea6-b811907f367e"


async def main():
    address = '08EF25D5-5FF3-2617-E116-8F9E689857D1'
    async with BleakClient(address) as client:
        print(f"Connected: {client.is_connected}")
        print("Sending data...")
        await client.write_gatt_char(INPUT_CHARACTERISTIC, b"ABC", response=True)
        print("Data sent")
        await asyncio.sleep(1.0)
        print("Reading response...")
        response = await client.read_gatt_char(OUTPUT_CHARACTERISTIC)
        print(f"response: {response}")


if __name__ == "__main__":
    asyncio.run(main())
