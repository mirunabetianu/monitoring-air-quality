## Project description

Arduino project that reads the data from a NodeMCU board and then sends it through Wifi to a Webpage.

![NodeMCU](https://images-na.ssl-images-amazon.com/images/I/51jT8afFtgL.jpg)

The components connected to the board are the following: 
* DHT22 (temperature and humidity)
* MQ135 (air quality)
* RGB led
* FAN (not implemented yet)

The data received from the board is sent to the webpage using WebSockets. All the data is parsed and then sent to the script.
The script receives the data, splits it and then plots on three different charts each values received. The data is sent each 2 seconds. The modification of the charts is real-time because of the usage of JavaScript. No refresh needed.
