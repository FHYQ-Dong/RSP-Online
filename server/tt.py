import socket

# Create a TCP socket
client_socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)

# Connect to the server
server_address = ('localhost', 12345)  # Replace with the actual server address and port
client_socket.connect(server_address)

# Send data to the server
message = "H"*1024
client_socket.sendall(message.encode("ascii"))

# Receive data from the server
data = client_socket.recv(1024)
print("Received:", data.decode())

# Close the socket
client_socket.close()
