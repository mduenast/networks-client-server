import Args_parser
import Commandes
import Configuration
import TCP_channel
import UDP_channel


class Servidor(object):

    def __init__(self):
        super(Servidor, self).__init__()

    def main(self):
        configuration = Configuration.Configuration()
        args_parser = Args_parser.Args_parser()
        args_parser.parse(configuration)

        commandes = Commandes.Commandes()
        commandes.configuracio = configuration
        commandes.start()

        canal_tcp = TCP_channel.TCP_channel()
        canal_tcp.configuracio = configuration
        canal_tcp.start()

        canal_udp = UDP_channel.UDP_channel()
        canal_udp.configuracio = configuration
        canal_udp.start()



if __name__ == "__main__":
    servidor = Servidor()
    servidor.main()