import bluetooth
import sys

if len(sys.argv) != 3:
  print('Usage: bt.py <ADDRESS> <MESSAGE>')
  exit(0)

bt_addr = sys.argv[1]
message = sys.argv[2]
port = 1
sock = bluetooth.BluetoothSocket( bluetooth.RFCOMM )
sock.connect((bt_addr, port))

sock.send(message + '\n')
sock.close()

