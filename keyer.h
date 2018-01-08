#ifndef CWX_KEYER_H
#define CWX_KEYER_H

#include "clisocket.h"

class CWXKeyer
{
    public:
        CWXKeyer();      
        
        int init(const char* host, int port=4992);
        
        void setWPM(int wpm);
        
        int transmitCW( char* msg, unsigned int length );
        int transmitCW( const std::string& msg );
        
        int abortCW();
        
    private:
        ClientSocket m_cwx;
        int m_wpm;
        uint32_t m_seq;
        
        const std::string Seq(void);
};



#endif
