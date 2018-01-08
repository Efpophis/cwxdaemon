#include <iostream>
#include <stdio.h>
#include <string>
#include <unistd.h>
#include <errno.h>
#include "keyer.h"

#ifndef RELEASE_BUILD
#define error_message(...) fprintf( stderr, __VA_ARGS__)
#else
#define error_message(...) // do nothing
#endif

using namespace std;

CWXKeyer::CWXKeyer() : m_cwx(), m_wpm(0), m_seq(0)
{
}

const std::string CWXKeyer::Seq(void)
{
    return string("C" + to_string(++m_seq) + "|");
}

void CWXKeyer::setWPM(int wpm)
{    
    string wpmCmd = Seq() + "cwx wpm " + std::to_string(wpm) + "\n";
    
    if ( wpm >= 10 && wpm <= 60 )
    {         
        m_cwx.Write( (unsigned char*)wpmCmd.c_str(), wpmCmd.length() );
                
        m_wpm = wpm;        
    }
}

// current WPM setting (10 - 60) on success, -1 on error
int CWXKeyer::init( const char* host, int port /* = B9600 */)
{
    int result = m_cwx.Open( (char*)host, port );        
    
    return result;
}


int CWXKeyer::abortCW()
{
    int result = -1;
    
    string abortCmd = Seq() + "cwx clear\n";
        
    result = m_cwx.Write( (unsigned char*)abortCmd.c_str(), abortCmd.length() );
    
    if ( result == abortCmd.length() )
    {
        result = 1;
    }
    
    return result;
}


int CWXKeyer::transmitCW( const std::string& msg )
{
    return transmitCW( const_cast<char*>(msg.c_str()), msg.length() );
}

int CWXKeyer::transmitCW( char* msg, unsigned int length )
{
    string txCmd = Seq() + "cwx send \"" + string(msg, length) + "\"\n";
    int txbytes;
    int result = 0;

    txbytes = m_cwx.Write( (unsigned char*)txCmd.c_str(), txCmd.length() );
    
    //cout << txCmd;
            
    if ( txbytes != txCmd.length() )
    {
        error_message("failed to send. txbytes == %d, errno == %d\n", txbytes, errno );
        result = -1;
    }
    else
        result = length;

    return result;
}



