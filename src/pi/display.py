import bluetooth

server_addr = "A0:A8:CD:B5:52:97"
client_addr = "2C:8A:72:ED:BE:F1"
UUID = "00001101-0000-1000-8000-00805F9B34FB"
port = 1
DATA_LENGTH = 20

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
        print client_sock.recv(DATA_LENGTH)
    except bluetooth.btcommon.BluetoothError:
        print "## Lost Bluetooth connection"
        server_sock.close()
        server_sock, client_sock = create_server_and_client_socket()
