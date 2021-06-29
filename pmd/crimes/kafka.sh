sudo -v
sudo zookeeper-server-start.sh /usr/lib/kafka/config/zookeeper.properties & disown
sudo kafka-server-start.sh /usr/lib/kafka/config/server.properties & disown

