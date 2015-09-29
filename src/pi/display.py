# Bluetooth server code
import bluetooth

server_addr = "A0:A8:CD:B5:52:97"
client_addr = "2C:8A:72:ED:BE:F1"
UUID = "d4b5ed27-e431-4c56-876e-404343f38a9b"
port = 45067

server_sock = bluetooth.BluetoothSocket(bluetooth.RFCOMM)
server_sock.bind(("",port))
server_sock.listen(1)

bluetooth.advertise_service(server_sock, "HyTech Racing", UUID)

print "Waiting for connection..."
client_sock = server_sock.accept()[0]
print "Client found"
while True:
    data_length = int(client_sock.recv(1).encode('hex'), 16)
    print client_sock.recv(data_length)
