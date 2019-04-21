#include <cstdio>
#include <cstdlib>
#include <exception>
#include <iostream>

#include <tgbot/tgbot.h>
#include "app_signal.hpp"

using namespace std;
using namespace TgBot;

long BLOCK_MIN = 0;
long BLOCK_MAX = 0;

long blocktime_ms() {
  return rand() % BLOCK_MAX + BLOCK_MIN;
}

auto nameOf = [](Chat::Type t) {
    switch (t) {
        case TgBot::Chat::Type::Supergroup: return string("Super Group");
        case TgBot::Chat::Type::Group: return string("Group");
        case TgBot::Chat::Type::Private: return string("Private Chat");
        case TgBot::Chat::Type::Channel: return string("Channel");
    }
    return string("Unknown");
};

void application_run(EventBroadcaster &observe, const Api &query) {
  auto handleSucide = [&query](Message::Ptr msg) {
    cerr << "onSuicide: " << msg->chat->id << ":" << msg->messageId << endl;

    long blocked = blocktime_ms();
    time_t ctime = time(nullptr);
    bool success = query.restrictChatMember(msg->chat->id, msg->from->id, ctime + blocked);

    cout << "Blocking user " << msg->from->firstName << "("<<msg->from->lastName<<","<<msg->from->id<<")";
    cout << " in chat " << msg->chat->firstName << " " << msg->chat->lastName << " at " << ctime << " for " << blocked;
    if (success) cout << " [success]";
    cout << endl;

    // tease the user
    const string str_failed("Failed to restrict you!");
    string str("Well done! You've been blocked for ");
    str += (blocked / 60); str += " seconds!";

    query.sendMessage(msg->chat->id, success?str:str_failed, false, msg->messageId);
  };

  observe.onCommand(string("suicide"), handleSucide);
  observe.onAnyMessage([&query](Message::Ptr msg) {
    cerr << "onAnyMessage: " << msg->chat->id << ":" << msg->messageId << endl;
    if (msg->chat->type != Chat::Type::Supergroup)
     query.sendMessage(msg->chat->id, "Expected to be sent in Supergroup, received " + nameOf(msg->chat->type));
    else query.sendMessage(msg->chat->id, "Unexpected command", false, msg->messageId);
  });

  cout << "Application bootup" << endl;
}

// launcher
int main() {
  const char *TOKEN_ENV = "TG_TOKEN";
  if (getenv(TOKEN_ENV) == NULL) {
    cerr << "Expected token set " << TOKEN_ENV << endl;
    exit(1);
  }

  BLOCK_MAX = atol(getenv("MIN_BLOCK"));
  BLOCK_MAX = atol(getenv("MAX_BLOCK"));

  string token(getenv(TOKEN_ENV));

  cout << "Telegram token: " << token << endl;

  HttpClient *httpc = getenv("USE_CURL") ? dynamic_cast<HttpClient*>(new CurlHttpClient()) : dynamic_cast<HttpClient*>(new BoostHttpOnlySslClient());
  Bot tg(token, *httpc);

  auto subscriber = tg.getEvents();
  auto api = tg.getApi();

  reg_signal_handers();
  application_run(subscriber, api);

  try {
    cout << "Tg bot :: " << api.getMe()->username << endl;

    api.deleteWebhook();

    TgLongPoll ml(tg);
    for (;;) {
      cout << "starting longpoll " << hex << (reinterpret_cast<void *>(&ml)) << " at " << dec << time(nullptr) << endl;
      ml.start();
      cout << "["<<time(nullptr)<<"]"<< " Long poll routine ended" << endl;
    }

  } catch (exception &e) { cerr << "Error! " << e.what() << endl; }

  return 0;
}
