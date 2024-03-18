def text_to_c_string(long_text, segment_length):
    segments = [long_text[i:i+segment_length] for i in range(0, len(long_text), segment_length)]
    c_string = '"\n"'.join(segments)
    c_string = '"' + c_string + '"'
    return c_string

# Example usage:
long_text = "abcdefghijk"
segment_length = 5
c_string = text_to_c_string(long_text, segment_length)
print(c_string)
