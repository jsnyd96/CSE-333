#include <iostream>
#include <string>
#include <atomic>
#include <thread>
#include <vector>

#include "CSE333.h"
#include "ClientSocket.h"
#include "hw4/GameInstance.h"
#include "Move.h"
extern "C" {
    #include "jansson.h"
}

#define BUF_SIZE    2048

using namespace hw5_net;
using namespace std;

void appendName(json_t* root);
void getBestMove(Move* &bestMove, GameInstance instance, int depth, int &totalMoves);


// Handles communication with the server.
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
    appendName(root);
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
    
    // Sits here until helloack is recieved.
    while (!(readCount = socket.WrappedRead(buf, BUF_SIZE - 1)));
    buf[readCount] = '\0';

    root = json_loads(buf, JSON_DECODE_ANY, NULL);
    string actionHelloAck(json_string_value(json_object_get(root, "action")));
    if (actionHelloAck.compare("helloack") != 0) {
        json_decref(root);
        return 1;
    }
    char *instanceDump = json_dumps(json_object_get(root, "gameinstance"), JSON_ENCODE_ANY);
    printf("Recieved helloack:\n %s\n", instanceDump);
    free(instanceDump);

    // Create initial GameInstance.

    GameInstance instance(json_object_get(root, "gameinstance"));
    printf("Created GameInstance model \n");
    json_decref(root);

    // Loop for reading the message from the server and performing the required action.
    // Respond with updated game state.
    int totalMoves = 0;
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

        if (!actionUpdate.compare("requestmove") == 0) {
            json_decref(root);
            return 1;
        }

        // Evaluate board and send back new mymove action.
        Move* bestMove = new Move(-1, -1, -1);
        (*bestMove).setScore(-1);

        //vector<Move*> moves;
        // spawn threads for each move

        printf("Finding moves...\n");
        
        getBestMove(bestMove, instance, 0, totalMoves);
/*
        for (int i = 0; i < (int)moves.size(); i++) {
            if ((*moves[i]).getScore() > (*bestMove).getScore()) {
                delete(bestMove);
                bestMove = moves[i];
            } else {
                delete(moves[i]);
            }
        }
*/

	// Apply move
	instance.swap(bestMove->getRow(), bestMove->getColumn(), bestMove->getDirection());

        // Send move to server
        json_t *root = json_object();
        json_object_set_new(root, "action", json_string("mymove"));
        appendName(root);
        json_object_set_new(root, "gameinstance", instance.save());
        json_object_set_new(root, "move", (*bestMove).toJson());
        json_object_set_new(root, "movesevaluated", json_integer(totalMoves));       

        char* newInstanceDump = json_dumps(root, JSON_ENCODE_ANY);
	printf("Sent json:\n %s\n", newInstanceDump);
        string newInstanceString(newInstanceDump);
        socket.WrappedWrite(newInstanceString.c_str(), newInstanceString.length());
        free(newInstanceDump);
        json_decref(root);

        delete(bestMove);
        printf("Sent mymove\n");
    }
}

void getBestMove(Move* &bestMove, GameInstance instance, int depth, int &totalMoves) {
/*
    for (int i = 0; i < instance.getRowCount() - 1; i++) {
        for (int j = 0; j < instance.getColCount(); j++) {
            GameInstance* cp = new GameInstance(instance);
            if ((*cp).swap(i, j, 3)) {
                Move* move = new Move(i, j, 3);
                moves.push_back(move);
                (*move).setScore((*cp).getScore());
                totalMoves++;
                delete(cp);

                //thread eval(evalMove, *move, *cp, depth + 1);
            } else {
                delete(cp);
            }
        }
    }
*/

    vector<Move*> moves;
    for (int i = 0; i < instance.getRowCount(); i++) {
        for (int j = 0; j < instance.getColCount() - 1; j++) {
            GameInstance* cp = new GameInstance(instance);
            if ((*cp).swap(i, j, 1)) {
                Move* move = new Move(i, j, 1);
                moves.push_back(move);
                (*move).setScore((*cp).getScore());
                totalMoves++;

                if (depth != 4) {
                    Move* nextBestMove = new Move(-1, -1, -1);
                    (*nextBestMove).setScore(-1);
                    getBestMove(nextBestMove, *cp, depth + 1, totalMoves);
                    (*move).setScore(nextBestMove->getScore());
                }


                delete(cp);
            } else {
                delete(cp);
            }
        }
    }

    for (int i = 0; i < (int)moves.size(); i++) {
        if ((*moves[i]).getScore() > (*bestMove).getScore()) {
            delete(bestMove);
            bestMove = moves[i];
        } else {
            delete(moves[i]);
        }
    }
}





// Adds the team's name to the json text before sending to client.
void appendName(json_t* root) {
    json_t* name = json_string("The Silver Hand");
    json_object_set_new(root, "teamname", name);
}
