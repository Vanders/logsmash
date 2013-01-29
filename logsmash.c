/* Copyright (c) 2013, Kristian Van Der Vliet <vanders@liqwyd.com>
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *     * Redistributions of source code must retain the above copyright
 *     notice, this list of conditions and the following disclaimer.
 *   * Redistributions in binary form must reproduce the above copyright
 *     notice, this list of conditions and the following disclaimer in the
 *     documentation and/or other materials provided with the distribution.
 *   * Neither the name of the <organization> nor the
 *     names of its contributors may be used to endorse or promote products
 *     derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL <COPYRIGHT HOLDER> BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <signal.h>
#include <syslog.h>

static int prio_s[] = {
    LOG_EMERG,
    LOG_ALERT,
    LOG_CRIT,
    LOG_ERR,
    LOG_WARNING,
    LOG_NOTICE,
    LOG_INFO,
    LOG_DEBUG
};

static const char *msg_s[] = {
    "printer on fire",
    "login for user 'root' on tty1, because they asked nicely.",
    "value of rand() exceeded lower universal limit.",
    "values of pi will rise to dom!",
    "you are not expected to understand this",
    "CPU temperature exceeded trigger (125C)",
    "this is not a real message"
};

static int run_s = 1;

void handle_sigterm(int signo){
  run_s = 0;
}

int get_prio_rand(void){
    int r;

    r = rand() % 8;
    return prio_s[r];
}

const char* get_message_rand(void){
    int r;

    r = rand() % 7;
    return msg_s[r];
}

int main(int argc, char* argv[]){
    int c;
    int std_output = 0;
    int count_output = 0;
    int usec, rate = 0;
    int quiet = 0;
    unsigned long n;
    struct sigaction sa;

    while( (c = getopt( argc, argv, "scl:q") ) != -1 ) {
        switch(c){
            case 's':
                std_output = 1;
                break;
            case 'c':
                count_output = 1;
                break;
            case 'l':
                rate = atoi(optarg);
                break;
            case 'q':
                quiet = 1;
                break;
            case '?':
                return EXIT_FAILURE;
        }
    }

    /* If rate limiting has been requested calculate the delay needed between
       each message*/
    if( rate > 0 ){
        usec = 1000000 / rate;
    } else {
        usec = 0;
    }

    /* Seed the PRNG and configure a signal handler so the user can stop
      cleanly */
    srand(time(NULL));

    sa.sa_handler = handle_sigterm;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = 0;

    sigaction(SIGINT, &sa, NULL);
    sigaction(SIGTERM, &sa, NULL);

    /* Open syslog and start sending messages */
    openlog("logsmash", LOG_CONS | LOG_PID | LOG_NDELAY, LOG_LOCAL1);

    for(n=0;;n++){
        int p;
        const char* m;

        p = get_prio_rand();
        m = get_message_rand();

        /* Inform the user of progress, if they've requested it. Note that you
           can use the -s or -c options, but not both.*/
        if(std_output)
            printf("%lu %s\n", n, m);
        else if(count_output){
            printf("\r%lu      ", n );
            fflush(stdout);
        }

        if(!quiet)
            syslog(LOG_MAKEPRI(LOG_LOCAL0,p), "%lu %s", n, m);

        if(rate)
            usleep(usec);

        /* Check for SIGTERM */
        if(!run_s){
          printf("\n");
          fflush(stdout);
          break;
        }
    }

    closelog();
    return EXIT_SUCCESS;
}

