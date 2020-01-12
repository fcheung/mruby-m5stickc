class Menu
  PADDING_LEFT = 1
  PADDING_TOP = 2
  def initialize(choices, preselected:0, portrait: true)
    @selected = preselected
    @choices = choices
    if portrait
      LCD.rotate 4
    else
      LCD.rotate 1
    end
  end

  def run
    reset_screen    
    result = nil
    while true
      render
      choice = @choices.values[get_next_choice]
      reset_screen
      result=choice.call
      if result == :wait
        get_next_choice
      end
      reset_screen
      break if result == :stop
    end
  end

  def get_next_choice
    ButtonWatcher.new([ButtonWatcher::PIN_A, ButtonWatcher::PIN_B]).run(timeout: 10000) do |event|
      next unless event
      if event.pressed?
        case event.pin
        when ButtonWatcher::PIN_A
          return @selected
        when ButtonWatcher::PIN_B
          increment_choice
        end
      end
    end
  end

  def reset_screen
    LCD.text_color = LCD::BLACK
    LCD.background_color = LCD::WHITE
    LCD.fill_screen(LCD::WHITE)
    LCD.set_cursor(0, 0)
  end

  def render
    selected = "* "
    unselected = "  "
    @choices.each.with_index do |(label, _), index|
      prefix = index == @selected  ? selected : unselected

      LCD.draw_string(prefix + label.to_s, PADDING_LEFT, PADDING_TOP + 12 * index, 1)
    end
  end

  def increment_choice
    @selected += 1
    @selected = 0 if @selected >= @choices.length
    render
  end
end
