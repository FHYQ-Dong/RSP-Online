import socket

# Define the server's IP address and port
HOST = '127.0.0.1'
PORT = 1234

# Create a TCP socket
server_socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)

# Bind the socket to the specified address and port
server_socket.bind((HOST, PORT))

# Listen for incoming connections
server_socket.listen()

print(f"Server listening on {HOST}:{PORT}")

while True:
    # Accept a client connection
    client_socket, client_address = server_socket.accept()
    print(f"Accepted connection from {client_address[0]}:{client_address[1]}")

    # Receive data from the client
    data = client_socket.recv(1024)#.decode()
    print(f"Received data from client: {data.hex()}")

    # Send a response back to the client
    response = "Hello"
    client_socket.sendall(response.encode("ascii"))

    # Close the client socket
    client_socket.close()
