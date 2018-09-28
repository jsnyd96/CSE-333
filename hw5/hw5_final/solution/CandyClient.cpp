#include <iostream>
#include <string>

#include "CSE333.h"
#include "ClientSocket.h"
#include "hw4/GameInstance.h"
extern "C" {
    #include "jansson.h"
}

#define BUF_SIZE    2048

using namespace hw5_net;
using namespace std;

string getNextMessage(ClientSocket socket);

int main(int argc, char *argv[]) {
    // Set up and connect socket.
    if (argc != 3) {
        printf("Usage: ./CandyClient HostName PortNumber\n");
        return 1;
    }

    string hostName(argv[1]);
    int port;
    sscanf(argv[2], "%i", &port);

    ClientSocket socket(hostName, port);    

    // Send 'hello' message to server.
    json_t *root = json_object();
    json_t *action = json_string("hello");
    json_object_set_new(root, "action", action);
    char *greetingsDump = json_dumps(root, JSON_ENCODE_ANY);
    string greeting(greetingsDump);
    socket.WrappedWrite(greeting.c_str(), greeting.length());
    free(greetingsDump);
    json_decref(root);
    printf("Sent greeting\n");

    // Recieve 'helloack' and create GameInstance from attached string.
    // Going to need to add a constructor to GameInstance that takes a json string rather
    // than a file path.
    char buf[BUF_SIZE];
    int readCount;
    
    // Sits here until a message is recieved.
    while (!(readCount = socket.WrappedRead(buf, BUF_SIZE - 1)));
    buf[readCount] = '\0';

    root = json_loads(buf, JSON_DECODE_ANY, NULL);
    string actionHelloAck(json_string_value(json_object_get(root, "action")));
    if (actionHelloAck.compare("helloack") != 0) {
        json_decref(root);
        return 1;
    }
    char *instanceDump = json_dumps(json_object_get(root, "gameinstance"), JSON_ENCODE_ANY);
    printf("Recieved helloack\n");
    free(instanceDump);

    // Perform initial server GameInstance update.

    GameInstance instance(json_object_get(root, "gameinstance"));
    printf("Created GameInstance model \n");
    json_decref(root);


    root = json_object();
    json_object_set_new(root, "action", json_string("update"));
    json_object_set_new(root, "gameinstance", instance.save());
    instanceDump = json_dumps(root, JSON_ENCODE_ANY);
    string instanceString(instanceDump);
    socket.WrappedWrite(instanceString.c_str(), instanceString.length());
    free(instanceDump);
    json_decref(root);

    printf("Sent initial gameinstance\n");

    // Loop for reading the message from the server and performing the required action.
    // Respond with updated game state.
    while (true) {
        while (!(readCount = socket.WrappedRead(buf, BUF_SIZE - 1)));
        buf[readCount] = '\0';
        
        root = json_loads(buf, JSON_DECODE_ANY, NULL);
        string actionUpdate(json_string_value(json_object_get(root, "action")));
        
        // Exit loop upon recieving bye message.
        if (actionUpdate.compare("bye") == 0) {
            printf("Recieved bye message\n");
            json_decref(root);
            break;
        }

        if (!actionUpdate.compare("move") == 0) {
            json_decref(root);
            return 1;
        }

        // Parse movement fields.
        int row = json_integer_value(json_object_get(root, "row"));
        int column = json_integer_value(json_object_get(root, "column"));
        int direction = json_integer_value(json_object_get(root, "direction"));
        json_decref(root);

        instance.swap(row, column, direction);

        printf("Performed swap\n");

        // Send back updated board.
        root = json_object();
        json_object_set_new(root, "gameinstance", instance.save());
        json_object_set_new(root, "action", json_string("update"));
        char* newInstanceDump = json_dumps(root, JSON_ENCODE_ANY);
        string newInstanceString(newInstanceDump);
        socket.WrappedWrite(newInstanceString.c_str(), newInstanceString.length());
        free(newInstanceDump);
        json_decref(root);

        printf("Sent gameinstance update\n");
    }
}

