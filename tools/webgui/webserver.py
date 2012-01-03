import tornado
import struct
import socket
import tornado.httpserver
import tornado.ioloop
import tornado.web
from tornado import websocket

class WebSocket(websocket.WebSocketHandler):
    def open(self):
        print 'open websocket'

    def close(self):
        print 'closing websocket'

    def __parse_message(self, message):
        message = message.split(' ')

        cmds = {'up': 1,
                'down': 2,
                'left': 3,
                'right': 4,
                'stop': 5,
                'zoomin': 6,
                'zoomout': 7,
                'spacestation': 8,
                'goto': 9,
                'rotateleft': 10,
                'rotateright': 11,
                'rotateup': 12,
                'rotatedown': 13,
                'rotatefront': 14,
                'rotateback': 15}

        if not message[0] in cmds.keys():
            return None
        else:
            cmd = cmds[message[0]]
            if cmd == 'goto':
                lat, log = float(message[1]), float(message[2])
            else:
                lat, log = 0.0, 0.0
            return struct.pack("lff", cmd, lat, log)

    def on_message(self, message):
        print 'message: ', repr(message)
        message = self.__parse_message(message)
        if message:
            UDPSock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
            UDPSock.sendto(message, ('127.0.0.1', 6666))
            self.write_message(u"Sent to server")
        else:
            self.write_message(u"Wrong command")

application = tornado.web.Application([(r"/world", WebSocket)])

if __name__ == "__main__":
    http_server = tornado.httpserver.HTTPServer(application)
    http_server.listen(8888)
    tornado.ioloop.IOLoop.instance().start()

