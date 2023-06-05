#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>

#define FRAME_HEADER_START 14 //number of bytes: MAC dest, MAC source, type
#define IPV4_HEADER_START 20 //number of bytes: IPv4 header without options

struct frame_s {
    uint8_t dest_mac[6];
    uint8_t source_mac[6];
    uint8_t type[2];
    uint8_t version_ihl;
    uint8_t data_a[9]; //can be dissected into components if needed
    uint8_t checksum[2];
    uint8_t source_ip[4];
    uint8_t dest_ip[4];
    uint8_t data_b[1484]; //ip options and rest of data
};

/*Combine 2 bytes into 1 hextet*/
uint32_t combine_bytes(uint8_t byte_1, uint8_t byte_2){

    uint32_t combined = 0;

    combined = combined | byte_1; 
    combined = combined << 8; //shift bits to append next byte
    combined = combined | byte_2;

    return combined;
}

/*Test checksum for IPv4 header*/
_Bool ipv4_checksum(struct frame_s *frame, uint8_t ip_header_length) {

    int err;
    uint8_t *packet_pointer = &(frame->version_ihl); //IPv4 header start
    
    uint32_t checksum_calculated = 0;
    uint32_t checksum = combine_bytes(frame->checksum[0], frame->checksum[1]);
    /*Take out checksum bytes*/
    uint8_t checksum_byte_1 = frame->checksum[0];
    uint8_t checksum_byte_2 = frame->checksum[1];
    frame->checksum[0] = 0;
    frame->checksum[1] = 0;

    /*Calculate checksum_calculated*/    
    uint8_t byte_1, byte_2;
    for(int i = 0; i < (ip_header_length * 2); i++){
        byte_1 = *packet_pointer;
        byte_2 = *(++packet_pointer);
        checksum_calculated += combine_bytes(byte_1, byte_2);
        ++packet_pointer;

        /*Add overflow bits*/
        if((checksum_calculated / 0x10000) != 0){
            checksum_calculated = 
            ((checksum_calculated % 0x10000) + 0x1);
        }
    }

    /*Put back checksum bytes*/
    frame->checksum[0] = checksum_byte_1;
    frame->checksum[1] = checksum_byte_2;
    
    /*Ones complement, remove bits created by negation*/
    checksum_calculated = (~checksum_calculated) % 0xffff0000;

    err = (checksum_calculated == checksum);
    return (err);
    
}

void test_header(struct frame_s *frame) {
    
    int err; 
    uint16_t nmemb;
    uint8_t ip_header_length;
    uint8_t ip_header_version;

    nmemb = FRAME_HEADER_START + IPV4_HEADER_START; //see #define
    err = fread(&(frame->dest_mac[0]), 1, nmemb, stdin);

    if(err != nmemb){

        fprintf(stderr, "%s", 
                "first fread() at test_header did not read properly\n");
        exit(EXIT_FAILURE);

    } 
    
    /*Test if frame type field is IPv4*/
    if(frame->type[0] != 0x08 || frame->type[1] != 0x00 ){

        fprintf(stderr, "%s", "header is not ipv4, not supported\n");
        exit(EXIT_FAILURE);

    } 
           
    ip_header_length = frame->version_ihl & 0x0F; 
    ip_header_version = ((frame->version_ihl & 0xF0) >> 4);

    /*Test if ip header version is IPv4. 
    Check if length in scope [5,15]*/
    if(ip_header_version != 4 || 
       ip_header_length < 5   ||
       ip_header_length > 15){
        
        fprintf(stderr, "%s", 
                "header is not ipv4 or length value is bad\n");
        exit(EXIT_FAILURE);

    }

    if(ip_header_length != 5){
        
        /*Read ip header options*/
        nmemb = (ip_header_length - 5) * 4; //times 4 bytes, see ihl definition
        err = fread(&frame->data_b[0], 1, nmemb, stdin);

        if(err != nmemb){

        fprintf(stderr, "%s", 
                "second fread() at test_header did not read properly\n");
        exit(EXIT_FAILURE);

        } 
     }
            
    /*Test if ip header checksum is correct*/
    err = ipv4_checksum(frame, ip_header_length);
    if(err != 1){
        fprintf(stderr, "%s", "ip header checksum fail\n");
        exit(EXIT_FAILURE);
    }

}

/*Basic stdout output to show the program works*/
void format_and_print(struct frame_s *frame){
    
    int i;

    printf("\n");
    printf("source MAC:       ");
    for (i = 0; i < 6; ++i){
        printf("%02x", frame->source_mac[i]);
        if(i < 5) printf("-");
    }
    printf("\n");

    printf("source IPv4:      ");
    for (i = 0; i < 4; ++i){
        printf("%d", frame->source_ip[i]);
        if(i < 3) printf(".");
    }
    printf("\n\n");

    printf("destination MAC:  ");
    for (i = 0; i < 6; ++i){
        printf("%02x", frame->dest_mac[i]);
        if(i < 5) printf("-");
    }
    printf("\n");

    printf("destination IPv4: ");
    for (i = 0; i < 4; ++i){
        printf("%d", frame->dest_ip[i]);
        if(i < 3) printf(".");
    }
    printf("\n\n");
}



int main(void) {

    struct frame_s *frame = malloc(sizeof(struct frame_s));

    test_header(frame);
    format_and_print(frame);

    free(frame);
    return 0;
}
