#ifndef _UTIL_H_
#define _UTIL_H_

#include "sfslite/async.h"
#include <string>
#include "message_types.h"

#define MAX_BID 10000

#define kSecToNanosec 1e9
#define kSecToMicrosec 1e6
#define kSecToMillisec 1e3
#define kMillisecToNanosec 1e6
#define kMillisecToMicrosec 1e3
#define kMillisecToSec 1e-3
#define kMicrosecToNanosec 1e3
#define kMicrosecToMillisec 1e-3
#define kMicrosecToSec 1e-6

inline static void
check_ret(int fd, str msg)
{
  if (fd < 0)
    fatal << msg << " failed\n";
}


// Copied from StackOverflow ID: 771453
template <typename M, typename V>
void map_to_vector(const M &m, V &v) {
  for(typename M::const_iterator it = m.begin(); it != m.end(); ++it) {
    v.push_back( it->second );
  }
}

static void gen_random_string(char *s, const int len) {
    static const char alphanum[] =
        ",./$&?"
        "0123456789"
        "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
        "abcdefghijklmnopqrstuvwxyz";

    for (int i = 0; i < len; ++i) {
        s[i] = alphanum[rand() % (sizeof(alphanum) - 1)];
    }
    s[len] = 0;
}

// Sample ad tag from: https://wiki.appnexus.com/display/industry/Ad+Tags

static std::string g_sample_ad = 
"<div class=\"singleAd\" id=\"TopLeft\"> <SCRIPT type=\"text/javascript\" \
SRC=\"http://ad.doubleclick.net/adj/N2434.nytimes/B3939938.6;sz=184x90;cli\
ck=http://www.nytimes.com/adx/bin/adx_click.html?type=goto&opzn&page=homep\
age.nytimes.com/index.html&pos=TopLeft&camp=Starbucks_ViaTasteTest_1185711\
-nyt8&ad=184x90_ViaTasteTest_Left&sn2=ab8a95f5/87622a3f&snr=doubleclick&sn\
x=1256160317&sn1=c77483d8/3440a1aa&goto=;ord=2009.10.21.21.28.38?\"></SCRI\
PT><NOSCRIPT><A HREF=\"http://www.nytimes.com/adx/bin/adx_click.html?type=\
goto&opzn&page=homepage.nytimes.com/index.html&pos=TopLeft&sn2=ab8a95f5/87\
622a3f&sn1=d4d31d54/ba1efee3&camp=Starbucks_ViaTasteTest_1185711-nyt8&ad=1\
84x90_ViaTasteTest_Left&goto=http://ad.doubleclick.net/jump/N2434.nytimes/\
B3939938.6;sz=184x90;ord=2009.10.21.21.28.38?\" TARGET=\"_blank\"><IMG SRC\
=\"http://ad.doubleclick.net/ad/N2434.nytimes/B3939938.6;sz=184x90;ord=200\
9.10.21.21.28.38?\" BORDER=0 WIDTH=184 HEIGHT=90 ALT=\"Click Here\"></A></\
NOSCRIPT></div>";



static void
get_page_data(a_req *req)
{
  int len = rand() % 100;
  char buf[len];
  gen_random_string(buf, len);

  req->url = std::string(buf);
  req->width = rand() % 1000;
  req->height = rand() % 1000;
}

static void
get_user_data(a_req *req)
{
  int len = rand() % 100;
  char buf[len];
  gen_random_string(buf, len);

  req->cookie_version = rand();
  req->cookie_age_seconds = rand();
  req->cookie_id = rand();
  
  req->user_agent = std::string(buf);  
  req->geo_criteria_id = rand() % 20;
  req->postal_code = rand() % 99999;
  req->timezone_offset = rand() % 24;
}


#endif
