# Dooglebot

This is the code that runs on the Thing Plus - ESP32-S2 WROOM and is used to gather the data powering Doogledoor (code here, website here).

The logic is all in `dooglebot.ino` where we:

- Connect to the 2.4GHz band of the WiFi network at my house
- Watch for the digital signal from the OpenPIR module that's monitoring motion to toggle from low to high
- Send a POST request to the Flask application hosting the web interface on Cloud Run (with a healthy timeout set to allow for the Cloud Run instance to boot up in the likely event that it's shut itself down)

All of the parts for this project are:

- [Thing Plus - ESP32-S2 WROOM](https://www.sparkfun.com/products/17743)
- [OpenPIR motion sensor](https://www.sparkfun.com/products/13968)
- [Waterproof enclosure](https://www.amazon.com/dp/B0BZR4VPRF?ref=ppx_yo2ov_dt_b_product_details&th=1)
- USB A to USB C adapter + charger

All of this is sitting right outside the dog door to the backyard. To read more, check out the [About page on Doogledoor](https://doogledoor.com/about).

Note: If you are viewing this project for inspiration, many of the sensitive credentials (wifi name, password, the REST endpoint and access token for the application, etc) are held in a header file - `Sensitive.h` - that is ignored from version control. You'll need to recreate this file locally yourself.
