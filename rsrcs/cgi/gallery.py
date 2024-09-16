import os
import cgi

image_folder = "./../upload"

def get_file_size(file_path):
    """Returns the size of the file in a human-readable format."""
    try:
        size_bytes = os.path.getsize(file_path)
        if size_bytes < 1024:
            return f"{size_bytes} B"
        elif size_bytes < 1024 * 1024:
            return f"{size_bytes / 1024:.1f} KB"
        elif size_bytes < 1024 * 1024 * 1024:
            return f"{size_bytes / (1024 * 1024):.1f} MB"
        else:
            return f"{size_bytes / (1024 * 1024 * 1024):.1f} GB"
    except Exception as e:
        return "Unknown Size"

def delete_file(file_name):
    """Deletes a file from the server."""
    file_path = os.path.join(image_folder, file_name)
    try:
        os.remove(file_path)
        return True
    except Exception as e:
        return False


form = cgi.FieldStorage()
if os.environ.get('REQUEST_METHOD') == 'DELETE':
    file_to_delete = form.getvalue("delete_file")
    if file_to_delete and delete_file(file_to_delete):
        print("Location: ./cgi/gallery.py\n")  # Redirect to the gallery page after deletion
        print()  # End of headers

print("""<!DOCTYPE html>
<html lang="en">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <link rel="stylesheet" href="../gallery.css">
    <title>Image Gallery</title>
    <script>
		async function deleteFile(fileName) {
			try {
				const response = await fetch(`/upload/${fileName}`, {
					method: 'DELETE',
					headers: {
						'Content-Type': 'application/json', // Use appropriate content-type if needed
						// Add any other headers if necessary
					},
				});

				if (!response.ok) {
					throw new Error(`HTTP error! Status: ${response.status}`);
				}

				// Handle the response if necessary
				console.log('File successfully deleted');
     			location.reload()
			} catch (error) {
				console.error('Error deleting file:', error);
			}
		}
	</script>
</head>
<body>
    <div class="file-container">
""")

# Print images
for image in os.listdir(image_folder):
    if not image.startswith('.'):
        image_path = os.path.join(image_folder, image)
        encoded_image = image.encode('utf-8').decode('utf-8')
        file_extension = os.path.splitext(image)[1].lower()
        
        is_image = file_extension in ['.jpg', '.jpeg', '.png', '.gif']
        
        if is_image:
            preview_html = f'<img src="{image_path}" alt="{encoded_image}" class="file-preview">'
        else:
            preview_html = f'<div class="file-icon pdf-icon"></div>'
        
        file_size = get_file_size(image_path)
        
        print(f'''
            <div class="file-item">
                {preview_html}
                <div class="file-details">
                    <h3 class="file-name">{encoded_image}</h3>
                    <p class="file-size">{file_size}</p>
                    <button onclick="deleteFile('{encoded_image}')" class="delete-button">Delete</button>
                </div>
            </div>
        ''')

print("""
    </div>
</body>
</html>
""")
