import socket
import json

HOST = "127.0.0.1"  # Server's hostname or IP address
PORT = 101  # Server's port

def connect_to_server():
    """Establishes a connection to the server."""
    s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    s.connect((HOST, PORT))
    return s

def login_request():
    """Handles login request."""
    code = "1"
    username = input("Enter your username: ")
    password = input("Enter your password : ")

    # Create JSON string for the request
    request_json = json.dumps({"username": username, "password": password})
    
    len_byte_string = len(request_json).to_bytes(4, byteorder='little')

    # Format data: code, length of JSON data as a 4-digit zero-padded string, JSON data
    data = code.encode() + len_byte_string + request_json.encode()
    return data
1
def signup_request():
    """Handles signup request."""
    code = "2"
    username = input("Enter your username: ")
    password = input("Enter your password: ")
    email = input("Enter your email: ")

    # Create JSON string for the request
    request_json = json.dumps({"username": username, "password": password, "email": email})
    
    len_byte_string = len(request_json).to_bytes(4, byteorder='little')

    # Format data: code, length of JSON data as a 4-digit zero-padded string, JSON data
    data = code.encode() + len_byte_string + request_json.encode()
    return data

def error_request():
    """Handles error request."""
    code = "56"
    request_json = '{"username": ""}'

    # Format data: code, length of JSON data as a 4-digit zero-padded string, JSON data
    len_byte_string = len(request_json).to_bytes(4, byteorder='little')

    # Format data: code, length of JSON data as a 4-digit zero-padded string, JSON data
    data = code.encode() + len_byte_string + request_json.encode()
    return data

def main():
    # Connect to the server
    with connect_to_server() as s:
        # Prompt for user choice
        choice = int(input("Hi and welcome to the trivia game:\n1. Login\n2. Signup\n3. Error\n"))

        while choice not in [1, 2, 3]:
            choice = int(input("Please enter a valid number (1, 2, or 3): "))

        if choice == 1:
            data = login_request()
        elif choice == 2:
            data = signup_request()
        else:
            data = error_request()

        # Send data to the server
        s.sendall(data)

        # Receive and print response from the server
        recv_data = s.recv(1024).decode()
        json_data= recv_data.split('{',1)[1]
        json_data= '{'+json_data

        json_parsed = json.loads(json_data)
        print(json_parsed)
        if (json_parsed["status"] == 1 ):
            print("Succeded")
        else:
            print("Failed")





if __name__ == "__main__":
    main()
