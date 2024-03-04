def bytes_to_mb(bytes):
    return bytes / (1024 * 1024)

def compute_ratio_percentage(compressed_size, plain_size):
    return (compressed_size/plain_size)*100

def set_locale(language):
    if language == 'en':
        import constants_en as constants
    else:
        import constants_pt as constants
    return constants