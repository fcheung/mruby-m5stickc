mirb = false
LCD.brightness=15
m = Menu.new(
  restart: -> { ESP32::System::restart},
  stats: ->{
    LCD.print "RUBY_VERSION: #{RUBY_VERSION}\n"
    LCD.print "MRUBY_VERSION: #{MRUBY_VERSION}\n"
    mem = ESP32::System.available_memory / 1000
    LCD.print "Available Memory: #{mem}K\n"
    :wait
  },
  mirb: -> { mirb = true; :stop},
  wifi: -> { 
    if Wifi.connected?
      LCD.print("Wifi connected")
    else
      Wifi.connect(WIFI_SSID, WIFI_PASSWORD)
    end
    :wait
  },
  logo: -> {LCD.draw_bitmap(0,0,80,81,LOGO_BYTES); :wait},
  callme: -> {
    h = HTTP.new
    h.authorize(TWILIO_USER,TWILIO_PASSWORD)
    url ="https://api.twilio.com/2010-04-01/Accounts/#{TWILIO_APP_ID}/Calls.json"
    data =  "To=#{TWILIO_TO_NUMBER}&From=#{TWILIO_FROM_NUMBER}&Url=http%3A%2F%2Fdemo.twilio.com%2Fdocs%2Fvoice.xml"
    code, message =  h.post(url,data,'application/x-www-form-urlencoded')
    LCD.print("http response: #{code.to_s}")
    puts message
    :wait
  }
)
m.run

if mirb
  #why is mirb not a regular option? Mirb.run crashses if not run from the toplevel & my limited knowledge of
  #mruby internals doesn't allow me to work out why
  LCD.clear
  Mirb.run
end
