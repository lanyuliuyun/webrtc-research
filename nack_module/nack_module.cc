
#include <webrtc/modules/video_coding/nack_module.h>
using namespace webrtc;

#include <iostream>

#include <stdlib.h>
#include <time.h>
#include <unistd.h>

class NackTester : public NackSender, public KeyFrameRequestSender
{
public:

NackTester()
{
    return;
}

~NackTester()
{
    return;
}

virtual void SendNack(const std::vector<uint16_t>& sequence_numbers)
{
    std::cout << "send nack packet:";
    auto iter = sequence_numbers.begin();
    auto iter_end = sequence_numbers.end();
    for (; iter != iter_end; ++iter)
    {
        std::cout << " " << *iter;
    }
    std::cout << std::endl;

    return;
}

virtual void RequestKeyFrame(void)
{
    std::cout << "request keyframe" << std::endl;
    
    return;
}

};

int gen_packet(VCMPacket *packet)
{
    static int sn = 0;
    
    sn++;
    
    if ((rand() % 100) > 85)
    {
        std::cout << "packet lost: " << sn << std::endl;
        return -1;
    }
    
    packet->seqNum = sn;
    
    return 0;
}

int main(int argc, char *argv[])
{
    NackTester *tester;
    NackModule *nack_module;
    
    srand((unsigned)time(NULL));
    
    tester = new NackTester();
    nack_module = new webrtc::NackModule(Clock::GetRealTimeClock(), tester, tester);
    
    for (;;)
    {
        int rand_count = (rand() % 100) + 30;
        while (rand_count-- > 0)
        {
            VCMPacket packet;
            if (gen_packet(&packet) == 0)
            {
                nack_module->OnReceivedPacket(packet);
            }

            if (nack_module->TimeUntilNextProcess() == 0)
            {
                nack_module->Process();
            }
        }

        usleep((30 + (rand() % 5))*1000);
    }
    
    delete nack_module;
    delete tester;
    
    return 0;
}