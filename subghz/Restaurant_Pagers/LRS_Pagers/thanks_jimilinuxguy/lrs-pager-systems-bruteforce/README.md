# lrs-pager-systems-bruteforce
Long Range Pager Systems  pagers and coasters URH and YS1 (yardstick one / cc11xx) information and brute force tool
# lrs-pager-systems-bruteforce
Long Range Pager Systems  pagers and coasters URH and YS1 (yardstick one / cc11xx) information and brute force tool
Inspired by [Tony Tiger's prior work using the HackRF One](https://github.com/tony-tiger/lrs)

I used the [LRS Transmitter Tuneup manual](https://fccid.io/2AB6OTX1605/Parts-List-Tune-Up-Info/Tune-Up-Procedures-2357525) to verify the center frequency, deviation, modulation and 

This script requires nothing more than [RfCat](https://github.com/atlas0fd00m/rfcat) and console-menu

```
pip install -r requirements.txt
```

```
Frequency: 467.75 Mhz
Modulation: FSK
Encoding: Manchester
Rate: 626 baud
Samples/Symble: 3200
Sample Rate: 2M
```

This script will brute-force all restauraunt ids, blasting all pagers (pager id 0) with the command to flash for 30 seconds.

<details><summary>Alert Command Options</summary>
  
```
Alert Commands
1 Flash 30 Seconds
2 Flash 5 Minutes
3 Flash/Beep 5X5
4 Beep 3 Times
5 Beep 5 Minutes
6 Glow 5 Minutes
7 Glow/Vib 15 Times
10 Flash Vib 1 Second
68 beep 3 times
```
</details>

<details><summary>Reprogramming LRS pagers</summary>
When a pager is first removed from its charging cardle (or powered on) it goes through a 3-4 second Power On Self Test (POST) like operation. After this is complete you have 10 seconds to send a reprogramming command. If the command is sent after this window, the pager will ignore it.

You can re-program both the restaurant ID as well as the pager number in one command. Please DO NOT use this command against a system that it is yours. Yes it might be funny to set off all the pagers at a restaurant but reprogramming them isn't cool.

```
The protocol generating a pager alert packet is:
preamble header restaurant_id system_id pager_number 0000 0000 alert_type crc
The crc is given by taking the sum of the hex values and then taking the modulus  of the sum by 255 
For restaurant 0 and pager 0 (all) with an alert type of 1 the value would be
aaaaaafc2d0000000000000000012b
```
</details>


<details><summary>Flipper Zero custom preset modulation for decoding LRS pagers</summary>
  
```
Flipper Zero custom preset modulation for decoding LRS pagers:
Custom_preset_name: Pagers
Custom_preset_module: CC1101
Custom_preset_data: 02 0D 07 04 08 32 0B 06 10 64 11 93 12 0C 13 02 14 00 15 15 18 18 19 16 1B 07 1C 00 1D 91 20 FB 21 56 22 10 00 00 C0 00 00 00 00 00 00 00
```
</details>

<details><summary>Video of Flipper Zero sending LRS pager signal</summary>

[Video of Flipper Zero sending LRS Pager signal](https://user-images.githubusercontent.com/164560/197110407-72ee0c76-43b3-4316-8f29-14f44c3e1a8e.mov)
</details>

