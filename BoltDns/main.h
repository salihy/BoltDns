//
//  main.h
//  BoltDns
//
//  Created by Salih YILDIRIM on 10.05.2025.
//

#ifndef main_h
#define main_h

#define MAX_DOMAIN_LEN 255

#pragma pack(push, 1)
typedef struct {
    uint16_t id;
    uint16_t flags;
    uint16_t qdcount;
    uint16_t ancount;
    uint16_t nscount;
    uint16_t arcount;
} DNSHeader;

typedef struct {
    uint16_t rcode  : 4;
    uint16_t nad    : 1;
    uint16_t ad     : 1;
    uint16_t z      : 1;
    uint16_t ra     : 1; //recursion available
    uint16_t rd     : 1; //recursion desired
    uint16_t tc     : 1; //truncated message
    uint16_t aa     : 1; //authoritative answer
    uint16_t opcode : 4;
    uint16_t qr     : 1; //query - response
} DNSHeaderFlags;

typedef struct {
    uint16_t type;       // Record type (e.g., 1 for A)
    uint16_t class;      // Class (usually 1 for IN)
    uint32_t ttl;        // Time to live
    uint16_t rdlength;   // Length of RDATA field
    uint32_t rdata;      // For A records, 4-byte IPv4 address
} DNSAnswer;

#pragma pack(pop)

typedef struct {
    char qname[MAX_DOMAIN_LEN];
    uint16_t qtype;
    uint16_t qclass;
} DNSQuestion;

typedef struct {
    DNSHeader *header;
    DNSHeaderFlags *flags;
    DNSQuestion **questions;
    
} DNSPackage;

void print_dns_pack(DNSPackage *);
int read_domain_name(const uint8_t *, const uint8_t *, char *);
uint16_t get_u16(const uint8_t *);

DNSPackage* parse_dns_query(uint8_t *buffer, ssize_t size) {
    
    DNSPackage *pack = malloc(sizeof(DNSPackage));
    DNSHeader *header = (DNSHeader*)buffer;
        
    uint16_t flag_val = ntohs(header->flags);
    char flag_arr[2];
    flag_arr[0] = (char)(flag_val & 0xFF);
    flag_arr[1] = (char)((flag_val >> 8) & 0xFF);
    DNSHeaderFlags *flags = (DNSHeaderFlags *)flag_arr;
    
    //pack->flags = malloc(sizeof(DNSHeaderFlags));
    //memcpy(pack->flags, flags, sizeof(DNSHeaderFlags));
    
    //pack->header = malloc(sizeof(DNSHeader));
    //memcpy(pack->header, header, sizeof(DNSHeader));
      
    pack->flags = flags;
    pack->header = header;
    
    int qdcount = ntohs(pack->header->qdcount);
    printf("TransactionId: 0x%x\n", ntohs(pack->header->id));
    printf("Questions: %d\n", qdcount);
    
    const uint8_t *ptr = buffer + sizeof(DNSHeader);
    
    pack->questions = malloc(sizeof(DNSQuestion) * qdcount);

    for (int i = 0; i < qdcount; i++) {
        char domain[MAX_DOMAIN_LEN];
        int name_len = read_domain_name(buffer, ptr, domain);
        
        if (name_len < 0) {
            printf("Failed to read domain\n");
            break;
        }
        
        ptr += name_len;
        
        uint16_t qtype = (ptr[0] << 8) | ptr[1];
        uint16_t qclass = (ptr[2] << 8) | ptr[3];
        
        DNSQuestion *q = malloc(sizeof(DNSQuestion));
        
        memcpy(q->qname, domain, name_len);
        q->qclass = qclass;
        q->qtype = qtype;
        
        pack->questions[i] = q;
        
    }
    
    print_dns_pack(pack);
    
    return pack;
}

void create_response(DNSPackage *pack, uint8_t *request, uint8_t *response, const char *ip_str) {

    int idx = 0;
    
    memcpy(response, pack->header, sizeof(DNSHeader));
    idx += sizeof(DNSHeader);
    
    int qname_len = (int)strlen((char *) &request[idx]) + 1;
    memcpy(response + idx, request + idx, sizeof(uint8_t) * qname_len + sizeof(uint16_t) * 2);
    
    idx += sizeof(uint8_t) * qname_len + sizeof(uint16_t) * 2;
    
    response[idx++] = 0xC0;
    response[idx++] = 0x0C;
    
    DNSAnswer *answer = malloc(sizeof(DNSAnswer));
    answer->type = htons(1); //A record
    answer->class = htons(1); //IN
    answer->ttl = htons(60);
    answer->rdlength = htons(4);
    inet_pton(AF_INET, ip_str, &answer->rdata);
    
    memcpy(response + idx, answer, sizeof(DNSAnswer));
    idx += sizeof(DNSAnswer);
}

void print_dns_pack(DNSPackage *pack) {
    printf("id: 0x%x\n",
           pack->header->id);
    
    for (int i = 0; i < ntohs(pack->header->qdcount); i++) {
        DNSQuestion *q = pack->questions[i];
        
        printf("qname: %s, qtype: %d, qclass: %d\n", q->qname, q->qtype, q->qclass);
    }
}

int read_domain_name(const uint8_t *buffer, const uint8_t *ptr, char *output){
    int len = 0;
    const uint8_t *start = ptr;
    while (*ptr != 0) {
        uint8_t label_len = *ptr++;
        if (len + label_len >= MAX_DOMAIN_LEN) {
            return -1;
        }
        
        for (int i = 0; i < label_len; i++) {
            output[len++] = *ptr++;
        }
        output[len++] = '.';
    }
    
    output[len - 1] = '\0';
    return (ptr - start + 1);
}

uint8_t *get_question_bytes(DNSQuestion *question) {
    char *dot_loc;
    char *qname = malloc(sizeof(char) * strlen(question->qname));
    memcpy(qname, question->qname, sizeof(char) * strlen(question->qname));
    
    uint8_t *qb = malloc(sizeof(qname) + sizeof(uint16_t) * 2 + 1);
    int ptr = 0;
    
    do {
        dot_loc = strchr(qname, '.');
        int len = 0;
        if (dot_loc == NULL) {
            len = strlen(qname);
            qb[ptr++] = htons(len);
            memcpy(qb + ptr, qname, len);
            ptr += len;
        }
        else {
            len = dot_loc - qname;
            qb[ptr++] = htons(len);
            memcpy(qb + ptr, qname, len);
            ptr += len;
            qname = dot_loc + 1;
        }
    } while (dot_loc != NULL);
    
    return NULL;
}

uint16_t get_u16(const uint8_t *buf) {
    return (buf[0] << 8) | buf[1];
}

#endif /* main_h */
