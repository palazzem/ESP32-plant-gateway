#ifndef MOCK_PUBSUBCLIENT_H
#define MOCK_PUBSUBCLIENT_H

#include <Client.h>

class PubSubClient {
public:
  PubSubClient();

  PubSubClient &setClient(Client &client);
  PubSubClient &setServer(const char *domain, int port);

  bool publish(const char *topic, const char *payload);
  bool connect(const char *id, const char *user, const char *pass);
  bool connected();
  void disconnect();
  int state();
};

#endif
