#!/usr/bin/env python3
import sys, os

print ("""
<!DOCTYPE html>
<html lang="en">
<head>
    <meta charset="UTF-8" />
    <title>Run CGI Script</title>
    <style>
        body {
            font-family: Arial, sans-serif;
            text-align: center;
            padding-top: 50px;
            background-color: #f9f9f9;
            color: #333;
        }
        h1 {
            color: #2c3e50;
        }
        input[type="text"] {
            padding: 10px;
            font-size: 1rem;
            width: 300px;
        }
        button {
            padding: 10px 15px;
            font-size: 1rem;
            background-color: #3498db;
            color: white;
            border: none;
            cursor: pointer;
        }
        button:hover {
            background-color: #2980b9;
        }
        a {
            text-decoration: none;
            color: #3498db;
            font-weight: bold;
        }
        a:hover {
            text-decoration: underline;
        }
        .output {
            margin-top: 20px;
            font-size: 1.2rem;
            color: #2c3e50;
        }
    </style>
</head>
<body>
    <h1>Run a POST CGI Script</h1>
    <form action="/cgi/cgiPOST.py" method="post" enctype="multipart/form-data">
        <input type="file" name="file" required />
        <button type="submit">Upload</button>
        <br /><br />
    <a href="/">Return to Homepage</a>
    </form>
    <div class="output">
""")

content_length = int(os.environ.get('CONTENT_LENGTH', 0))
body = sys.stdin.read(content_length)

print(f"""
<textarea readonly style="width:90%;height:300px;resize:vertical;overflow:auto;border:1px solid #ccc;margin-top:20px;">
{body.replace("&", "&amp;").replace("<", "&lt;").replace(">", "&gt;")}
</textarea>
""")

print("""
</div>
</body>
</html>
""")
