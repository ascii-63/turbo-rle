# Turbo Run-Length Encoding Compression
Efficient Parallelization of Run-Length Encoding (RLE) Compression using Single Instruction, Multiple Data (SIMD) and Master-Workers Model in Cloud-Based Computing

+ Author: https://github.com/acsii-63
+ Contact: contact.nqduy@gmail.com
+ With heartfelt gratitude, a special thanks to: https://github.com/powturbo

## Requirement
```
sudo apt install cat make gcc
```
## Install:
+ Clone this repository
```
git clone https://github.com/acsii-63/turbo-rle.git
```
+ Create storage folder
```
cd <turbo-rle-path>
mkdir -p files/temp
```
+ Set permission for bash file:
```
sudo chmod +x bash/*
sudo chmod +x master.sh
```
+ Build:
```
make all
```
## Usage:
+ Firstly, config the ```master.sh``` file with worker IP host
+ Secondly, run this command for each worker instance (for worker #i):
```
./worker 10000 <master-host> && rm -f files/*
```
Next instance (i+1) will be port 10001 and so on.
+ Master instance:
```
bash master.sh
```