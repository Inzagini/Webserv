import os
print("<h1>Hello World!</h1>")

print("Content-Type: text/plain\n")

# Print all environment variables (for debugging)
for key, value in os.environ.items():
	print(f"<p>{key} = {value}</p>")
