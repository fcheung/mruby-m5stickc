require 'chunky_png'

#
# converts a png image to the format expected by the device 
#
image = ChunkyPNG::Image.from_file(ARGV[0])

pixels_in_565_format= image.pixels.map do |pix|
  r = (pix & 0xFF000000)>>24
  g = (pix & 0xFF0000)>>16
  b = (pix & 0xFF00)>>8

  r/= 8
  g/=4
  b/=8

  r<<11 | g << 5 | b
end

puts pixels_in_565_format.pack('S*').inspect
