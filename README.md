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

