import asyncio
from bleak import BleakScanner


async def main_all():
    print("scanning for 5 seconds, please wait...")

    devices = await BleakScanner.discover(
        return_adv=True
    )
    

    for d, a in devices.values():
        print()
        print(d)
        print("-" * len(str(d)))
        print(a)

async def main():
    print("scanning for 5 seconds, please wait...")

    device = await BleakScanner.find_device_by_name("Key-Value Storage", timeout = 2)
    if device:
        print(device)
    else:
        print("Not found")


if __name__ == "__main__":
    asyncio.run(main())