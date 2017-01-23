//============================================================================
// Name        : SocketServer.cpp
// Author      :
// Version     :
// Copyright   : Your copyright notice
// Description : Hello World in C++, Ansi-style
//============================================================================
/*
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

void error(const char *msg) {
  perror(msg);
  exit(1);
}

int main(int argc, char *argv[]) {
  int sockfd, newsockfd, portno;
  socklen_t clilen;
  char buffer[256];
  struct sockaddr_in serv_addr, cli_addr;
  int n;
  if (argc < 2) {
    fprintf(stderr, "ERROR, no port provided\n");
    exit(1);
  }
  sockfd = socket(AF_INET, SOCK_STREAM, 0);
  if (sockfd < 0) error("ERROR opening socket");
  bzero((char *)&serv_addr, sizeof(serv_addr));
  portno = atoi(argv[1]);
  serv_addr.sin_family = AF_INET;
  serv_addr.sin_addr.s_addr = INADDR_ANY;
  serv_addr.sin_port = htons(portno);
  if (bind(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
    error("ERROR on binding");
  listen(sockfd, 5);
  // while (true) {
  clilen = sizeof(cli_addr);
  newsockfd = accept(sockfd, (struct sockaddr *)&cli_addr, &clilen);
  if (newsockfd < 0) error("ERROR on accept");
  bzero(buffer, 256);
  n = read(newsockfd, buffer, 255);
  if (n < 0) error("ERROR reading from socket");
  printf("Here is the message: %s\n", buffer);
  n = write(newsockfd, "I got your message", 18);
  if (n < 0) error("ERROR writing to socket");
  // break;
  //}
  close(newsockfd);
  close(sockfd);
  return 0;
}
 */
/*
    C socket server example
*/

#include <arpa/inet.h>  //inet_addr
#include <stdio.h>
#include <string.h>  //strlen
#include <sys/socket.h>
#include <unistd.h>  //write
#include <iostream>
int main(int argc, char *argv[]) {
  int socket_desc, client_sock, c, read_size;
  struct sockaddr_in server, client;
  char client_message[4000];

  // Create socket
  socket_desc = socket(AF_INET, SOCK_STREAM, 0);
  if (socket_desc == -1) {
    printf("Could not create socket");
  }
  puts("Socket created");

  // Prepare the sockaddr_in structure
  server.sin_family = AF_INET;
  server.sin_addr.s_addr = INADDR_ANY;
  // server.sin_port = htons(3344);
  server.sin_port = htons(8765);
  // Bind
  if (bind(socket_desc, (struct sockaddr *)&server, sizeof(server)) < 0) {
    // print the error message
    perror("bind failed. Error");
    return 1;
  }
  puts("bind done");

  // Listen
  listen(socket_desc, 3);

  // Accept and incoming connection
  puts("Waiting for incoming connections...");
  c = sizeof(struct sockaddr_in);

  // accept connection from an incoming client
  client_sock =
      accept(socket_desc, (struct sockaddr *)&client, (socklen_t *)&c);
  if (client_sock < 0) {
    perror("accept failed");
    return 1;
  }
  puts("Connection accepted");
  int count = 0;
  char cm[4000];
  int total = 0;
  int readed = 0;
  char len[4];
  // Receive a message from client
  while ((read_size = recv(client_sock, client_message, 4000, 0)) > 0) {
    // Send the message back to client
    // write(client_sock, client_message, strlen(client_message));
    // char *s = strstr(client_message, "login");
    // std::cout << "s:" << *s << std::endl;
    // std::cout << read_size << "client_message:" << client_message <<
    // std::endl;
    //

    memcpy(len, client_message + 1, 4);
    int tmp = 0;
    for (int i = 0; i < 4; i++) {
      int j = (int)(len[i] & 0xff) << (i * 8);
      tmp += j;
    }
    /*
    printf("hex:%02x :%02x :%02x :%02x :%02x :%02x \r\n", client_message[0],
           client_message[1], client_message[2], client_message[3],
           client_message[4], client_message[5]);
    printf("hex:%02x :%02x :%02x :%02x  \r\n", len[0], len[1], len[2], len[3]);
    */
    if (tmp < 20480 && tmp > 0) total = tmp;
    std::cout << "tmp:" << tmp << "total:" << total << std::endl;

    if (tmp < 20480) {
      memcpy(cm, client_message + 5, read_size - 5);
      std::cout << "readed:" << readed << " read_size:" << read_size
                << std::endl;
      readed = read_size - 5;
    } else {
      memcpy(cm + readed, client_message, read_size);
      readed += read_size;
    }
    std::cout << "readed:" << readed << " total:" << total << std::endl;
    if (total != readed) continue;
    // memcpy(cm, client_message + 5, read_size - 5);
    // cm[read_size - 5] = '\0';
    std::cout << "cm:" << cm << std::endl;
    if (strstr(cm, "\"action\":\"login\"") != NULL) {
      char callback[1000] =
          "{\"result\": "
          "\"ok\"}";
      size_t length = strlen(callback);
      int l = static_cast<int>(length);
      char msg[l + 5];
      msg[0] = '1';
      msg[1] = (char)l & 0xFF;
      msg[2] = (char)l >> 8 & 0xFF;
      msg[3] = (char)l >> 16 & 0xFF;
      msg[4] = (char)l >> 24 & 0xFF;

      memcpy(msg + 5, callback, l + 1);
      std::cout << "count:" << callback << std::endl;
      c = write(client_sock, msg, l + 5);
      std::cout << "count:" << c << std::endl;
    } else if (strstr(cm, "\"action\":\"list\"") != NULL) {
      char callback[1000] =
          "{\"result\" : \"list_result\",\"list\" : [ {\"client_id\" : "
          "\"00001\"}, {\"client_id\" : \"00002\"} ],\"next_from\" : "
          "\"50\"}";
      size_t length = strlen(callback);
      int l = static_cast<int>(length);
      char msg[l + 5];
      msg[0] = '1';
      msg[1] = (char)l & 0xFF;
      msg[2] = (char)l >> 8 & 0xFF;
      msg[3] = (char)l >> 16 & 0xFF;
      msg[4] = (char)l >> 24 & 0xFF;

      memcpy(msg + 5, callback, l + 1);
      std::cout << "count:" << callback << std::endl;
      c = write(client_sock, msg, l + 5);
    } else if (strstr(cm, "\"action\":\"query\"") != NULL) {
      /*
    char callback[4000] =
        "{\"result\": \"actionlist\",\"actions\": [{\"action\": "
        "\"login\",\"client_id\": \"00003\"}, {\"action\": "
        "\"logout\",\"client_id\": \"00002\"}, {\"action\": "
        "\"videocall\",\"sdp\": {\"type\": \"offer\",\"sdp\": \"v=0o=- "
        "4527318532208535765 2 IN IP4 127.0.0.1s=-t=0 0a=group:BUNDLE "
        "sdparta_0 sdparta_1 sdparta_2a=msid-semantic: WMS "
        "this_is_local_streamm=audio 9 UDP/TLS/RTP/SAVPF 109 9 0 8c=IN IP4 "
        "0.0.0.0a=rtcp:9 IN IP4 "
        "0.0.0.0a=ice-ufrag:UDMya=ice-pwd:EloFjQnZJlgQjkUsTPKmBwroa="
        "fingerprint:sha-256 "
        "12:AA:9F:CA:E7:3D:4D:92:7D:CD:9E:CE:C6:C5:69:78:4D:0D:8D:03:CD:74:"
        "39:B7:A7:51:E0:19:2F:D4:E0:B8a=setup:activea=mid:sdparta_0a=extmap:"
        "1 "
        "urn:ietf:params:rtp-hdrext:ssrc-audio-levela=sendonlya=rtcp-muxa="
        "rtpmap:109 opus/48000/2a=fmtp:109 "
        "minptime=10;useinbandfec=1a=rtpmap:9 G722/8000a=rtpmap:0 "
        "PCMU/8000a=rtpmap:8 PCMA/8000a=ssrc:4286118465 "
        "cname:cVlV6VeQIqwnm/eua=ssrc:4286118465 msid:this_is_local_stream "
        "audio_labela=ssrc:4286118465 "
        "mslabel:this_is_local_streama=ssrc:4286118465 "
        "label:audio_labelm=video 9 UDP/TLS/RTP/SAVPF 120c=IN IP4 "
        "0.0.0.0a=rtcp:9 IN IP4 "
        "0.0.0.0a=ice-ufrag:UDMya=ice-pwd:EloFjQnZJlgQjkUsTPKmBwroa="
        "fingerprint:sha-256 "
        "12:AA:9F:CA:E7:3D:4D:92:7D:CD:9E:CE:C6:C5:69:78:4D:0D:8D:03:CD:74:"
        "39:B7:A7:51:E0:19:2F:D4:E0:B8a=setup:activea=mid:sdparta_1a="
        "sendonlya=rtcp-muxa=rtpmap:120 VP8/90000a=rtcp-fb:120 ccm "
        "fira=rtcp-fb:120 nacka=rtcp-fb:120 nack plia=rtcp-fb:120 "
        "goog-remba=ssrc:2350979377 cname:cVlV6VeQIqwnm/eua=ssrc:2350979377 "
        "msid:this_is_local_stream video_labela=ssrc:2350979377 "
        "mslabel:this_is_local_streama=ssrc:2350979377 "
        "label:video_labelm=application 9 DTLS/SCTP 5000c=IN IP4 "
        "0.0.0.0b=AS:30a=ice-ufrag:UDMya=ice-pwd:EloFjQnZJlgQjkUsTPKmBwroa="
        "fingerprint:sha-256 "
        "12:AA:9F:CA:E7:3D:4D:92:7D:CD:9E:CE:C6:C5:69:78:4D:0D:8D:03:CD:74:"
        "39:B7:A7:51:E0:19:2F:D4:E0:B8a=setup:activea=mid:sdparta_2a=sctpmap:"
        "5000 webrtc-datachannel 1024\"},\"from_client_id\": "
        "\"00001\"}]}\r\n\0";
    */
      if (count == 0) {
        char callback[4000] =
            "{\"result\": \"actionlist\",\"actions\": [{\"action\": "
            "\"login\",\"client_id\": "
            "\"00003\"},{\"action\": "
            "\"logout\",\"client_id\": "
            "\"00002\"}]}";

        c = write(client_sock, callback, strlen(callback));
        std::cout << "count:" << c << std::endl;
        count++;
      } else if (count == 1) {
        /*
      char callback[4000] =
          "{\"result\": \"actionlist\",\"actions\": [{\"action\": "
          "\"videocall\",\"sdp\": {\"type\": \"answer\",\"sdp\": \"v=0o=- "
          "4527318532208535765 2 IN IP4 127.0.0.1s=-t=0 0a=group:BUNDLE "
          "sdparta_0 sdparta_1 sdparta_2a=msid-semantic: WMS "
          "this_is_local_streamm=audio 9 UDP/TLS/RTP/SAVPF 109 9 0 8c=IN IP4 "
          "0.0.0.0a=rtcp:9 IN IP4 "
          "0.0.0.0a=ice-ufrag:UDMya=ice-pwd:EloFjQnZJlgQjkUsTPKmBwroa="
          "fingerprint:sha-256 "
          "12:AA:9F:CA:E7:3D:4D:92:7D:CD:9E:CE:C6:C5:69:78:4D:0D:8D:03:CD:74:"
          "39:B7:A7:51:E0:19:2F:D4:E0:B8a=setup:activea=mid:sdparta_0a="
          "extmap:"
          "1 "
          "urn:ietf:params:rtp-hdrext:ssrc-audio-levela=sendonlya=rtcp-muxa="
          "rtpmap:109 opus/48000/2a=fmtp:109 "
          "minptime=10;useinbandfec=1a=rtpmap:9 G722/8000a=rtpmap:0 "
          "PCMU/8000a=rtpmap:8 PCMA/8000a=ssrc:4286118465 "
          "cname:cVlV6VeQIqwnm/eua=ssrc:4286118465 msid:this_is_local_stream "
          "audio_labela=ssrc:4286118465 "
          "mslabel:this_is_local_streama=ssrc:4286118465 "
          "label:audio_labelm=video 9 UDP/TLS/RTP/SAVPF 120c=IN IP4 "
          "0.0.0.0a=rtcp:9 IN IP4 "
          "0.0.0.0a=ice-ufrag:UDMya=ice-pwd:EloFjQnZJlgQjkUsTPKmBwroa="
          "fingerprint:sha-256 "
          "12:AA:9F:CA:E7:3D:4D:92:7D:CD:9E:CE:C6:C5:69:78:4D:0D:8D:03:CD:74:"
          "39:B7:A7:51:E0:19:2F:D4:E0:B8a=setup:activea=mid:sdparta_1a="
          "sendonlya=rtcp-muxa=rtpmap:120 VP8/90000a=rtcp-fb:120 ccm "
          "fira=rtcp-fb:120 nacka=rtcp-fb:120 nack plia=rtcp-fb:120 "
          "goog-remba=ssrc:2350979377 "
          "cname:cVlV6VeQIqwnm/eua=ssrc:2350979377 "
          "msid:this_is_local_stream video_labela=ssrc:2350979377 "
          "mslabel:this_is_local_streama=ssrc:2350979377 "
          "label:video_labelm=application 9 DTLS/SCTP 5000c=IN IP4 "
          "0.0.0.0b=AS:30a=ice-ufrag:UDMya=ice-pwd:EloFjQnZJlgQjkUsTPKmBwroa="
          "fingerprint:sha-256 "
          "12:AA:9F:CA:E7:3D:4D:92:7D:CD:9E:CE:C6:C5:69:78:4D:0D:8D:03:CD:74:"
          "39:B7:A7:51:E0:19:2F:D4:E0:B8a=setup:activea=mid:sdparta_2a="
          "sctpmap:"
          "5000 webrtc-datachannel 1024\"},\"from_client_id\": "
          "\"00001\"}]}\r\n\0";
      */
        char callback[4000] =
            "{\"result\": \"actionlist\",\"actions\": [{\"action\": "
            "\"videocall\",\"sdp\": {\"type\": \"answer\",\"sdp\": \"v=0o=- "
            "6583077029174519167 2 IN IP4 127.0.0.1s=-t=0 0a=group:BUNDLE "
            "sdparta_0 sdparta_1 sdparta_2a=msid-semantic: WMS "
            "this_is_local_streamm=audio 9 UDP/TLS/RTP/SAVPF 109 9 0 8c=IN IP4 "
            "0.0.0.0a=rtcp:9 IN IP4 "
            "0.0.0.0a=ice-ufrag:m41Ga=ice-pwd:KkSTkhPnzuw4PaZ2C/"
            "W7QbJQa=fingerprint:sha-256 "
            "1F:69:CC:94:18:5E:9E:7D:C8:42:0D:30:B2:2F:B3:8B:61:DB:6C:5A:96:80:"
            "75:"
            "F3:0A:03:4B:6E:46:FA:B4:7Aa=setup:activea=mid:sdparta_0a=extmap:1 "
            "urn:ietf:params:rtp-hdrext:ssrc-audio-levela=sendonlya=rtcp-muxa="
            "rtpmap:109 opus/48000/2a=fmtp:109 "
            "minptime=10;useinbandfec=1a=rtpmap:9 G722/8000a=rtpmap:0 "
            "PCMU/8000a=rtpmap:8 PCMA/8000a=ssrc:923676356 "
            "cname:mQCv8TYkVw9YjJmma=ssrc:923676356 msid:this_is_local_stream "
            "audio_labela=ssrc:923676356 "
            "mslabel:this_is_local_streama=ssrc:923676356 "
            "label:audio_labelm=video "
            "9 UDP/TLS/RTP/SAVPF 120c=IN IP4 0.0.0.0a=rtcp:9 IN IP4 "
            "0.0.0.0a=ice-ufrag:m41Ga=ice-pwd:KkSTkhPnzuw4PaZ2C/"
            "W7QbJQa=fingerprint:sha-256 "
            "1F:69:CC:94:18:5E:9E:7D:C8:42:0D:30:B2:2F:B3:8B:61:DB:6C:5A:96:80:"
            "75:"
            "F3:0A:03:4B:6E:46:FA:B4:7Aa=setup:activea=mid:sdparta_1a="
            "sendonlya="
            "rtcp-muxa=rtpmap:120 VP8/90000a=rtcp-fb:120 ccm fira=rtcp-fb:120 "
            "nacka=rtcp-fb:120 nack plia=rtcp-fb:120 "
            "goog-remba=ssrc:3092270950 "
            "cname:mQCv8TYkVw9YjJmma=ssrc:3092270950 msid:this_is_local_stream "
            "video_labela=ssrc:3092270950 "
            "mslabel:this_is_local_streama=ssrc:3092270950 "
            "label:video_labelm=application 9 DTLS/SCTP 5000c=IN IP4 "
            "0.0.0.0b=AS:30a=ice-ufrag:m41Ga=ice-pwd:KkSTkhPnzuw4PaZ2C/"
            "W7QbJQa=fingerprint:sha-256 "
            "1F:69:CC:94:18:5E:9E:7D:C8:42:0D:30:B2:2F:B3:8B:61:DB:6C:5A:96:80:"
            "75:"
            "F3:0A:03:4B:6E:46:FA:B4:7Aa=setup:activea=mid:sdparta_2a=sctpmap:"
            "5000 "
            "webrtc-datachannel 1024\"},\"from_client_id\": "
            "\"00001\"}]}";
        size_t length = strlen(callback);
        int l = static_cast<int>(length);
        char msg[l + 5];
        msg[0] = '1';
        msg[1] = (char)l & 0xFF;
        msg[2] = (char)l >> 8 & 0xFF;
        msg[3] = (char)l >> 16 & 0xFF;
        msg[4] = (char)l >> 24 & 0xFF;

        memcpy(msg + 5, callback, l);
        std::cout << "count:" << callback << std::endl;
        c = write(client_sock, msg, l + 5);
        count++;
      } else if (count == 2) {
        char callback[4000] =
            "{\"result\": \"actionlist\",\"actions\": [{\"action\": "
            "\"videocall\",\"sdp\": {\"sdpMid\": \"audio\",\"candidate\": "
            "\"candidate:951005718 1 udp 2122260223 192.168.0.130 54302 typ "
            "host generation 0 ufrag 5F85safCaINHwQ5g\",\"sdpMLineIndex\": "
            "0},\"from_client_id \": \"00001 \"}]}";
        size_t length = strlen(callback);
        int l = static_cast<int>(length);
        char msg[l + 5];
        msg[0] = '1';
        msg[1] = (char)l & 0xFF;
        msg[2] = (char)l >> 8 & 0xFF;
        msg[3] = (char)l >> 16 & 0xFF;
        msg[4] = (char)l >> 24 & 0xFF;

        memcpy(msg + 5, callback, l);
        std::cout << "count:" << callback << std::endl;
        c = write(client_sock, msg, l + 5);
      }
    } else if (strstr(cm, "\"action\":\"videocall\"") != NULL) {
      std::cout << "client message:" << cm << std::endl;
      // c = write(client_sock, client_message, strlen(client_message));
      // std::cout << "count:" << c << std::endl;
    }
    total = 0;
    readed = 0;
  }

  if (read_size == 0) {
    puts("Client disconnected");
    fflush(stdout);
  } else if (read_size == -1) {
    perror("recv failed");
  }

  return 0;
}
