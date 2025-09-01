#define MAX_URLS 1000
#define MAX_URL_LEN 10001

static char* long_url[MAX_URLS];
static char* short_url[MAX_URLS];
static int url_counts=0;

char* generateShortUrls(int id){
    char *shorturl = malloc(MAX_URL_LEN);
    sprintf(shorturl, "http://tinyurl.com/%d", id);
    return shorturl;
}
char* encode(char* longUrl) {
    if(url_counts>MAX_URLS) return NULL;
    long_url[url_counts] = strdup(longUrl);
    char *shorturl = generateShortUrls(url_counts);
    short_url[url_counts] = shorturl;
    return short_url[url_counts++];
}

/** Decodes a shortened URL to its original URL. */
char* decode(char* shortUrl) {
    for(int i=0; i<url_counts; i++){
        if(strcmp(short_url[i], shortUrl) == 0){
            return long_url[i];
        }
    }
    return NULL;
}
