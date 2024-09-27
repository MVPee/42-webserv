import os
import sys

if __name__ == "__main__":
	if (os.environ["REQUEST_METHOD"] != "POST"):
		print("HTTP/1.1 200 OK \r\nContent-Type: text/html;charset=utf-8\r\n\r\n  Not a post Request")
		sys.exit(1)
	content_len = int(os.environ["CONTENT_LENGTH"])
	content = sys.stdin.read(content_len)
	print(f'HTTP/1.1 200 OK \r\nContent-Type: text/html;charset=utf-8\r\n\r\n You posted: "{content}"')