#include "main.hpp"

int g_signum = 0;

void handle_sigint(int sig, siginfo_t *info, void *context) {
    (void)info;
    (void)context;

    if(sig)
    {
        std::cerr << "\n Signal reçu. Arrêt du serveur..." << std::endl;
        g_signum = 1;
    }
}


int main(int argc, char **argv)
{
	Webserv		poll_test;
	int			err = 0;

	try
	{
		if (argc < 2)
		{
			if (parse_config("config.cf", poll_test))
				return (0);
		}
		else
		{
			if (parse_config(argv[1], poll_test))
				return (0);
		}
	}
	catch(std::exception& e)
	{
		return (0);
	}
	signal(SIGPIPE, SIG_IGN);
	signal(SIGPIPE, SIG_IGN);
	struct sigaction sa;
    sa.sa_flags = SA_SIGINFO;
    sa.sa_sigaction = handle_sigint; 
    sigemptyset(&sa.sa_mask);
    if (sigaction(SIGINT, &sa, NULL) == -1) {
        if (SIGINT)
        {
            herror("sigaction");
            return 0;
        }
    }
	try
	{
		while (1)
		{
			if ((err = poll(poll_test.c_arr(), poll_test.get_size(), 0)) > 0)
			{
				poll_test.new_connect(err);
				poll_test.handle_recv(err);
			}
			if (g_signum == 1) {
        		break; 
    		}
		}
	}
	catch(const std::exception& e)
	{
		std::cerr << e.what() << '\n';
	}
}