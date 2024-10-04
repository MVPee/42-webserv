<h1 align="center">
	📖 42-Webserv
</h1>

<p align="center">
	<img alt="GitHub code size in bytes" src="https://img.shields.io/github/languages/code-size/MVPee/42-webserv?color=lightblue" />
	<img alt="Code language count" src="https://img.shields.io/github/languages/count/MVPee/42-webserv?color=yellow" />
	<img alt="GitHub top language" src="https://img.shields.io/github/languages/top/MVPee/42-webserv?color=blue" />
	<img alt="GitHub last commit" src="https://img.shields.io/github/last-commit/MVPee/42-webserv?color=green" />
</p>

<h3 align="center">
	<a href="#-about-the-project">About</a>
	<span> · </span>
	<a href="#-running-the-server">Running</a>
	<span> · </span>
	<a href="#-configurating-the-server">Configuration</a>
	<span> · </span>
	<a href="#-key-features">Key features</a>
</h3>


# 💡 About the project
>This is when you finally understand why a URL starts
with HTTP.

This project is about writing a non-blocking HTTP/1.1 server written in C++, designed to handle multiple simultaneous connections efficiently using asynchronous I/O. The server adheres to the HTTP/1.1 protocol and supports essential features such as GET, POST, and error handling, ensuring responsive communication without stalling the main execution thread.

![gallery](./.imgs/gallery.png)

## 🚀 Running the server 

```bash
make run
```
It will automatically compile the project and run it.
```bash
./webserv [configuration file]
```
Will launch the previously compiled project with `make` and run it. (configuration file must be a valid `.conf` file see section below)

![Servers](./.imgs/Servers.png)

## 💻 Configurating the server

</br>It will allow you to setup the following parameters :
- `name` -- define the server name
- `listen`:`port` -- to configure on which port the server will be listening (only one `listen` per `server` block)
- `methods` -- to set the authorized HTTP request (`GET` | `POST` | `DELETE`)
- `location` -- to set parameters for a specific route
- `index` -- to serve a default file if none match the requested URI
- `root` -- to route the requested URI to another directory
- `upload` -- to set a specific directory for upload requests
- `listing` -- to list all files present in the targeted directory (`ON` | `OFF`)
- `error` -- to define the page to display when an error occurs, add a default page if none is provided in the `.conf` file.
- `cgi` -- to configure a cgi to execute for a specific file extenstion (`.php` | `.py` | `.pl`)
- `redirection` -- to define a redirection to a location or to an another server
```
{
    name server
    listen 0.0.0.0:8080
    methods GET POST
    body_size 1000000000000
    root ./rsrcs
    upload /upload
    index index.html
    error 404 404.html
    location /cookie
        root ./rsrcs/cookie
        index cookie.html
        methods GET
    location /cgi
        root ./rsrcs/cgi
        listing ON
        cgi .py .php
        methods GET POST
    location /intra
        root ./rsrcs/intra
        index intra.html
        methods GET
    location /listing
        listing ON
        error 403 403.html
        error 404 404.html
        root ./rsrcs/listing
        methods GET
    location /listing/403.html
        root ./rsrcs/listing/403.html
    location /listing/404.html
        root ./rsrcs/listing/404.html
    location /listing/1/2/3
        redirection /listing
    location /upload
        root ./rsrcs/upload
        methods GET DELETE
    location /redirection
        redirection https://fr.wikipedia.org/
}
```
------------
## 📜 Key Features

- **Configuration Flexibility**  
  -- Accepts a custom configuration file or defaults to a predefined path

- **Non-Blocking Architecture**  
  -- Uses a single `select()` (or equivalent) per server for all I/O operations, including listening and client communication

- **Efficient Resource Management**  
  -- Handles all read/write operations via `select()`, optimizing resource usage and **preventing blocking**

- **HTTP Methods**  
  -- Supports `GET`, `POST`, and `DELETE`

- **File Uploads**  
  -- Allows clients to upload files directly to the server

- **Static File Serving**  
  -- Serves static websites

- **Accurate HTTP Responses**  
  -- Delivers precise status codes along with a specified error page or a default one

- **CGI Integration**  
  -- Supports CGI execution (e.g., `PHP`, `Python`, `Perl`, ...)

- **Multi-Port Listening**  
  -- Listens on multiple ports as defined in the configuration file

- **Browser Compatibility**  
  -- Compliant with HTTP 1.1 standards.

- **Handle Cookies**  
  -- A small cookie clicker has been made to show the implementation of the cookies

- **Keep-alive connection**  
  -- Connection is kept alive if possible

- **Time out**  
  -- Timeout for CGI and basic requests

---

![Cookie-clicker](./.imgs/Cookie-clicker.png)

---
This group project was done by [MVPee (mvan-pee)](https://github.com/MVPee) and [Dan (dspilleb)](https://github.com/dspilleb)
