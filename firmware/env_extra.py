import os

Import("env")

# Path to the .env file at the project root
env_file = os.path.join(env.get("PROJECT_DIR"), "..", ".env")

def load_dotenv(path):
    if not os.path.exists(path):
        print(f"\033[91mError: .env file not found at {path}\033[0m")
        return {}
    
    config = {}
    with open(path, "r") as f:
        for line in f:
            line = line.strip()
            # Ignore empty lines and comments
            if not line or line.startswith("#"):
                continue
            if "=" in line:
                key, value = line.split("=", 1)
                # Strip spaces and optional surrounding quotes
                config[key.strip()] = value.strip().strip("'\"")
    return config

# Load variables
dotenv_config = load_dotenv(env_file)

# Required mapping (Env Key -> C++ Macro)
macro_map = {
    "WIFI_SSID": "WIFI_SSID",
    "WIFI_PASSWORD": "WIFI_PASSWORD",
    "VITE_FIREBASE_API_KEY": "FIREBASE_API_KEY",
    "VITE_FIREBASE_DATABASE_URL": "FIREBASE_DATABASE_URL"
}

missing_keys = []

for env_key, macro_name in macro_map.items():
    value = dotenv_config.get(env_key)
    if value:
        # Wrap the value in double quotes for C++ string literals
        # We use escaped double quotes because this is passed to the compiler
        quoted_value = f'\\"{value}\\"'
        env.Append(CPPDEFINES=[(macro_name, quoted_value)])
    else:
        missing_keys.append(env_key)

# Fail fast if required variables are missing
if missing_keys:
    print(f"\033[91m\n[Build Error] Missing required variables in .env:\033[0m")
    for key in missing_keys:
        print(f"\033[91m  - {key}\033[0m")
    print("\033[93mPlease update your .env file before building.\n\033[0m")
    env.Exit(1)  # Abort the build directly
