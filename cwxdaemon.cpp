#include <iostream>
#include <cstdlib>
#include <getopt.h>
#include "udpsocket.h"
#include "clisocket.h"
#include "keyer.h"

using namespace std;

// any bigger than this and we have to 
// wait for the keyer's buffer to go ready
// before sending any more to it.
#define KEYER_MAX_BUFFER_SIZE (256)
#define ESC (27)
#define DEFAULT_CWD_PORT (60000)
#define FLEX_API_PORT (4992)

void daemon( UdpSocket&, CWXKeyer& );

int main( int argc, char* argv[])
{
      
    unsigned short udpPort  = DEFAULT_CWD_PORT; //atoi( argv[1] );
    unsigned short flexPort = FLEX_API_PORT;
    string flexHost = "localhost"; // = argv[2];
    
    UdpSocket mySocket;
    CWXKeyer myKeyer;
    int done = 0;
    
    while (!done)
    {
        int this_option_optind = optind ? optind : 1;
        int option_index = 0;
        static struct option long_options[] =
        {
            {"udpport", required_argument, 0, 0},
            {"flexhost", required_argument, 0, 1},
            {"flexport", required_argument, 0, 2},
            {0,0,0,0}
        };
    
        int c = getopt_long(argc,argv,"",long_options, &option_index);
        
        switch (c)
        {
            case 0:
                udpPort = stoi(string(optarg));
            break;

            case 1:
                flexHost = string(optarg);
            break;
            
            case 2:
                flexPort = stoi(string(optarg));
                cerr << "WARNING: overriding Flex API port to " << flexPort << endl;
            break;
            
            default:
            case -1:
                done = 1;
            break;    
            
        }
    }
    
    cout << "listening on UDP: " << udpPort << endl;
    cout << "Connecting to host: " << flexHost << " : " << flexPort << endl;
        
    if ( mySocket.initialize( udpPort ) )
    {   
        if ( myKeyer.init( flexHost.c_str(), flexPort ) != -1 )
        {
            daemon( mySocket, myKeyer );
        }
        else
        {
            cerr << "Failed to initialize keyer on port " << flexHost << endl;
            exit(1);
        }
    }
    else
    {
        cerr << "Failed to initialize udp socket on port " << udpPort << endl;
        exit(1);    
    }   
    
    return 0;
}


// #define CWDAEMON_MESSAGE_SIZE_MAX        256 /* Maximal size of single message. */
// #define CWDAEMON_REQUEST_QUEUE_SIZE_MAX 4000 /* Maximal size of common buffer/fifo where requests may be pushed to. */


void daemon( UdpSocket& mySocket, CWXKeyer& myKeyer )
{
    bool done = false;

    
    while ( !done )
    {
        unsigned char msg[KEYER_MAX_BUFFER_SIZE] = {0};
        unsigned short port = 0;
        bool txEnabled = true;
        int bytesSent;
        
        int bytesRead = mySocket.receive(msg, sizeof(msg), &port );
        
        if ( bytesRead > 0 )
        {
            if ( msg[0] == ESC ) // 27 is ESC, so this is a command, not a msg
            {
                switch (msg[1])
                {
                    case '2':
                    {
                        int wpm = atoi((char*)(&msg[2]));
                        
                        myKeyer.setWPM( wpm );
                    }
                    break;
                    
                    case '4':
                        myKeyer.abortCW();
                    break;
                    
                    case '5':
                        cout << "Exiting ts2kdaemon by client request" << endl;
                        done = true;
                    break;
                    
                    default:
                    break;                
                }
            }
            else
            {                
                //cout << string((char*)msg) << endl;
                                
                bytesSent = myKeyer.transmitCW( (char*)msg, bytesRead );
                
                if ( bytesSent == -1 )
                {
                    cout << "Failed to send message: bytesSent = " << bytesSent << ", bytesRead = " << bytesRead << endl;
                }
            }
        }
        else
        {
            cout << "Something went wrong: read " << bytesRead << " from UDP socket." << endl;
            done = true;
        }        
    }
}
