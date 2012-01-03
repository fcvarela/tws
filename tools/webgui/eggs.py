from SAPO import Broker
import GeoIP
import socket
import struct
from datetime import datetime
import simplejson

GI = GeoIP.open("GeoIPCity.dat", GeoIP.GEOIP_STANDARD)

def get_coords(data, ipadd):
    gir = GI.record_by_addr(ipadd)
    if gir is None:
        return
    
    cords = 'x'.join([str(gir['latitude']), str(gir['longitude'])])

    try:
        data[cords] += 1
    except KeyError:
        data[cords] = 1

def normalize_data(data):

    maximum = max(data.values())
    norm_data = {}
    for key, value in data.iteritems():
        norm_data[key] = value / float(maximum)
    return norm_data

def send_data(data):
    print 'sending data'
    norm_data = normalize_data(data)

    data = [(key, value) for key, value in norm_data.iteritems()]
    data.sort(lambda x,y: cmp(y[1],x[1]))
    data = data[:100]
    sdata = []
    data = dict(data)
    for key, value in data.iteritems():
        lat, lon = key.split("x")
        sdata.append(float(value))
        sdata.append(float(lat))
        sdata.append(float(lon))

    fmt = 'fff' * len(data.keys())
    message = struct.pack(fmt, *sdata)

    UDPSock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
    UDPSock.sendto(message, ('127.0.0.1', 6667))

def load_data():
    try:
        f = open('spam_and_eggs.json')
        data = simplejson.loads(f.read())
        f.close()
        return data
    except IOError, ioe:
        print "rer: %r" % (ioe)
        return None

def dump_data(data):
    f = open('spam_and_eggs.json', "w")
    f.write(simplejson.dumps(data))
    f.close()

def main():

    client = Broker.Client('broker.labs.sapo.pt', 3322)
    client.subscribe('/ptmail/spam/block/ip', 'TOPIC')
    data = load_data()
    if data is None:
        data = {}
    start = datetime.now()
    while True:
        message = client.consume()
        country, ipadd = message.payload.split("|")
        get_coords(data, ipadd)

        now = datetime.now()
        if (now - start).seconds >= 2:
            send_data(data)
            dump_data(data)
            start = now


if __name__ == '__main__':
    main()





