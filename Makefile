BUILDROOTDIR = $(HOME)/buildroot
TEST_DIR = $(BUILDROOTDIR)/board/agslaser/rootfs_overlay/debug
INCLUDES = -I /usr/include/x86_64-linux-gnu -I /usr/include -I $(HOME)/buildroot -I $(HOME)/ags/ags-source/linux_headers/include
CC=x86_64-linux-gnu-gcc
LDFLAGS = -static
LIBS = -lm
CFLAGS = -g -march=atom
TEST_EXEC = ramp ramp1 ramp2 ramp9 tgfind_in step xy xytest xy_io ttyloop1 ttyloop2 xycenter sinusoid1 lg_drv_test lg_drv_sine readio writeio tgfind2
TEST_OBJS = ramp.o ramp2.o ramp9.o tgfind_in.o step.o xy.o xytest.o xy_io.o ttyloop1.o ttyloop2.o xycenter.o sinusoid1.o lg_drv_test.o lg_drv_sine.o readio writeio tgfind2.o

default:  $(TEST_EXEC)
all:  $(TEST_EXEC)
tgfind2.o: tgfind2.c
	$(CC) -c $(CFLAGS) $(INCLUDES) tgfind2.c
tgfind2: tgfind2.o
	$(CC) $(LDFLAGS) -o tgfind2 tgfind2.o $(LIBS)
readio.o: readio.c
	$(CC) -c $(CFLAGS) $(INCLUDES) readio.c
readio: readio.o
	$(CC) $(LDFLAGS) -o readio readio.o $(LIBS)
writeio.o: writeio.c
	$(CC) -c $(CFLAGS) $(INCLUDES) writeio.c
writeio: writeio.o
	$(CC) $(LDFLAGS) -o writeio writeio.o $(LIBS)
lg_drv_sine.o: lg_drv_sine.c
	$(CC) -c $(CFLAGS) $(INCLUDES) lg_drv_sine.c
lg_drv_sine: lg_drv_sine.o
	$(CC) $(LDFLAGS) -o lg_drv_sine lg_drv_sine.o $(LIBS)
lg_drv_test.o: lg_drv_test.c
	$(CC) -c $(CFLAGS) $(INCLUDES) lg_drv_test.c
lg_drv_test: lg_drv_test.o
	$(CC) $(LDFLAGS) -o lg_drv_test lg_drv_test.o $(LIBS)
ramp: ramp.o
	$(CC) $(LDFLAGS) -o ramp ramp.o $(LIBS)
ramp.o: ramp.c
	$(CC) -c $(CFLAGS) $(INCLUDES) ramp.c
ramp1: ramp1.o
	$(CC) $(LDFLAGS) -o ramp1 ramp1.o $(LIBS)
ramp1.o: ramp1.c
	$(CC) -c $(CFLAGS) $(INCLUDES) ramp1.c
ramp2: ramp2.o
	$(CC) $(LDFLAGS) -o ramp2 ramp2.o $(LIBS)
ramp2.o: ramp2.c
	$(CC) -c $(CFLAGS) $(INCLUDES) $(LIBS) ramp2.c
ramp9: ramp9.o
	$(CC) $(LDFLAGS) -o ramp9 ramp9.o $(LIBS)
ramp9.o: ramp9.c
	$(CC) -c $(CFLAGS) $(INCLUDES) ramp9.c
sinusoid1: sinusoid1.o
	$(CC) $(LDFLAGS) -o sinusoid1 sinusoid1.o $(LIBS)
sinusoid1.o: sinusoid1.c
	$(CC) -c $(CFLAGS) $(INCLUDES) sinusoid1.c
step: step.o
	$(CC) $(LDFLAGS) -o step step.o $(LIBS)
step.o: step.c
	$(CC) -c $(CFLAGS) $(INCLUDES) $(LIBS) step.c
ttyloop1: ttyloop1.o
	$(CC) $(LDFLAGS) -o ttyloop1 ttyloop1.o $(LIBS)
ttyloop1.o: ttyloop1.c
	$(CC) -c $(CFLAGS) $(INCLUDES) $(LIBS) ttyloop1.c
ttyloop2: ttyloop2.o
	$(CC) $(LDFLAGS) -o ttyloop2 ttyloop2.o $(LIBS)
ttyloop2.o: ttyloop2.c
	$(CC) -c $(CFLAGS) $(INCLUDES) $(LIBS) ttyloop2.c
xy: xy.o
	$(CC) $(LDFLAGS) -o xy xy.o $(LIBS)
xy.o: xy.c
	$(CC) -c $(CFLAGS) $(INCLUDES) $(LIBS) xy.c
xy_io: xy_io.o
	$(CC) $(LDFLAGS) -o xy_io xy_io.o $(LIBS)
xy_io.o: xy_io.c
	$(CC) -c $(CFLAGS) $(INCLUDES) $(LIBS) xy_io.c
xytest: xytest.o
	$(CC) $(LDFLAGS) -o xytest xytest.o $(LIBS)
xytest.o: xytest.c
	$(CC) -c $(CFLAGS) $(LIBS) xytest.c
tgfind_in: tgfind_in.o
	$(CC) $(LDFLAGS) -o tgfind_in tgfind_in.o $(LIBS)
tgfind_in.o: tgfind_in.c
	$(CC) -c $(CFLAGS) -I$(INCLUDES) $(LIBS) tgfind_in.c
xycenter: xycenter.o
	$(CC) $(LDFLAGS) -o xycenter xycenter.o $(LIBS)
xycenter.o: xycenter.c
	$(CC) -c $(CFLAGS) -I$(INCLUDES) $(LIBS) xycenter.c
clean:
	rm *.o $(TEST_EXEC)
install:
	chmod 755 $(TEST_EXEC); cp $(TEST_EXEC) $(TEST_DIR)
burnusb:
	sudo umount /dev/sdb1
	sudo mount /dev/sdb1 /mnt/stick
	sudo mount -o loop,ro $(BUILDROOTDIR)/output/images/rootfs.ext2 $(BUILDROOTDIR)/output/ext2
	sudo cp -avrf $(BUILDROOTDIR)/output/ext2/* /mnt/stick
	sudo cp $(BUILDROOTDIR)/output/images/bzImage /mnt/stick
	sudo cp $(BUILDROOTDIR)/output/images/bzImage /mnt/stick/boot
	sudo cp $(HOME)/ags/ags-demo/rev1.0/extlinux.conf /mnt/stick
	sudo umount /dev/sdb1
	sudo umount $(BUILDROOTDIR)/output/ext2

