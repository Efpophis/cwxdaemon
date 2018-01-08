#include <iostream>
#include <cstdlib>
#include "udpsocket.h"
#include "clisocket.h"
#include "keyer.h"

using namespace std;

// any bigger than this and we have to 
// wait for the keyer's buffer to go ready
// before sending any more to it.
#define KEYER_MAX_BUFFER_SIZE (256)
#define ESC (27)

void daemon( UdpSocket&, CWXKeyer& );

int main( int argc, char* argv[])
{
    // TODO: map out args and use optarg here
    if ( argc != 3 )
    {
        cout << "Usage: " << argv[0] << " [udp port] [host name/ip]" << endl;
        exit(1);
    }
    
    unsigned short netPort = atoi( argv[1] );
    string flexHost = argv[2];
    
    UdpSocket mySocket;
    CWXKeyer myKeyer;
    
    if ( mySocket.initialize( netPort ) )
    {   
        if ( myKeyer.init( flexHost.c_str() ) )
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
        cerr << "Failed to initialize udp socket on port " << netPort << endl;
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
