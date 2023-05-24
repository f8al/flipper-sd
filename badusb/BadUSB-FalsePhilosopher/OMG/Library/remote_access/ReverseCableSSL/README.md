**Title: ReverseCableSSL**

<p>Author: 0iphor13<br>
OS: Windows<br>
Version: 1.0<br>
Requirements: OMG Firmware v.2.5 or higher</p>

**What is ReverseCableSSL?**
#
<p>ReverseCableSSL gets you remote access to your target in seconds.<br>
Unlike ReverseCable, ReverseCableSSL offers encrypted traffic via OpenSSL.</p>


**Instruction:**
<p>!!!Insert the IP of your attacking machine & PORT into the payload at the beginning!!!<br>
1. Create key.pem & cert.pem like so: <br>
	> openssl req -x509 -newkey rsa:4096 -keyout key.pem -out cert.pem -days 365 -nodes<br>
	It will ask for information about the certificate - Insert whatever you want.<br>

2. For catching the shell you need to start a listener, which supports encrypted traffic.<br>
I recommend openssl itself or ncat - Example syntax for both:<br>
	> `openssl s_server -quiet -key key.pem -cert cert.pem -port [Port Number]` <br>
	> `ncat --listen -p [Port Number] --ssl --ssl-cert cert.pem --ssl-key key.pem`</p>

3. Plug in Cable.

![alt text](https://github.com/0iphor13/omg-payloads/blob/master/payloads/library/remote_access/ReverseCableSSL/CreateCert.png)
![alt text](https://github.com/0iphor13/omg-payloads/blob/master/payloads/library/remote_access/ReverseCableSSL/StartScreen.jpg)
