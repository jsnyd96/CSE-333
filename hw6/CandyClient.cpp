#include <iostream>
#include <string>
#include <atomic>
#include <thread>
#include <vector>
#include <future>
#include <mutex>

#include "CSE333.h"
#include "ClientSocket.h"
#include "hw4/GameInstance.h"
#include "Move.h"
extern "C" {
    #include "jansson.h"
}

#define BUF_SIZE    10000

using namespace hw5_net;
using namespace std;

void appendName(json_t* root);
void getBoardValue(int* boardValue, GameInstance instance, int depth, int *totalMoves);
Move getBestMove(GameInstance instance, int *totalMoves);

std::mutex mtx;

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
    //printf("Recieved helloack:\n %s\n", instanceDump);
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
        json_decref(root);

        // Evaluate board and send back new mymove action.

        //vector<Move*> moves;
        // spawn threads for each move

        printf("Finding moves...\n");
        
        Move bestMove = getBestMove(instance, &totalMoves);
        printf("Best Move is %i, %i, %i, with score %i\n", bestMove.getRow(),
            bestMove.getColumn(), bestMove.getDirection(), bestMove.getScore());
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
        instance.swap(bestMove.getRow(), bestMove.getColumn(), bestMove.getDirection());

        // Send move to server
        json_t *root = json_object();
        json_object_set_new(root, "action", json_string("mymove"));
        appendName(root);
        json_object_set_new(root, "gameinstance", instance.save());
        json_object_set_new(root, "move", bestMove.toJson());
        json_object_set_new(root, "movesevaluated", json_integer(totalMoves));       

        char* newInstanceDump = json_dumps(root, JSON_ENCODE_ANY);
        //printf("Sent json:\n %s\n", newInstanceDump);
        string newInstanceString(newInstanceDump);
        socket.WrappedWrite(newInstanceString.c_str(), newInstanceString.length());
        free(newInstanceDump);
        json_decref(root);

        printf("Sent mymove\n");
    }
}

Move getBestMove(GameInstance instance, int *totalMoves) {
    vector<Move*> moves;
    for (int i = 0; i < instance.getRowCount(); i++) {
        for (int j = 0; j < instance.getColCount() - 1; j++) {
            GameInstance* cp = new GameInstance(instance);
            if ((*cp).swap(i, j, 1)) {
                Move* move = new Move(i, j, 1);
                int* value = new int();
                getBoardValue(value, *cp, 0, totalMoves);
                move->setScore(*value);
                delete(value);
                moves.push_back(move);
                
                delete(cp);
            } else {
                delete(cp);
            }
        }
    }
    
    int bestI = -1;
    int bestScore = -1;
    for (int i = 0; i < (int)moves.size(); i++) {
        printf("Move %i, %i, %i, has score %i\n", moves[i]->getRow(),
            moves[i]->getColumn(), moves[i]->getDirection(), moves[i]->getScore());

        if ((*moves[i]).getScore() > bestScore) {
            bestI = i;
            bestScore = (*moves[i]).getScore();
        }
    }

    Move bestMove = *(moves[bestI]);

    for (int i = 0; i < (int)moves.size(); i++) {
        delete(moves[i]);
    } 

    // Need to handle freeing moves.
    return bestMove;
}

// Takes in a board state.  Uses bestMove to store the best move from that state.
// and its associated score. 
void getBoardValue(int* boardValue, GameInstance instance, int depth, int *totalMoves) {
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

    if (depth == 3) {
        *boardValue = instance.getScore();
        return;
    }

    vector<int*> boardValues;
    vector<thread> threads;

    for (int i = 0; i < instance.getRowCount(); i++) {
        for (int j = 0; j < instance.getColCount() - 1; j++) {
            GameInstance cp(instance);
            // Locked to avoid multiple threads changing at once.
            mtx.lock();
            (*totalMoves)++;
            mtx.unlock();

            if ((cp).swap(i, j, 1)) {
                int* childValue = new int();
                boardValues.push_back(childValue);
                threads.push_back(thread(getBoardValue, childValue,
                cp, depth+1, totalMoves));    
            } 
        }
    }

    // Join all threads.
    for (int i = 0; i < (int)threads.size(); i++) {
        threads[i].join();
    }

    // Iterate through return array
    int bestValue = -1;
    for (int i = 0; i < (int)boardValues.size(); i++){
        // Replace scores in moves with child move scores.
        if (*boardValues[i] > bestValue) {
            bestValue = *boardValues[i];
        }
    }

    // Delete all the return ints.
    for (int i = 0; i < (int)boardValues.size(); i++) {
        delete(boardValues[i]);
    }

    *boardValue = bestValue;
    return;
}





// Adds the team's name to the json text before sending to client.
void appendName(json_t* root) {
    json_t* name = json_string("The Silver Hand");
    json_object_set_new(root, "teamname", name);
}
