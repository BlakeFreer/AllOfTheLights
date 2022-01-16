import requests
import time
for i in range(30):
    response = requests.get("http://192.168.0.35:5000/query")
    print(response.json())
    time.sleep(1)

# def hexToRGB(hex):
#     hex = hex.lstrip('#')
#     rgb= tuple(int(hex[i:i+2], 16) for i in (0, 2, 4))