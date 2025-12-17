# Webserv

<p align="center">
  <img src="https://img.shields.io/badge/42%20School-Webserv-blue" alt="42 Webserv"/>
</p>

> **A lightweight HTTP/1.1 web server written in C++98 for the 42 School curriculum.**

---

## 🚀 Features

- **HTTP/1.1 compliant**: Handles `GET`, `POST`, `DELETE`, and more
- **Multiple server blocks**: Virtual hosts & flexible config
- **Static file serving**: HTML, CSS, images, etc.
- **CGI support**: Python CGI scripts for dynamic content
- **File uploads**: Upload files to a directory
- **Custom error pages**: 403, 404, 405, etc.
- **Directory listing**: Optional auto-indexing
- **Redirection**: Configurable HTTP redirects
- **Nginx-inspired config**: Easy `.conf` files

---

## 📁 Project Structure

```text
Webserv/
├── Makefile
├── src/           # C++ source files
├── inc/           # C++ headers
├── app/
│   ├── html/      # Static HTML files
│   ├── uploads/   # Upload directory
│   ├── cgi/       # Python CGI scripts
│   └── errorPages/# Custom error pages
├── conf.d/        # Configuration files
├── test.sh        # Test script
└── README.md
```

---

## ⚙️ Build & Run

### Prerequisites
- Linux
- C++98 compiler (e.g., `g++`)
- Python 3 (for CGI)
- `make`, `curl`

### Build
```sh
make
```

### Run
```sh
./webserv conf.d/default.conf
```
Or use the Makefile shortcut:
```sh
make t
```

---

## 📝 Configuration

- Config files: `conf.d/default.conf`, `conf.d/test.conf`
- Define multiple servers, locations, error pages, uploads, CGI, etc.
- Inspired by Nginx config style

---

## 🧪 Testing

Run the provided test script:
```sh
bash test.sh
```
This will use `curl` to test endpoints, methods, and features.

---

## 🌐 Example Endpoints

- `/` — Homepage
- `/upload` — File upload form
- `/cgi/cgiGET.py` — Example CGI GET
- `/cgi/cgiPOST.py` — Example CGI POST

---

## 👤 Authors

- Inzagini

---

## 📜 License

This project is for educational purposes as part of the 42 School curriculum.

---

<p align="center">
  <em>42 Webserv &copy; 2025</em>
</p>Webserv
