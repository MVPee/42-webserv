<h1 align="center">
	📖 Webserv | 42 School
</h1>

<p align="center">
	<img alt="GitHub code size in bytes" src="https://img.shields.io/github/languages/code-size/MVPee/42-webserv?color=lightblue" />
	<img alt="Code language count" src="https://img.shields.io/github/languages/count/MVPee/42-webserv?color=yellow" />
	<img alt="GitHub top language" src="https://img.shields.io/github/languages/top/MVPee/42-webserv?color=blue" />
	<img alt="GitHub last commit" src="https://img.shields.io/github/last-commit/MVPee/42-webserv?color=green" />
</p>

<h3 align="center">
	<a href="#-Running%20the%20server">Running</a>
	<span> · </span>
	<a href="#-congigurating%20the%20server">Configuration</a>
	<span> · </span>
</h3>


# 💡 About the project
>This is when you finally understand why a URL starts
with HTTP.

This project is about writing a HTTP/1.1 server.

[//]: #TODO (inclure des images)

## 🚀 Running the server 

```bash
make run
```
It will automatically compile the project and run it.
```bash
./webserv [configuration file]
```
Will launch the previously compiled project with `make` and run it. (configuration file must be a valid `.conf` file see section below)


## 💻 configurating the server

</br>It will allow you to setup the following parameters :
- `name` -- define the server name
- `listen` -- to configure on which port the server will be listening (only one `listen` per `server` block).
- `methods` -- to set the authorized HTTP request (`GET` | `POST` | `DELETE`)
- `location` -- to set parameters for a specific route.
- `index` -- to serve a default file if none match the requested URI.
- `root` -- to route the requested URI to another directory.
- `upload` -- to set a specific directory for upload requests.
- `listing` -- to list all files present in the targeted directory (`ON` | `OFF`).
- `error` -- to define the allowed methods for HTTP requests.
- `cgi` -- to configure a cgi to execute for a specific file extenstion (`.php` | `.py` | `.cgi`).

------------
## 📜 Key Features

- **Configuration Flexibility**  
  Accepts a custom configuration file or defaults to a predefined path.

- **Non-Blocking Architecture**  
  Uses a single `poll()` (or equivalent) for all I/O operations, including listening and client communication.

- **Efficient Resource Management**  
  Handles all read/write operations via `poll()`, optimizing resource usage and preventing blocking.

- **HTTP Methods**  
  Supports `GET`, `POST`, and `DELETE` for flexible web interactions.

- **File Uploads**  
  Allows clients to upload files directly to the server.

- **Static File Serving**  
  Serves static websites seamlessly.

- **Accurate HTTP Responses**  
  Delivers precise status codes and default error pages for a smooth user experience.

- **CGI Integration**  
  Supports CGI execution (e.g., PHP, Python), enabling dynamic content while remaining non-blocking.

- **Multi-Port Listening**  
  Listens on multiple ports as defined in the configuration file.

- **Browser Compatibility**  
  Fully compliant with modern web browsers and HTTP 1.1 standards.

- **Resilience Under Load**  
  Stress-tested to maintain availability under high demand.