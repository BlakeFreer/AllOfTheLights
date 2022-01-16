##Author: Ivan Lange
##Date: 15 Jan 2022
##Purpose: To request data from the local host, and relay it to the client pi's. 

import socket           #Imports socket, thread, and time libraries
from _thread import *
import time
import requests
import json
import rgb2serial
import serial

serverSocket = socket.socket()  #Creates a socket object
host = '192.168.0.78'           #Server ID (IP of PC/Server)
port = 12345                    #Port number
myClients = []                  #Declares an empty list to store each client object connected

firstRun = True #Boolean for first get request from the local host.
response = {}   #Initialize response dictionary from local host.
lastResponse = {}   #Initiliaze last response dictionary.


def decodeText(message):                        #Function to decode message from server
    decodedMessage = message.decode('utf-8')    #Decodes the encoded message
    return decodedMessage                       #Returns the decoded message

def encodeText(message):                        #Function to encode text
    encodedMessage = bytes(message, 'utf-8')    #Encodes the text
    return encodedMessage                       #Returns the encoded text

##def threadedClient(connection):                                #When a new thread is started, threadedClient code begins
##    connection.sendall(encodeText('Welcome to the Server'))    #Sends an encoded message to the client
##    while True:
##        data = connection.recv(1024)                           #Waits to receive a message
##        if decodeText(data) == '0':                            #If the client is properly connected (i.e.sends '0'), the server sends them a command
##            command = input("Enter command: ")
##            sendText(command)                                  #Sends command through sendText method
##    connection.close()

def threadedWeb(connection):
    global firstRun     #Global variables and object
    global response
    global lastResponse
    global ser
    while True:
        response = requests.get("http://192.168.0.35:5000/query").json()        #Get request from the local host
        if(firstRun):                                                           #If the first get request, updare firstRun, set last response equal to response and send the message to client
            firstRun = False
            lastResponse = response
            sendText(response["pattern"])

        if(response != lastResponse):           #If the current response is different than the last response, update the response and send it to the client.
            sendText(response["pattern"])
            for byte in rgb2serial.patternToPackets(response["pattern"]):   #Reads in consecutive byes from packet and writes them to serial.
                ser.write(byte)
        lastResponse = response;    #Update the last response variable
        time.sleep(1)               #Sleep 

def sendText(message):                          #Function to send a message to all clients
    encodedMessage = encodeText(message)        #Encodes the message given in the function parameter
    for client in myClients:                    #Cycles through each client and sends them the encoded message
        client.sendall(encodedMessage)          
        time.sleep(0.1)

def addClient():                                                           #Function to add a client to the server
    while True:                                                            #The loop will run until the server code is terminated
        global myClients
        client, address = serverSocket.accept()                            #Waits for a new client connection, accepts it, and makes an object for them and stores their ID and port
        print('Connected to: ' + address[0] + ':' + str(address[1]))       #Prints the client's ID and port
        start_new_thread(threadedWeb, (client, ))                          #Starts a thread for that object, to run the threadedClient function
        myClients.append(client)                                           #Adds the client object to a global list of clients

def main():
    global ser
    ser = serial.Serial('/dev/ttyAMA0', 9600)
    try:                                    #Initializes the server socket with host IP and port (requires a tuple)
        serverSocket.bind((host, port))
    except socket.error as e:               #Handles an error and prints it if required
        print(str(e))

    print('Waiting for a Connection...')    
    serverSocket.listen(5)                  #listen is required to allow the server to use the accept method
    addClient()                             #Runs the addClient function
    serverSocket.close()                    #Closes the socket when terminated

if __name__ == "__main__":
    rgb2serial.test()   #Run the rgb2serial code
    main()
