### Overview ###

Transcendance is a full-stack web application that provides users with an interactive online gaming experience by implementing the classic Pong game with modern features. It is designed as part of the 42 school curriculum, focusing on web technologies, authentication, and real-time interactions.

### Features ###

- ✅ User Authentication – OAuth2 (42 API) and Two-Factor Authentication (2FA)
- ✅ Real-time Multiplayer Pong Game – Play 2v2 or 4v4
- ✅ Local tournament with matchmaking features
- ✅ User Profiles – Customizable profiles with avatars, username
- ✅ Leaderboard that updates after multiplayer games
- ✅ Add friends and track their online connection with sockets
- ✅ View user win and match history
- ✅ Security features such as JWT token and user input sanitization

### Config file usage ###

```conf
server {
    server_name     <name>;
    listen           <port>;
    root             <root directory>;
    default          <default file>;
    protocol         <allowed methods>;
    host             <IP address>;

    # Location blocks for different URL paths
    location / {
        methods <allowed methods>;
        root <root directory>;
        cgi <on/off>;
        autoindex <on/off>;
        upload_path <upload directory>;
    }
    
    # Additional location blocks for different paths (e.g., /cgi/)
}
```
### How to run the project

### 1. Clone the Repository
To get started, clone the Webserv repository to your local machine:
```bash
git clone <repository_url>
cd <repository_directory>
```
### 2. Build the project

Compile using the command make

### 3. Run the server

Without arguments will take the config.cf file:
```
./server
```
With arguments for a custom .cf file as follow:
```
./server <path_to_custom_config_file>
```

