import os

print("<h1> Hello World </h1>\n")

print("Content-Type: text/plain\n")



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
        .output {
            margin-top: 20px;
            font-size: 1.2rem;
            color: #2c3e50;
        }
    </style>
</head>
<body>
    <h1>Run a CGI Script</h1>
    <form action="/cgi/cgi.py" method="post" enctype="multipart/form-data">
        <input type="file" name="file" required />
        <button type="submit">Upload</button>
    </form>
    <div class="output">
""")

# Print all environment variables (for debugging)
for key, value in os.environ.items():
	print(f"<p>{key} = {value}</p>")

print("""
	</div>
</body>
</html>
""")
