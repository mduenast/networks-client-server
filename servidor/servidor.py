import Args_parser
import Commandes
import Configuration


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


if __name__ == "__main__":
    servidor = Servidor()
    servidor.main()