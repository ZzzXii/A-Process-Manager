CSC 360 
Assignment 1
Zexi Luo
V00929142

Instructions on how to use PMan:
1. execute "make" from terminal to compile PMan
2. execute "./PMan" from terminal to run PMan


PMan supports following commands:
1. bg <cmd>: start program <cmd> in the background
2. bglist: display a list of all the programs starting in the background
3. bgkill <pid>: send the TERM signal to the process with process id <pid> to terminate process <pid>
4. bgstop <pid>: send the STOP signal to the process with process id <pid> to stop process <pid>
5. bgstart <pid>: send the CONT signal to the process with process id <pid> to restart process <pid>
6. pstat <pid>: list comm, state, utime, stime, rss, voluntary_ctxt_switches and nonvoluntary_ctxt_switches of process <pid>
7. q: stop program and exit

