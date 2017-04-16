#include  <stdio.h>
#include  <limits.h>
#include  <stdlib.h>
#include  <errno.h>
#include  <string.h>
#include  <ctype.h>
#include  <time.h>

int
genwin(int min, int max)
{
    printf("\e[95mGenerating winning number...\e[39m\n");
    srand(time(0));
    return rand() % max + min;
}

int
alrdyguessed(size_t *count, int *guessed, int guess)
{
    if (*count == 0)
    {
        guessed[(*count)++] = guess;
        realloc(guessed, *count);
        return 0;
    }

    int indx;
    for (indx = 0; indx < *count; indx++) {
       if (guessed[indx] == guess) { return 1; }
    }

    guessed[(*count)++] = guess;
    realloc(guessed, sizeof(guessed[0]) * (*count));

    return 0;
}


void
flushbuf(char *buf)
{
    buf[strlen(buf) - 1] = 0;
}

int
getintin(char *msg, int *pn)
{
    char *buf = malloc(11);
    if (!buf)
    {
        perror("\e[91m");
        perror(errno ? strerror(errno) : "malloc");
        perror("\e[39m");
        exit(EXIT_FAILURE);
    }

    char *ts;

    int tn;

    do
    {
        printf("\e[95m\n%s\e[94m", msg);
        if (!fgets(buf, sizeof buf, stdin))
        {
            printf("\e[31m%s is not a valid number!\e[39m\n", buf);
            flushbuf(buf);

            continue;
        }

        flushbuf(buf);

        errno = 0;
        tn = strtol(buf, &ts, 10);
        if ((errno == ERANGE
                    && (tn == LONG_MAX || tn == LONG_MIN))
                || (errno && !tn))
        {
            perror("\e[91mstrtol\e[39m");
            exit(EXIT_FAILURE);
        }

        if (ts == buf && *ts != '\0') {
            printf("\e[31m'%s' is not a number!\e[39m\n", ts);
        }
    } while (ts != (buf + strlen(buf)));

    return ((*pn) = tn);
}

int
main(void)
{
    int max;
    int min;

    getintin("\e[95mChoose\e[31m min\e[95m number: ", &min);
    while (getintin("\e[95mChoose\e[32m max\e[95m number: ", &max) <= min) {
        if (max <= min) {
            printf("\e[32mMax(%d)\e[33m needs to be \e[32mhigher\e[33m than "
                      "\e[31mmin(%d)\e[39m\n",
                   max,
                   min);
        }
    }

    size_t sz = snprintf(0,
                         0lu,
                         "Guess a number between \e[31m%d\e[95m and "
                            "\e[32m%d\e[95m: ",
                         min,
                         max);
    if (!sz)
    {
        perror("\e[91msnprintf\e[39m");

        return EXIT_FAILURE;
    }

    char *msg = malloc(++sz);
    if (!msg)
    {
        perror("\e[91m");
        perror(errno ? strerror(errno) : "malloc");
        perror("\e[39m");
        free(msg);

        return EXIT_FAILURE;
    }

    if (!snprintf(msg,
                  sz,
                  "Guess a number between \e[31m%d\e[95m "
                      "and\e[32m %d\e[95m: ",
                  min,
                  max))
    {
        perror("\e[91msnprintf\e[39m");
        free(msg);

        return EXIT_FAILURE;
    }

    int win   = genwin(min, max);
    int tries = 0;
    int won   = 0;

    size_t count  = 0;
    int *guessed  = malloc(sizeof(int));
    if(!guessed)
    {
        perror("\e[91m");
        perror(errno ? strerror(errno) : "malloc");
        perror("\e[39m");
        free(msg);

        return EXIT_FAILURE;
    }

    int guess;
    do
    {
        getintin(msg, &guess);
        tries++;
        if (!alrdyguessed(&count, guessed, guess))
        {
            if (guess > max)
            {
                printf("\e[33mYour guess of \e[94m%d\e[33m is \e[32mgreater"
                          "\e[33m than the \e[32mmax(%d)\e[33m!\e[39m\n",
                       guess,
                       max);

                continue;
            }
            else if (guess < min)
            {
                printf("\e[33mYour guess of \e[94m%d\e[33m is \e[31mless"
                          "\e[33m than the \e[31mmin(%d)\e[33m!\e[39m\n",
                       guess,
                       min);

                continue;
            }

            printf("\e[33m%s\e[39m\n",
                   (guess > win
                     ? "To \e[32mhigh\e[33m!  Guess \e[31mlower\e[33m."
                     : (guess < win
                         ? "To \e[31mlow\e[33m!  Guess \e[32mhigher\e[33m."
                         : (won++, "\e[32mCorrect!  You win!"))));

            continue;
        }

        printf("\n\e[33mYou already guessed \e[94m%d\e[33m...\e[39m\n",
               guess);
    } while (!won);

    free(msg);

    return EXIT_SUCCESS;
}
