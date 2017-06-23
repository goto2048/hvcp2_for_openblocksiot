#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <termios.h>
#include <unistd.h>
#include <time.h>

#include <sys/socket.h>
#include <sys/un.h>

#include <errno.h>

#include "hvc_data.h"

#define SERIAL_PORT "/dev/ttyACM0"

int main(int argc, char *argv[])
{
    int fd;
    int ret;
    uint8_t sbuf[] = {0xFE, 0, 0, 0 };
    uint8_t cbuf[] = {0xFE, 0x01, 0x01, 0, 0 };
    uint8_t buf[] = {0xFE, 0x04, 0x03, 0x00, 0xff, 0x03, 0x00};
    uint8_t rbuf[1894];
    int8_t jbuf[4000];
    int i;
    socklen_t socklen;

    struct termios oldtio, newtio;
    struct sockaddr_un addr;

    fd = open(SERIAL_PORT, O_RDWR | O_NDELAY | O_NOCTTY);

    tcgetattr( fd, &oldtio );
    newtio = oldtio;

    newtio.c_cflag = B9600 | CS8 | CLOCAL | CREAD;
    newtio.c_iflag = 0;
    newtio.c_lflag = 0;
    newtio.c_oflag = 0;
    newtio.c_cc[VTIME] = 0;
    newtio.c_cc[VMIN] = 0;

    tcflush(fd, TCIOFLUSH);
    tcsetattr(fd, TCSANOW, &newtio);

/* version */
    write(fd, sbuf, 4);
    sleep(1);
    ret = read(fd, rbuf, sizeof(rbuf));

    printf("ret=%d\n", ret);
    printf("rbuf[]: ");
    for (i = 0; i < ret; i++) {
        printf("0x%02x, ",rbuf[i]);
    }
    printf("\n");

/* camera setting */
    write(fd, cbuf, 5);
    sleep(1);
    ret = read(fd, rbuf, sizeof(rbuf));

    printf("ret=%d\n", ret);
    printf("rbuf[]: ");
    for (i = 0; i < ret; i++) {
        printf("0x%02x, ",rbuf[i]);
    }
    printf("\n");

    while(1) {
	write(fd, buf, sizeof(buf));
	sleep(2);
	ret = read(fd, rbuf, sizeof(rbuf));

	printf("ret=%d\n", ret);
	printf("rbuf[]: ");
	for (i = 0; i < ret; i++) {
		printf("0x%02x, ",rbuf[i]);
	}
	printf("\n");
	hvc_data2json( rbuf, jbuf );
	ret = write_uds( jbuf );
	if ( ret > 0 ) break;
	sleep(5);
    }

    tcsetattr(fd, TCSANOW, &oldtio);
    close(fd);

    return 0;
} 

hvc_data2json( uint8_t *rbuf, int8_t *jbuf ) {

    int i,j;
    int8_t str[81];
    struct tm *tm;
    time_t timer;
    int8_t sbuf[30];

    timer = time(NULL);
    tm = localtime(&timer);
    strftime(str,sizeof(str),"%Y-%m-%dT%H:%I:%S",tm);

    hvc_header.body = *(rbuf+6);
    hvc_header.hand = *(rbuf+7);
    hvc_header.face = *(rbuf+8);
    sprintf( jbuf, "{\"time\":\"%s\",\"body\":%d,\"hand\":%d,\"face\":%d,", str, hvc_header.body, hvc_header.hand, hvc_header.face );

/* parameter of body */
    j=1;
    rbuf+=10;
    for ( i = hvc_header.body; i; i--,j++ ) {
	sprintf( jbuf, "%s\"body%d_x\":%d,", jbuf, j,(uint16_t)*(rbuf)+((uint16_t)*(rbuf+1) << 8 ) );
	sprintf( jbuf, "%s\"body%d_y\":%d,", jbuf, j,(uint16_t)*(rbuf+2)+((uint16_t)*(rbuf+3) << 8 ) );
	sprintf( jbuf, "%s\"body%d_size\":%d,", jbuf, j,(uint16_t)*(rbuf+4)+((uint16_t)*(rbuf+5) << 8 ) );
	sprintf( jbuf, "%s\"body%d_reliability\":%d,", jbuf, j,(uint16_t)*(rbuf+6)+((uint16_t)*(rbuf+7) << 8 ) );
	rbuf+=8;
    }


/* parameter of hand */
    j=1;
    for ( i = hvc_header.hand; i; i--,j++ ) {
	sprintf( jbuf, "%s\"hand%d_x\":%d,", jbuf, j,(uint16_t)*(rbuf)+((uint16_t)*(rbuf+1) << 8 ) );
	sprintf( jbuf, "%s\"hand%d_y\":%d,", jbuf, j,(uint16_t)*(rbuf+2)+((uint16_t)*(rbuf+3) << 8 ) );
	sprintf( jbuf, "%s\"hand%d_size\":%d,", jbuf, j,(uint16_t)*(rbuf+4)+((uint16_t)*(rbuf+5) << 8 ) );
	sprintf( jbuf, "%s\"hand%d_reliability\":%d,", jbuf, j,(uint16_t)*(rbuf+6)+((uint16_t)*(rbuf+7) << 8 ) );
	rbuf+=8;
    }

/* parameter of face */
    j=1;
    for ( i = hvc_header.face; i; i--,j++ ) {
	sprintf( jbuf, "%s\"face%d_x\":%d,", jbuf, j,(uint16_t)*(rbuf)+((uint16_t)*(rbuf+1) << 8 ) );
	sprintf( jbuf, "%s\"face%d_y\":%d,", jbuf, j,(uint16_t)*(rbuf+2)+((uint16_t)*(rbuf+3) << 8 ) );
	sprintf( jbuf, "%s\"face%d_size\":%d,", jbuf, j,(uint16_t)*(rbuf+4)+((uint16_t)*(rbuf+5) << 8 ) );
	sprintf( jbuf, "%s\"face%d_rel_face\":%d,", jbuf, j,(uint16_t)*(rbuf+6)+((uint16_t)*(rbuf+7) << 8 ) );
	sprintf( jbuf, "%s\"face%d_h_angle\":%d,", jbuf, j,(uint16_t)*(rbuf+8)+((uint16_t)*(rbuf+9) << 8 ) );
	sprintf( jbuf, "%s\"face%d_v_angle\":%d,", jbuf, j,(uint16_t)*(rbuf+10)+((uint16_t)*(rbuf+11) << 8 ) );
	sprintf( jbuf, "%s\"face%d_f_angle\":%d,", jbuf, j,(uint16_t)*(rbuf+12)+((uint16_t)*(rbuf+13) << 8 ) );
	sprintf( jbuf, "%s\"face%d_rel_angle\":%d,", jbuf, j,(uint16_t)*(rbuf+14)+((uint16_t)*(rbuf+15) << 8 ) );
	sprintf( jbuf, "%s\"face%d_age\":%d,", jbuf, j, *(rbuf+16) );
	sprintf( jbuf, "%s\"face%d_rel_age\":%d,", jbuf, j,(uint16_t)*(rbuf+17)+((uint16_t)*(rbuf+18) << 8 ) );
	sprintf( jbuf, "%s\"face%d_sex\":%d,", jbuf, j, *(rbuf+19) );
	sprintf( jbuf, "%s\"face%d_rel_sex\":%d,", jbuf, j,(uint16_t)*(rbuf+20)+((uint16_t)*(rbuf+21) << 8 ) );
	sprintf( jbuf, "%s\"face%d_h_sight\":%d,", jbuf, j, (int8_t)*(rbuf+22) );
	sprintf( jbuf, "%s\"face%d_v_sight\":%d,", jbuf, j, (int8_t)*(rbuf+23) );
	sprintf( jbuf, "%s\"face%d_left_eye\":%d,", jbuf, j,(uint16_t)*(rbuf+24)+((uint16_t)*(rbuf+25) << 8 ) );
	sprintf( jbuf, "%s\"face%d_right_eye\":%d,", jbuf, j,(uint16_t)*(rbuf+26)+((uint16_t)*(rbuf+27) << 8 ) );
	sprintf( jbuf, "%s\"face%d_exp_less\":%d,", jbuf, j, *(rbuf+28) );
	sprintf( jbuf, "%s\"face%d_exp_joy\":%d,", jbuf, j, *(rbuf+29) );
	sprintf( jbuf, "%s\"face%d_exp_surprise\":%d,", jbuf, j, *(rbuf+30) );
	sprintf( jbuf, "%s\"face%d_exp_anger\":%d,", jbuf, j, *(rbuf+31) );
	sprintf( jbuf, "%s\"face%d_exp_sad\":%d,", jbuf, j, *(rbuf+32) );
	sprintf( jbuf, "%s\"face%d_exp_positive\":%d,", jbuf, j, *(rbuf+33) );
	sprintf( jbuf, "%s\"face%d_userid\":%d,", jbuf, j,(uint16_t)*(rbuf+34)+((uint16_t)*(rbuf+35) << 8 ) );
	sprintf( jbuf, "%s\"face%d_score\":%d,", jbuf, j,(uint16_t)*(rbuf+36)+((uint16_t)*(rbuf+37) << 8 ) );
	rbuf+=38;
    }

/* change last charcter */
    *(jbuf+strlen(jbuf)) = 0;
    *(jbuf+strlen(jbuf)-1) = '}';
    printf( "strlen=%d\n", strlen(jbuf) );
    
    printf( jbuf );
    return 0;
}

/* write json data to unix domain socket */
write_uds( uint8_t *jbuf ) {
    int uds, ret;
    socklen_t socklen;
    struct sockaddr_un addr;

    uds = socket(AF_UNIX, SOCK_STREAM, 0);
    memset(&addr, 0, sizeof(addr));
    addr.sun_family = AF_UNIX;
    strcpy(addr.sun_path+1, "/pd_emitter_lite/device_user_0000001.sock");
    socklen =  sizeof(sa_family_t) + strlen(addr.sun_path+1)+1;
    ret = connect(uds, (struct sockaddr *)&addr, socklen);

    if (ret < 0) {
        int err = errno;
        printf("connect NG (errno: %d)\n", err);
	close(uds);
	return 1;
    }
    else {
        printf("connect OK\n");
    }
    write(uds, jbuf, strlen(jbuf));
    
    close( uds );
    return 0;
}
