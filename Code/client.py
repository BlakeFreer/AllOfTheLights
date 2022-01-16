##Author: Ivan Lange
##Date: 15 Jan 2022
##Purpose: To accept data from the server pi, and use it to modify the lights.

import socket   #Imports socket library
import rgb2serial #Imports te rgb2serial code
import serial #Imports the serial library

clientSocket = socket.socket()  #Declares a socket object
host = '192.168.0.78'           #Server ID (IP of PC/server)
port = 12345                    #Port number


def decodeText(message):                        #Function to decode message from server
    decodedMessage = message.decode('utf-8')    #Decodes the encoded message
    return decodedMessage                       #Returns the decoded message

def encodeText(message):                        #Function to encode text
    encodedMessage = bytes(message, 'utf-8')    #Encodes the text
    return encodedMessage                       #Returns the encoded text

def clientReceive():
    #Function for client to loop and receive commands from server
    global ser
    try:                                        
        while True:                                     #Loop runs until code is terminated               
            encodedMessage = clientSocket.recv(1024)    #Message received from server
            message = decodeText(encodedMessage)        #Message is decoded
            print('received: ' + str(message))          #Prints the message
            for byte in rgb2serial.patternToPackets(message):   #Reads in consecutive byes from packet and writes them to serial.
                ser.write(byte)
    except ConnectionResetError:                        #Exception handling
        print('Host server has disconnected')
    except Exception as e:
        print('Unknown error has occured:', str(e))

def main():
    global ser
    ser = serial.Serial('/dev/serial0', 9600)   #Intializes the serial for the pi
    print('Waiting for connection')
    try:
        clientSocket.connect((host, port))      #Connects to the server
    except socket.error as e:                   #Excepts any errors           
        print(str(e))

    clientReceive()                         #Runs the clientReceive function
    clientSocket.close()                    #Closes the socket

main()
