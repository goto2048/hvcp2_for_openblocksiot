all:hvc

hvc:hvc.c hvc_data.h
	cc -o hvc hvc.c
