
#include <slotted_udp.h>

#include <iostream>
#include <sstream>
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

void send_data(s_udp_channel_t* channel, std::string msg )
{
    uint8_t buffer[1024];
    ssize_t rd_len = 0;
    struct epoll_event ev;
    int epoll_des;
    int nfds =0;
    int32_t send_wait = -1;

    epoll_des = epoll_create1(0);
    if (epoll_des == -1) {
        perror("epoll_create1");
        exit(255);
    }

    ev.events = EPOLLIN;
    ev.data.fd = channel->socket_des;

    if (epoll_ctl(epoll_des, EPOLL_CTL_ADD, channel->socket_des, &ev) == -1) {
        perror("epoll_ctl: channel desc");
        exit(EXIT_FAILURE);
    }

    int cpt = 0;

    // We need to read packet data from the channel to process
    // slot 0 pacekts sent by the master.
    while(1) {
        uint64_t tmp_wait = 0;
        ssize_t length = 0;
        uint32_t latency = 0;
        uint8_t packet_loss_detected = 0;

        stringstream sstr;
        sstr << msg << "_" << cpt;

                //rd_len = read(input_fd, buffer, sizeof(buffer));
        rd_len = sstr.str().size();

        if (rd_len == 0) {
            puts("Done reading");
            break;
        }

        // Do we have data to transmit and are waiting
        // for the send window to come into play?
//        printf("rd_len = %ld\n", rd_len);

        s_udp_get_sleep_duration(channel, &tmp_wait);

        send_wait = (int32_t) (tmp_wait / 1000) + 1;
        printf("Will wait %d msec. master_clock[%lu]\n",
               send_wait, s_udp_get_master_clock(channel));

//        nfds = epoll_wait(epoll_des, &ev, 1, send_wait);
//        if (nfds == -1) {
//            perror("epoll_wait");
//            exit(EXIT_FAILURE);
//        }

        printf("Sending %ld bytes master_clock[%lu]\n", rd_len,
               s_udp_get_master_clock(channel));
        s_udp_wait_and_send_packet(channel, (const unsigned char*)(sstr.str().c_str()), sstr.str().size() );

//        if (nfds == 1) {
//            //printf("Will read packet\n");
//            s_udp_receive_packet(channel,
//                                 buffer,
//                                 sizeof(buffer),
//                                 &length,
//                                 &latency,
//                                 &packet_loss_detected);
//        }

        cpt++;

    }
    return;
}

int main( int argc, char** argv )
{

    char is_sender = -1;
    int slot = atoi( argv[1] );
    std::string msg = argv[2];

    int opt;
    s_udp_channel_t channel;

    if (s_udp_init_channel(&channel,
                           is_sender,
                           CHANNEL_DEFAULT_ADDRESS,
                           CHANNEL_DEFAULT_PORT,
                           slot) != S_UDP_OK)
            exit(255);

    if (s_udp_attach_channel(&channel) != S_UDP_OK)
        exit(255);

    std::thread sync_th = std::thread( syncing, &channel );
    sleep(2);

    send_data(&channel, msg);

	return 0;
}
