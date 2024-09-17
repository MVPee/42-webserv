import os
import sys

if __name__ == "__main__":
	if (os.environ["REQUEST_METHOD"] != "POST"):
		print("Not a post Request")
		sys.exit(1)
	content_len = int(os.environ["CONTENT_LENGTH"])
	content = sys.stdin.read(content_len)
	print(f'You posted: "{content}"')