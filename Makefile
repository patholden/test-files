BUILDROOT_DIR = /home/patti/buildroot/board/ags/rootfs_overlay
INCLUDES = -I /usr/include/x86_64-linux-gnu -I /usr/include -I /home/patti/buildroot
CC=x86_64-linux-gnu-gcc
LDFLAGS = -static
LIBS = -lm
CFLAGS = -g -march=atom
TEST_OBJS = ramp.o ramp2.o tgfind_in.o step.o xy.o xytest.o
TEST_EXEC = ramp ramp2 tgfind_in step xy xytest
default:  $(TEST_EXEC)
all:  $(TEST_EXEC)
ramp: ramp.o
	$(CC) $(LDFLAGS) -o ramp ramp.o $(LIBS)
ramp.o: ramp.c
	$(CC) -c $(CFLAGS) $(INCLUDES) ramp.c

ramp2: ramp2.o
	$(CC) $(LDFLAGS) -o ramp2 ramp2.o $(LIBS)
ramp2.o: ramp2.c
	$(CC) -c $(CFLAGS) $(INCLUDES) $(LIBS) ramp2.c
step: step.o
	$(CC) $(LDFLAGS) -o step step.o $(LIBS)
step.o: step.c
	$(CC) -c $(CFLAGS) $(INCLUDES) $(LIBS) step.c
xy: xy.o
	$(CC) $(LDFLAGS) -o xy xy.o $(LIBS)
xy.o: xy.c
	$(CC) -c $(CFLAGS) $(INCLUDES) $(LIBS) xy.c
xytest: xytest.o
	$(CC) $(LDFLAGS) -o xytest xytest.o $(LIBS)
xytest.o: xytest.c
	$(CC) -c $(CFLAGS) $(LIBS) xytest.c
tgfind_in: tgfind_in.o
	$(CC) $(LDFLAGS) -o tgfind_in tgfind_in.o $(LIBS)
tgfind_in.o: tgfind_in.c
	$(CC) -c $(CFLAGS) -I$(INCLUDE_DIR) $(LIBS) tgfind_in.c
clean:
	rm *.o *~ $(TEST_EXEC)
install:
	chmod 755 $(TEST_EXEC); cp $(TEST_EXEC) $(BUILDROOT_DIR)

