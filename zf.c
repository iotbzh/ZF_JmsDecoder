/*
 * Copyright 2019 IoT.BzH
 *
 * Licensed under the Apache License, Version 2.0 (the "License").
 * You may not use this file except in compliance with the License.
 * A copy of the License is located at
 *
 *   http://www.apache.org/licenses/LICENSE-2.0
 *
 * or in the "license" file accompanying this file. This file is distributed
 * on an "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either
 * express or implied. See the License for the specific language governing
 * permissions and limitations under the License.
 */

#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <net/if.h>
#include <linux/can.h>
#include <linux/can/raw.h>

#include <stdbool.h>
#include <unistd.h>

int soc;
int read_can_port;

#define DEAD_ZONE_BIT 0x2


static int open_port(const char *port)
{
	struct ifreq ifr;
	struct sockaddr_can addr;

	/* open socket */
	soc = socket(PF_CAN, SOCK_RAW, CAN_RAW);
	if(soc < 0)
	{
		return (-1);
	}

	addr.can_family = AF_CAN;
	strcpy(ifr.ifr_name, port);

	if (ioctl(soc, SIOCGIFINDEX, &ifr) < 0)
	{

		return (-1);
	}

	addr.can_ifindex = ifr.ifr_ifindex;

	fcntl(soc, F_SETFL, O_NONBLOCK);

	if (bind(soc, (struct sockaddr *)&addr, sizeof(addr)) < 0)
	{

		return (-1);
	}

	return 0;
}

static int send_port(struct can_frame *frame)
{
	int retval;
	retval = write(soc, frame, sizeof(struct can_frame));
	if (retval != sizeof(struct can_frame))
	{
		return (-1);
	}
	else
	{
		return (0);
	}
}


static void read_port()
{
	struct can_frame frame_rd;
	int recvbytes = 0;

	read_can_port = 1;
	while(read_can_port)
	{
		struct timeval timeout = {1, 0};
		fd_set readSet;
		FD_ZERO(&readSet);
		FD_SET(soc, &readSet);

		if (select((soc + 1), &readSet, NULL, NULL, &timeout) >= 0)
		{
			if (!read_can_port)
			{
				break;
			}
			if (FD_ISSET(soc, &readSet))
			{
				recvbytes = read(soc, &frame_rd, sizeof(struct can_frame));
				if (!recvbytes)
					continue;

				short x, y, z;

				y = frame_rd.data[1] |
					frame_rd.data[0] << 8;

				x =  frame_rd.data[3] |
					 frame_rd.data[2] << 8;

				z =  frame_rd.data[5] |
					 frame_rd.data[4] << 8;

				bool dead;
				dead = (frame_rd.data[6] & DEAD_ZONE_BIT) == DEAD_ZONE_BIT;

				printf("X: %d Y: %d Z: %d dead zone: %d\n", x, y, z, dead);

			}
		}
	}

}


int close_port()
{
	close(soc);
	return 0;
}

int main(void)
{
	open_port("can0");
	read_port();
	return 0;
}
