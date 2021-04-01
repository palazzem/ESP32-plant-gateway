#ifndef MOCK_PUBSUBCLIENT_H
#define MOCK_PUBSUBCLIENT_H

#include <Client.h>

class PubSubClient {
public:
  PubSubClient();

  virtual PubSubClient &setClient(Client &client);
  virtual PubSubClient &setServer(const char *domain, int port);

  virtual bool publish(const char *topic, const char *payload);
  virtual bool connect(const char *id, const char *user, const char *pass);
  virtual bool connected();
  virtual void disconnect();
  virtual int state();
};

#endif
