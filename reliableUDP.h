
#define SYN_MASK 0x01
#define ACK_MASK 0x02
#define RST_MASK 0x04

struct reliable_udp_header {
  int seq_num;
  int ack_num;
  short int window_size;
  char flags;
  char dummy;
};

