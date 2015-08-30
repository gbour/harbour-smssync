# harbour-smssync
SMS synchronization daemon for Jolla devices

Everything is connected now. You want to be able to read the sms you received on your phone
directly on your computer, or on your tablet.

**smssync** fill the gap, allowing to synchronize your sms with you desktop computer or your tablet.



**WARNING: this sofware is in alpha stage. use at your own risks**

* no configuration interface yet
* requires a dedicated, reacheable MQTT server (such as *mosquitto*)
* no MQTT/SSL nor authentication support yet

! do not use it for sensible informations, or only on a local network you control !

## Configuration ##

### jolla application

* install the application either through Warehouse or by downloading & installing rpm file directly
* edit /etc/xdg/harbour-smssync/harbour-smssyncd.conf and set your correct values

```
        [mqtt]
        # mqtt server ip address
        server="127.0.0.1"
        # mqtt server port
        port=1883

        # device name (used in MQTT topic)
        deviceid="jolla"
        # MQTT keepalive and ping delay (in seconds)
        keepalive=100
```
* restart smssyncd daemon (you need to open a shell on your jolla device)
```
$> systemctl --user restart harbour-smssyncd
```

*ENJOY :)*

### desktop side

you can use any MQTT client to *read* sms received on/sent from your Jolla phone.
Here is an example using mosquitto client:
```
~$>  mosquitto_sub -h localhost -t smssync/#
{"content": "What's up, bro ?","dir": "in","from": "+33XXXXXXXXX","id": "","type": "msg"}

```


For convenience, I provide a simple python client in *desktop-notification-client/* directory.
This client display *sent & received sms * as desktop notifications (for 20 seconds).

It connects by default to mqtt server *localhost:1883*


```
~/harbour-smssync $> ./desktop-notification-client/smssync-notify [mqtt-server:port]
```

![smssync-notify](https://raw.githubusercontent.com/gbour/harbour-smssync/master/desktop-notification-client/resources/screenshot.png)

## Roadmap

* configuration interface
* send sms from remote device
* sms read status sync (jolla -> desktop and desktop -> jolla)
* SSL encryption for MQTT notifications
* MQTT authentication
* end-to-end encryption
* feature-full desktop client
* phone calls events notification & action (hangup)

## License

*harbour-smssync* is distributed under GPL3 license terms.

## About

* main author: Guillaume Bour <guillaume@bour.cc>
* icon: derived from https://openclipart.org/detail/77119/smstext by shokunin
