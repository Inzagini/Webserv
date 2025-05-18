#include "config.hpp"
#include "server.hpp"
#include "request.hpp"

int main ()
{
	Config conf;
	std::string filename = std::string("./conf.d/default.conf");
	conf.load(filename);

	// int server_id = setSocket(conf.getServer()[0]);
	// connectionHandle(server_id);

	char request[] =
    "GET / HTTP/1.1\r\n"
    "Host: 127.0.0.1:8080\r\n"
    "Connection: keep-alive\r\n"
    "sec-ch-ua: \"Chromium\";v=\"136\", \"Brave\";v=\"136\", \"Not.A/Brand\";v=\"99\"\r\n"
    "sec-ch-ua-mobile: ?0\r\n"
    "sec-ch-ua-platform: \"Windows\"\r\n"
    "Upgrade-Insecure-Requests: 1\r\n"
    "User-Agent: Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/136.0.0.0 Safari/537.36\r\n"
    "Accept: text/html,application/xhtml+xml,application/xml;q=0.9,image/avif,image/webp,image/apng,*/*;q=0.8\r\n"
    "Sec-GPC: 1\r\n"
    "Accept-Language: en-US,en;q=0.8\r\n"
    "Sec-Fetch-Site: none\r\n"
    "Sec-Fetch-Mode: navigate\r\n"
    "Sec-Fetch-User: ?1\r\n"
    "Sec-Fetch-Dest: document\r\n"
    "Accept-Encoding: gzip, deflate, br, zstd\r\n"
	"Content-Lenght: 27"
    "\r\n"
	"name=John+Doe&age=30&job=dev";

	HttpRequest req = parseHttpRequest(request);
	std::string	rootPath = "/app/html";
	std::string full_response = serveFileRequest(1, req, rootPath);
	std::cout << "[Response]\n" << full_response << std::endl;
	// send(client_fd, full_response.c_str(), full_response.length(), 0);
	// printHttpRequest(req);

	return EXIT_SUCCESS;
}
