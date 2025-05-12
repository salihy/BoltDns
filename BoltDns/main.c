//
//  main.c
//  BoltDns
//
//  Created by Salih YILDIRIM on 10.05.2025.
//

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <assert.h>
#include "main.h"

#define PORT 53
#define BUFFER_SIZE 1024

int main(int argc, const char * argv[]) {
    // insert code here...
    int sockfd;
    uint8_t buffer[BUFFER_SIZE];
    
    struct sockaddr_in server_addr, client_addr;
    sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    assert(sockfd > 0 && "Socket could not be created!");
    
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(PORT);
    
    if (bind(sockfd, (const struct sockaddr *) &server_addr, sizeof(server_addr)) < 0) {
        perror("bind failed");
        close(sockfd);
        exit(EXIT_FAILURE);
    }
    
    printf("UDP Echo Server listening on port %d...\n", PORT);
    
    while (1) {
        socklen_t len = sizeof(client_addr);
        ssize_t n = recvfrom(sockfd, buffer, BUFFER_SIZE, 0, (struct sockaddr *) &client_addr, &len);
        
        if (n < 0) {
            perror("recvfrom failed");
            continue;
        }
        
        //buffer[n] = '\0';
        
        DNSPackage *pack = parse_dns_query(buffer, n);
        
        printf("received size: %zd\n", n);
        uint8_t response[BUFFER_SIZE];
        
        create_response(pack, buffer, response, "10.0.0.11");
        
        sendto(sockfd, response, sizeof(response), 0, (struct sockaddr *) &client_addr, len);
        
        free(pack->questions[0]);
        free(pack->questions);
        free(pack);
        
    }
    
    close(sockfd);
    
    return 0;
}
