#include <unistd.h>
#include <stdio.h>
int main(int argc, char **argv, char **envp)
{
	printf("Content-Length: 100\r\nContent-Type: text/html\r\nStatus: 400\r\n\r\n<!DOCTYPE html>\n<html>\n<body>\n\n<h1>My First Heading</h1>\n<p>My first paragraph.</p>\n\n</body>\n</html>");
	return (0);
}
