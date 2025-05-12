# BoltDns

**BoltDns** is a lightweight DNS server written in C. It listens for UDP DNS queries, parses the incoming DNS request, and sends back a hardcoded A record response.

---

## 📦 Features

- Parses DNS queries according to [RFC 1035](https://tools.ietf.org/html/rfc1035)
- Extracts DNS header, flags, and question section
- Constructs a DNS response with a static IPv4 address
- Uses low-level socket programming (UDP, port 53)
- Demonstrates DNS bitfield parsing with C structs

---

## 📁 Project Structure

BoltDns/  
├── main.c # Main UDP server loop  
├── main.h # DNS structs and parsing logic  
└── README.md # Project documentation  

---

## ⚙️ How It Works

1. Server binds to UDP port 53.
2. Receives and parses DNS query from a client.
3. Extracts header and question section.
4. Constructs and sends a valid DNS response:
   - Only A records (IPv4)
   - Response IP is hardcoded (e.g., `10.0.0.11`)

---

## 🧪 Sample Output

UDP Echo Server listening on port 53...  
TransactionId: 0xabcd  
Questions: 1  
qname: example.com, qtype: 1, qclass: 1  
received size: 42  


---

## 🧰 Build & Run

### Requirements

- GCC or Clang
- Unix-like OS (Linux/macOS)
- Root or sudo privileges (to bind port 53)

### Compile

```bash
gcc -o BoltDns main.c
sudo ./BoltDns
```

## 🧬 DNS Packet Format  

BoltDns handles these DNS sections:  

- DNSHeader – ID, flags, counts (packed struct)
- DNSHeaderFlags – uses bitfields for parsing flags
- DNSQuestion – domain name, type, and class
- DNSAnswer – A record with TTL and IP address
All fields are converted with ntohs() and htons() where appropriate to handle network-to-host byte order.

## ⚠️ Limitations  

- Only supports A (IPv4) record queries
- Only handles one question per query
- IP in response is static (10.0.0.11)
- No support for recursion, caching, or NS/AR sections

## 🔬 Testing
Use dig or nslookup:  

```bash
dig @127.0.0.1 example.com
```
Expected response: A record pointing to 10.0.0.11.  

## 📄 License
MIT License


