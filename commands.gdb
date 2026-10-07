b start.c:58
c
b usertrapret
c
p cpus[$tp]->proc->name
b sys_exec
c
finish
p cpus[$tp]->proc->name
q
