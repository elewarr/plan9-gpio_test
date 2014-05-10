#include <u.h>
#include <libc.h>

int setup(void);

void
main(void)
{
	int eventfd, pinfd, i, reads = 0, state;
	u32int event;
	char pin;

	if(setup() != 0)
	{
		exits("ctlfd");
	}

	eventfd = open("#G/gpio/event", OREAD);
	if(!eventfd)
	{
		exits("eventfd");
	}

	pinfd = open("#G/gpio/GPIO0", OREAD);
	if(!pinfd)
	{
		close(eventfd);
		exits("pinfd");
	}

	while(++reads < 100)
	{
		read(eventfd, &event, sizeof(u32int));

		for(i = 0; i < sizeof(u32int) * 8; i++)
		{
			state = event & (1 << i);
			if((state >> i) & 1)
			{
				print("%d: pin %d, state changed\n", reads, i);
				if(i == 17) {
					read(pinfd, &pin, 1);
					print("\tGPIO0 state=%c\n", pin);
				}
			}
			
		}
	}
	close(eventfd);
	close(pinfd);
	
	exits(0);
}

int setup(void)
{
	int ctlfd;
	char boardscheme[] = "scheme board";
	char funcin[] = "function in GPIO0";
	char edgerising[] = "event edge-rising enable GPIO0";
	char edgefalling[] = "event edge-falling enable GPIO0";
	
	ctlfd = open("#G/gpio/ctl", OWRITE);
	if(!ctlfd)
	{
		return -1;
	}

	write(ctlfd, boardscheme, strlen(boardscheme));
	write(ctlfd, funcin, strlen(funcin));
	write(ctlfd, edgerising, strlen(edgerising));
	write(ctlfd, edgefalling, strlen(edgefalling));
	
	close(ctlfd);

	return 0;
}