
#include <slotted_udp.h>

#include <iostream>
#include <thread>

#include <getopt.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <sys/epoll.h>

#define CHANNEL_DEFAULT_ADDRESS "224.0.0.123"
#define CHANNEL_DEFAULT_PORT 49234

using namespace std;

void syncing( s_udp_channel_t* channel )
{
    while(true)
    {
        s_udp_wait_for_channel_ready(channel);
        usleep(100000);
    }
}

void recv_data(s_udp_channel_t* channel, int output_fd)
{
    uint8_t buffer[1024];
    ssize_t rd_len = 0;
    uint32_t latency = 0;
    uint8_t packet_loss_detected = 0;

    while(1) {
        s_udp_err_t res = S_UDP_OK;

        res = s_udp_receive_packet(channel,
                                   buffer,
                                   sizeof(buffer),
                                   &rd_len,
                                   &latency,
                                   &packet_loss_detected);

        // This may have been loopback read, or a
        // master packet that was internally processed.
        if (res == S_UDP_TRY_AGAIN)
            continue;

        if (rd_len == 0)
            break;

        if (res != S_UDP_OK) {
//            fprintf(stderr, "Packet receive failed: %s\n",
//                    s_udp_error_string(res));
//            // exit(255);
            continue;
        }

        if (output_fd != 1) {
            printf("t_id[%.9lu] lat[%.5u] len[%.4ld] p_loss[%c]\n",
                   channel->transaction_id,
                   latency,
                   rd_len,
                   packet_loss_detected?'Y':'N');

            write(output_fd, buffer, rd_len);
        }
        else {
            buffer[rd_len] = 0;
            printf("t_id[%.9lu] lat[%.5u] len[%.4ld] p_loss[%c]: %s%c",
                   channel->transaction_id,
                   latency,
                   rd_len,
                   packet_loss_detected?'Y':'N',
                   buffer,
                   (buffer[rd_len-1]=='\n')?0:'\n');
        }
    }
    return;
}

int main( int argc, char** argv )
{

    int slot = atoi( argv[1] );
    int opt;
    s_udp_channel_t channel;

    if (s_udp_init_channel(&channel,
                           0,
                           CHANNEL_DEFAULT_ADDRESS,
                           CHANNEL_DEFAULT_PORT,
                           slot) != S_UDP_OK)
        exit(255);

    if (s_udp_attach_channel(&channel) != S_UDP_OK)
    {
        cerr << "nope." << endl;
        exit(255);
    }

    std::thread sync_th = std::thread( syncing, &channel );
    sleep(1);

    recv_data(&channel, 1);

    return 0;
}
