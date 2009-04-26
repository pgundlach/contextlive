
obj=output.cgi runtexexec runtexexec4mkiv result.cgi
src=output.c runtexexec.c runtexexec4mkiv.c result.c 
ifeq ($(HOME), /Users/pg)
  HOST=-DSCHNEE
else
  HOST=-DVLINUX
endif

ADDCFLAGS=-Wall -I/opt/local/include $(HOST)
ADDLDFLAGS=-L/opt/local/lib
all: $(obj)

%.cgi : %.c
	gcc $(CFLAGS) $(ADDCFLAGS)  -c $< 
	gcc $*.o $(ADDLDFLAGS) -lcgic -o $@ -s

runtexexec4mkiv: runtexexec4mkiv.c
	gcc $(CFLAGS) $(ADDCFLAGS) runtexexec4mkiv.c -Wall -o runtexexec4mkiv 

runtexexec: runtexexec.c
	gcc $(CFLAGS) $(ADDCFLAGS) runtexexec.c -Wall -o runtexexec 

clean:
	-rm -f $(obj) output.o result.o
