# Unit Test:

## Include test about:
### 1. File transfer using socket
    + Install:
    ```
    make all
    ```
    + Run server node:
    ```
    ./server [server-port] # If don't include a port, the DEFAULT_PORT will be use (12345)
    ```
    + Run client node:
    ```
    ./client [server-host] [server-port] [path-to-file]
    ```
    + Clean up:
    ```
    make clean
    ```
### 2. Split and merge binary file