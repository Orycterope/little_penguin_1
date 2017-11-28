TARGET= hello_world

KDIR = /lib/modules/$(shell uname -r)/build

obj-m += $(TARGET).o
$(TARGET)-y := module_main.o proc_file.o

all:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) modules

clean:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) clean

