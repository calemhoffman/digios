/*
 * tcpReceiver.cpp
 *
 * Multi-threaded C++ TCP receiver for HELIOS/DGS DAQ.
 * One thread per IOC — each thread is an independent clone of gtReceiver4
 * logic with thread-local buffers. No shared mutable state between threads.
 *
 * Usage:
 *   tcpReceiver <filename_base> <maxfilesize> <GEBID> <server1> [server2] ...
 *
 * Example (matching start_run.sh usage):
 *   tcpReceiver ARR01_run_001.gtd 2000000000 14 ioc1 ioc2 ioc3 ioc4
 *
 * Each thread writes its own board-ID-indexed files (e.g. _0105, _0106).
 * Since each VME digitizer board has a unique board_id, there is no
 * file contention between threads.
 *
 * Build:
 *   g++ -O2 -pthread -o tcpReceiver tcpReceiver.cpp
 *
 * Author: HELIOS AI (General HELIOS) — 2026-03-12
 * Based on gtReceiver4.c by C. Lionberger (LBL) / ANL DGS group.
 * No existing files were modified.
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <time.h>
#include <fcntl.h>
#include <errno.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>

#include <pthread.h>
#include <atomic>
#include <vector>
#include <string>

/* ------------------------------------------------------------------ */
/* Protocol constants — identical to gtReceiver4.h / psNet.h          */
/* ------------------------------------------------------------------ */
#define SERVER_PORT           9001
#define CLIENT_REQUEST_EVENTS 1
#define SERVER_NORMAL_RETURN  2
#define SERVER_SENDER_OFF     3
#define SERVER_SUMMARY        4
#define INSUFF_DATA           5

#define EOE         0xaaaaaaaa
#define HDRLENINTS  13
#define MAXBOARDID  100000
#define MAXNS       100000
#define DATA_MEM_SIZE 10000000

#define WRITEGTFORMAT 1

/* ------------------------------------------------------------------ */
/* Protocol structs — from psNet.h                                     */
/* ------------------------------------------------------------------ */
typedef struct {
    int type;
    int recLen;
    int status;
    int recs;
} evtServerRetStruct;

struct reqPacket {
    int type;
};

struct gebData {
    int type;
    int length;
    long long timestamp;
};
typedef struct gebData GEBDATA;

/* ------------------------------------------------------------------ */
/* Global state                                                         */
/* ------------------------------------------------------------------ */
static std::atomic<bool> g_running(true);  /* set false on SIGINT */
static int g_GEB_TYPE_DGS = 0;
static long long g_maxFileSize = 2000000000LL;
static char g_filenameBase[512] = {0};     /* e.g. ARR01_run_001.gtd */

/* Per-board output files — indexed by board_id.                       */
/* Each board_id is unique to one VME digitizer → one thread writes it */
/* No mutex needed: different threads never share the same board_id.   */
static int g_ofile[MAXBOARDID];

/* ------------------------------------------------------------------ */
/* Per-thread context                                                   */
/* ------------------------------------------------------------------ */
struct ThreadCtx {
    std::string server;   /* "ioc1", "ioc2", etc.  */
    int threadIdx;        /* 0-based index          */
};

/* ------------------------------------------------------------------ */
/* Signal handler — graceful shutdown                                   */
/* ------------------------------------------------------------------ */
static void signal_catcher(int sig)
{
    printf("\n[tcpReceiver] caught signal %d — shutting down all threads...\n", sig);
    fflush(stdout);
    g_running = false;
}

/* ------------------------------------------------------------------ */
/* Resolve hostname → dotted-decimal IP string                          */
/* ------------------------------------------------------------------ */
static bool resolve_host(const char *hostname, char *ipout, size_t iplen)
{
    struct hostent *hp = gethostbyname(hostname);
    if (!hp || hp->h_addrtype != AF_INET) {
        fprintf(stderr, "[tcpReceiver] cannot resolve host '%s'\n", hostname);
        return false;
    }
    inet_ntop(hp->h_addrtype, hp->h_addr, ipout, (socklen_t)iplen);
    return true;
}

/* ------------------------------------------------------------------ */
/* Connect to one IOC server, return socket fd or -1                   */
/* ------------------------------------------------------------------ */
static int connect_to_server(const char *ip)
{
    struct sockaddr_in addr;
    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_port   = htons(SERVER_PORT);
    addr.sin_addr.s_addr = inet_addr(ip);

    if (addr.sin_addr.s_addr == INADDR_NONE) {
        fprintf(stderr, "[tcpReceiver] bad IP: %s\n", ip);
        return -1;
    }

    int sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock < 0) {
        perror("[tcpReceiver] socket");
        return -1;
    }

    if (connect(sock, (struct sockaddr*)&addr, sizeof(addr)) < 0) {
        fprintf(stderr, "[tcpReceiver] connect to %s failed: %s\n", ip, strerror(errno));
        close(sock);
        return -1;
    }

    struct reqPacket req;
    req.type = htonl(CLIENT_REQUEST_EVENTS);
    if (write(sock, &req, sizeof(req)) < 0) {
        perror("[tcpReceiver] write request");
        close(sock);
        return -1;
    }
    return sock;
}

/* ------------------------------------------------------------------ */
/* Get one data buffer from the server.                                 */
/* Returns number of bytes read into datamem, or -1 on error/no-data.  */
/* ------------------------------------------------------------------ */
static int get_data(int &sock, const char *ip, char *datamem)
{
    /* (re)connect if needed */
    if (sock < 0) {
        sock = connect_to_server(ip);
        if (sock < 0) return -1;
    }

    evtServerRetStruct reply;
    int bytesret = 0;
    int numret;
    while (bytesret < (int)sizeof(reply)) {
        numret = read(sock, ((char*)&reply) + bytesret, sizeof(reply) - bytesret);
        if (numret <= 0) { close(sock); sock = -1; return -1; }
        bytesret += numret;
    }

    int temptype = ntohl(reply.type);

    if (temptype == SERVER_SUMMARY) {
        int recsize = ntohl(reply.recLen);
        /* ask for actual data */
        struct reqPacket req;
        req.type = htonl(CLIENT_REQUEST_EVENTS);
        if (write(sock, &req, sizeof(req)) < 0) {
            close(sock); sock = -1; return -1;
        }
        /* read the data */
        int numbytesleft = recsize;
        bytesret = 0;
        while (bytesret < numbytesleft) {
            numret = read(sock, datamem + bytesret, numbytesleft - bytesret);
            if (numret <= 0) { close(sock); sock = -1; return -1; }
            bytesret += numret;
        }
        return recsize;

    } else if (temptype == INSUFF_DATA) {
        struct reqPacket req;
        req.type = htonl(CLIENT_REQUEST_EVENTS);
        write(sock, &req, sizeof(req));
        return -1;

    } else {
        /* SERVER_SENDER_OFF or unknown */
        close(sock); sock = -1;
        return -1;
    }
}

/* ------------------------------------------------------------------ */
/* Write events to board-indexed files (mirrors gtReceiver4 logic)     */
/* No mutex needed — each board_id belongs to exactly one VME DIG.     */
/* ------------------------------------------------------------------ */
static int write_events(const char *inptr, int size2write,
                        long long &totbytesInFile, int &chunck,
                        long long &totbytes, const char *filenameBase)
{
    GEBDATA Geb;
    unsigned int *testpattern = (unsigned int*)inptr;
    int curpos = 0, nbp = 0;
    int nwritten = 0;

    /* skip leading EOE markers */
    while (*testpattern == EOE) {
        testpattern++;
        curpos += sizeof(unsigned int);
        nbp   += sizeof(unsigned int);
    }

    while (nbp < size2write) {
        int nbytes = 0;
        unsigned int hdr[HDRLENINTS];
        int i1;

        /* find end of event */
        while (*testpattern != EOE && nbp <= size2write) {
            i1 = nbytes / 4;
            if (i1 < HDRLENINTS) hdr[i1] = *testpattern;
            nbytes += sizeof(unsigned int);
            nbp    += sizeof(unsigned int);
            testpattern++;
        }

        if (nbytes == 0) break;

        /* build GEB header */
        Geb.type   = g_GEB_TYPE_DGS;
        Geb.length = nbytes;

        /* byte-swap header words and extract timestamp */
        for (int i = 0; i < HDRLENINTS; i++) {
            unsigned int t1 = (hdr[i] & 0x000000ff) << 24;
            unsigned int t2 = (hdr[i] & 0x0000ff00) << 8;
            unsigned int t3 = (hdr[i] & 0x00ff0000) >> 8;
            unsigned int t4 = (hdr[i] & 0xff000000) >> 24;
            hdr[i] = t1 + t2 + t3 + t4;
        }
        Geb.timestamp  = (unsigned long long)hdr[1];
        Geb.timestamp += ((unsigned long long)(hdr[2] & 0x0000ffff)) << 32;

        /* extract board_id and packet_len from header */
        unsigned int tmp = hdr[0];
        int board_id   = ((hdr[0] >>= 4) & 0xfff);
        int packet_len = ((tmp >>= 16) & 0x000007ff) * 4;

        if (nbytes < 56) {
            fprintf(stderr, "[tcpReceiver] event too short (%d bytes), skip\n", nbytes);
            goto skip_eoe;
        }
        if (abs(nbytes - packet_len) > 8) {
            fprintf(stderr, "[tcpReceiver] length mismatch nbytes=%d pktlen=%d, skip\n",
                    nbytes, packet_len);
            goto skip_eoe;
        }

        /* open file for this board if not already open */
        if (g_ofile[board_id] <= 0) {
            char fname[600];
            char cstr[16];
            strncpy(fname, filenameBase, sizeof(fname)-1);
            sprintf(cstr, "_%3.3i", chunck);
            strcat(fname, cstr);
            sprintf(cstr, "_%4.4i", board_id);
            strcat(fname, cstr);

            /* refuse to overwrite */
            int chk = open(fname, O_RDONLY, 0);
            if (chk != -1) {
                close(chk);
                fprintf(stderr, "[tcpReceiver] ERROR: file '%s' already exists! Aborting.\n", fname);
                exit(1);
            }

            g_ofile[board_id] = open(fname, O_WRONLY | O_CREAT, 0644);
            if (g_ofile[board_id] > 0)
                printf("[tcpReceiver] opened file %s\n", fname);
            else {
                fprintf(stderr, "[tcpReceiver] failed to open %s\n", fname);
                exit(1);
            }
        }

        /* write GEB header + payload */
        write(g_ofile[board_id], &Geb, sizeof(GEBDATA));
        write(g_ofile[board_id], inptr + curpos, Geb.length);
        nwritten += sizeof(GEBDATA) + Geb.length;

    skip_eoe:
        curpos += nbytes;
        /* skip EOE markers between events */
        while (*testpattern == EOE) {
            testpattern++;
            curpos += sizeof(unsigned int);
            nbp    += sizeof(unsigned int);
        }
    }

    totbytes        += nwritten;
    totbytesInFile  += nwritten;

    /* roll over to new file chunk if max size reached */
    if (totbytesInFile >= g_maxFileSize) {
        printf("[tcpReceiver] max file size reached (%.1f MB), rolling to chunk %d\n",
               (double)totbytesInFile / 1024 / 1024, chunck + 1);
        for (int i = 0; i < MAXBOARDID; i++) {
            if (g_ofile[i] > 0) {
                close(g_ofile[i]);
                g_ofile[i] = 0;
            }
        }
        chunck++;
        totbytesInFile = 0;
    }

    return nwritten;
}

/* ------------------------------------------------------------------ */
/* Per-thread receiver loop                                             */
/* ------------------------------------------------------------------ */
static void* thread_receiver(void *arg)
{
    ThreadCtx *ctx = (ThreadCtx*)arg;

    /* thread-local data buffer — no sharing between threads */
    char *datamem = (char*)malloc(DATA_MEM_SIZE);
    if (!datamem) {
        fprintf(stderr, "[tcpReceiver] thread %d: malloc failed\n", ctx->threadIdx);
        return NULL;
    }

    char ip[INET_ADDRSTRLEN];
    if (!resolve_host(ctx->server.c_str(), ip, sizeof(ip))) {
        free(datamem);
        return NULL;
    }

    printf("[tcpReceiver] thread %d: connecting to %s (%s)\n",
           ctx->threadIdx, ctx->server.c_str(), ip);

    int sock = -1;
    int ns   = 1;
    long long totbytes       = 0;
    long long totbytesInFile = 0;
    long long tnow, tthen    = 0;
    int chunck               = 0;

    while (g_running) {
        int nbytes = get_data(sock, ip, datamem);

        if (nbytes <= 0) {
            usleep(ns);
            ns = (ns << 1);
            if (ns > MAXNS) ns = MAXNS;

            if (ns >= MAXNS) {
                tnow = time(NULL);
                if ((tnow - tthen) >= 15) {
                    printf("[tcpReceiver] thread %d (%s): %.3f MB received, waiting for data...\n",
                           ctx->threadIdx, ctx->server.c_str(),
                           (double)totbytes / 1024 / 1024);
                    fflush(stdout);
                    tthen = tnow;
                }
            }
        } else {
            ns = 1;
            write_events(datamem, nbytes, totbytesInFile, chunck, totbytes, g_filenameBase);

            tnow = time(NULL);
            if ((tnow - tthen) >= 15) {
                printf("[tcpReceiver] thread %d (%s): %.3f MB received\n",
                       ctx->threadIdx, ctx->server.c_str(),
                       (double)totbytes / 1024 / 1024);
                fflush(stdout);
                tthen = tnow;
            }
        }
    }

    /* cleanup */
    if (sock >= 0) close(sock);
    free(datamem);
    printf("[tcpReceiver] thread %d (%s): stopped. Total: %.3f MB\n",
           ctx->threadIdx, ctx->server.c_str(), (double)totbytes / 1024 / 1024);
    return NULL;
}

/* ------------------------------------------------------------------ */
/* main                                                                 */
/* ------------------------------------------------------------------ */
int main(int argc, char **argv)
{
    printf("tcpReceiver — multi-threaded DGS data receiver\n");
    printf("Based on gtReceiver4.c (C. Lionberger, LBL / ANL)\n\n");

    if (argc < 5) {
        printf("Usage: %s <filename_base> <maxfilesize> <GEBID> <server1> [server2] ...\n", argv[0]);
        printf("  e.g: %s ARR01_run_001.gtd 2000000000 14 ioc1 ioc2 ioc3 ioc4\n", argv[0]);
        printf("\nGEBID: 14=DGS, 15=DGSTRIG, 16=DFMA\n");
        return 1;
    }

    strncpy(g_filenameBase, argv[1], sizeof(g_filenameBase) - 1);
    g_maxFileSize   = atoll(argv[2]);
    g_GEB_TYPE_DGS  = atoi(argv[3]);

    memset(g_ofile, 0, sizeof(g_ofile));

    /* collect server names */
    std::vector<std::string> servers;
    for (int i = 4; i < argc; i++)
        servers.push_back(argv[i]);

    int nthreads = (int)servers.size();
    printf("Starting %d receiver thread(s) for: ", nthreads);
    for (auto &s : servers) printf("%s ", s.c_str());
    printf("\nFilename base : %s\n", g_filenameBase);
    printf("Max file size : %lld bytes\n", g_maxFileSize);
    printf("GEB type ID   : %d\n\n", g_GEB_TYPE_DGS);
    fflush(stdout);

    signal(SIGINT,  signal_catcher);
    signal(SIGTERM, signal_catcher);

    /* launch threads */
    std::vector<pthread_t> threads(nthreads);
    std::vector<ThreadCtx> ctxs(nthreads);

    for (int i = 0; i < nthreads; i++) {
        ctxs[i].server    = servers[i];
        ctxs[i].threadIdx = i;
        if (pthread_create(&threads[i], NULL, thread_receiver, &ctxs[i]) != 0) {
            perror("pthread_create");
            return 1;
        }
    }

    /* wait for all threads */
    for (int i = 0; i < nthreads; i++)
        pthread_join(threads[i], NULL);

    /* close any still-open files */
    for (int i = 0; i < MAXBOARDID; i++)
        if (g_ofile[i] > 0) close(g_ofile[i]);

    printf("\n[tcpReceiver] all threads done. Exiting.\n");
    return 0;
}
