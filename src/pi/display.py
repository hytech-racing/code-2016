# Bluetooth server code
import bluetooth
from struct import unpack

server_addr = "A0:A8:CD:B5:52:97"
client_addr = "2C:8A:72:ED:BE:F1"
UUID = "d4b5ed27-e431-4c56-876e-404343f38a9b"
port = 45067

def create_server_and_client_socket():
    print "## Waiting for connection..."
    ss = bluetooth.BluetoothSocket(bluetooth.RFCOMM)
    ss.bind((server_addr, port))
    ss.listen(1)
    bluetooth.advertise_service(ss, "HyTech Racing", UUID)
    cs = ss.accept()[0]
    print "## Client found"
    return (ss, cs)

server_sock, client_sock = create_server_and_client_socket()

while True:
    try:
        # data_length = unpack("<i", client_sock.recv(4))[0]
        print client_sock.recv(8)
    except bluetooth.btcommon.BluetoothError:
        print "## Lost Bluetooth connection"
        server_sock.close()
        server_sock, client_sock = create_server_and_client_socket()
        continue
