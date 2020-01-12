class HTTP
  def get(url)
    _get(url, url.start_with?('https://'))
  end

  def post(url, body, content_type)
    _post(url, body, content_type, url.start_with?('https://'))
  end
end
